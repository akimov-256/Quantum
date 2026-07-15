/*
* Quantum, A free and open-source download manager that is written in C++
* Copyright (C) 2025  Yovsky <Yovsky@proton.me>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "downloader.h"
#include <QtNetwork/QNetworkRequest>
#include <QUrl>
#include <QTimer>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void Downloader::download(downloadInformations Info)
{
    info = Info;
    isCancelling = false;
    m_chunksCompleted = 0;
    info.currentSize = 0;
    m_tempPaths.clear();
    m_url = QUrl(Info.url);

    m_qdmTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Quantum";
    info.ID = QUuid::createUuid().toString(QUuid::WithoutBraces);
    QDir dir;
    dir.mkpath(m_qdmTempDir + "/" + info.ID);

    info.tempPath = m_qdmTempDir + "/" + info.ID + "/" + info.fileName + ".qdm";
    m_file.setFileName(info.tempPath);
    m_file.resize(info.fileByteSize);

    QNetworkRequest request(m_url);
    QNetworkReply *reply = manager->head(request);
    connect (reply, &QNetworkReply::finished, this, &Downloader::onHeadFinished);
}

void Downloader::onHeadFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Failed to contact target host server:" << reply->errorString();
        return;
    }
    info.fileByteSize = reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    if (info.fileByteSize < 1024 * 1024 * 10) info.chunkCount = 1;

    // set part count, populate the parts array and add a one part at the end if the last part is not divisible by 4Mb
    int partCount = (info.fileByteSize % 4194304 == 0) ? info.fileByteSize / 4194304 : info.fileByteSize / 4194304 + 1;
    info.fileParts.resize(partCount);
    for (int i = 0; i < info.fileParts.size(); i++)
    {
        info.fileParts[i].start = i * 4194304;
        info.fileParts[i].end = info.fileParts[i].start + 4194304;
    }

    // set the end of the last part to the end of the file
    info.fileParts[partCount - 1].end = info.fileByteSize - 1;

    QByteArray acceptRanges = reply->rawHeader("Accept-Ranges");
    if (acceptRanges.toLower().contains("bytes") || info.chunkCount == 1)
    {
        qDebug() << "chunk count :" + QString::number(info.chunkCount);
        chunkProgress.resize(info.fileParts.size());
        WriteDownloadData();
        StartDataTimer();
        SetupWorkers();
    }
    else
    {
        QNetworkRequest request(m_url);
        QByteArray rangeHeader = "bytes=0-0";
        request.setRawHeader("Range", rangeHeader);
        QNetworkReply *test = manager->get(request);
        connect (test, &QNetworkReply::finished, this, &Downloader::onHeadTestFinished);
    }
}

void Downloader::onHeadTestFinished()
{
    QNetworkReply *test = qobject_cast<QNetworkReply*>(sender());
    if (!test) return;
    test->deleteLater();
    if (test->error() != QNetworkReply::NoError)
        info.chunkCount = 1;
    else if (test->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 206)
        info.chunkCount = 1;
    chunkProgress.resize(info.fileParts.size());
    WriteDownloadData();
    StartDataTimer();
    SetupWorkers();
}

void Downloader::WriteDownloadData()
{
    QFile dataFile(m_qdmTempDir + "/" + info.ID + "/" + QFileInfo(info.savePath).fileName() + ".qdmtemp");
    if (!dataFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        emit downloadFinished(false, "Failed to store download data.");
        return;
    }

    QJsonObject root;
    root["url"] = m_url.toString();
    root["downloadID"] = info.ID;
    root["savePath"] = info.savePath;
    root["chunkCount"] = info.chunkCount;
    root["fileSize"] = info.fileByteSize;

    QJsonArray chunks;
    for (qint64 progress : chunkProgress)
    {
        chunks.append(progress);
    }
    root["chunks"] = chunks;

    // save the file parts informations
    QJsonArray partsArray;
    for (const Part &part : info.fileParts)
    {
        QJsonObject obj;
        obj["start"] = part.start;
        obj["end"] = part.end;
        obj["used"] = part.used;
        obj["done"] = part.done;

        partsArray.append(obj);
    }
    root["parts"] = partsArray;

    QJsonDocument doc(root);
    dataFile.write(doc.toJson());

    // Used .qdmtemp then renamed it to ensure .qdmdata is created successfully and is not corrupted.
    dataFile.close();
    QFile::remove(m_qdmTempDir + "/" + info.ID + "/" + QFileInfo(info.savePath).fileName() + ".qdmdata");
    if (!dataFile.rename(m_qdmTempDir + "/" + info.ID + "/" + QFileInfo(info.savePath).fileName() + ".qdmdata"))
    {
        emit downloadFinished(false, "Failed to store download data.");
        return;
    }
}

void Downloader::StartDataTimer()
{
    if (!saveTimer)
    {
        saveTimer = new QTimer(this);
        connect(saveTimer, &QTimer::timeout, this, &Downloader::WriteDownloadData);
    }
    saveTimer->start(5000);
}

void Downloader::SetupWorkers()
{
    for (int i = 0; i < info.chunkCount; i++)
    {
        // Set up the worker and thread and add them to the tracking list
        QThread *workerThread = new QThread(this);
        DownloadWorker *worker = new DownloadWorker();
        worker->moveToThread(workerThread);

        m_workers.append(worker);
        m_workerThreads.append(workerThread);

        // Set the first threads to the appropriate parts
        connect(workerThread, &QThread::started, worker, [=]() {
            worker->StartDownload(i, info.fileParts[i].start, info.fileParts[i].end, false, info);
            info.fileParts[i].used = true;
        });

        // Connect signals
        connect(worker, &::DownloadWorker::Progress, this, &Downloader::onChunkProgress);
        connect(worker, &::DownloadWorker::Finished, this, &Downloader::onChunkFinished);
        connect(worker, &DownloadWorker::ErrorOcc, this, &Downloader::onWorkerError);

        workerThread->start();
    }
}

void Downloader::onChunkProgress(int chunkIndex, qint64 bytes)
{
    info.currentSize += bytes;
    if (chunkIndex >= 0 && chunkIndex < chunkProgress.size())
        chunkProgress[chunkIndex] += bytes;
    emit progressChanged(info.currentSize, info.fileByteSize);
}

void Downloader::onChunkFinished(DownloadWorker *worker, bool wasStopped)
{
    if (wasStopped)
    {
        retireWorker(worker);
        return;
    }

    info.fileParts[worker->m_chunkIndex].done = true;

    for (int i = 0; i < info.fileParts.size(); i++)
    {
        // Assign the next available thread to the free worker
        if (!info.fileParts[i].used)
        {
            info.fileParts[i].used = true;
            QMetaObject::invokeMethod(worker, [=]() {
                worker->StartDownload(i, info.fileParts[i].start, info.fileParts[i].end, false, info);
            }, Qt::QueuedConnection);
            return;
        }
    }

    retireWorker(worker);
    if (m_workers.isEmpty())
        handleDownloadFinish();
}

void Downloader::retireWorker(DownloadWorker *worker)
{
    // Handle worker and thread deletion
    m_workers.removeOne(worker);
    QThread *thread = worker->thread();
    m_workerThreads.removeOne(thread);
    thread->quit();
    thread->wait();
    thread->deleteLater();
    worker->deleteLater();
}

void Downloader::handleDownloadFinish()
{
    if (saveTimer) saveTimer->stop();

    m_file.rename(info.savePath);

    if (!info.SHA256.isEmpty())
    {
        QFile verFile(info.savePath);
        if (!verFile.open(QIODevice::ReadOnly))
        {
            emit downloadFinished(false, "Failed to verify file hash.");
            return;
        }

        QByteArray hash = QCryptographicHash::hash(verFile.readAll(), QCryptographicHash::Sha256);
        if (hash.toHex().toLower() != info.SHA256.toLower())
        {
            verFile.close();
            QFile::remove(info.savePath);
            emit downloadFinished(false, "File hash does not meet provided hash.");
            return;
        }
        verFile.close();
    }

    // Delete the temp directory
    QDir tempDir(m_qdmTempDir + "/" + info.ID);
    tempDir.removeRecursively();
    emit downloadFinished(true, "Download completed successfully.");
}

void Downloader::onReadReady()
{
    if (reply->bytesAvailable() > 0)
    {
        // Write the available data to the file
        QByteArray data = reply->readAll();
        info.currentSize += data.size();
        file.write(data);
    }
}

void Downloader::onDownloadFinished()
{
    QNetworkReply *senderReply = qobject_cast<QNetworkReply*>(sender());
    if (!senderReply) return;

    if (senderReply == reply) reply = nullptr;

    QNetworkReply::NetworkError err = senderReply->error();

    if (err == QNetworkReply::OperationCanceledError) {
        if (isPausing) {
            isPausing = false;
            file.flush();
            file.close();
        }
        senderReply->deleteLater();
        return;
    }

    if (err != QNetworkReply::NoError)
    {
        file.close();
        emit downloadFinished(false, "Error: " + senderReply->errorString());
    }
    else
    {
        if (senderReply->bytesAvailable() > 0)
            file.write(senderReply->readAll());

        file.close();
        emit downloadFinished(true, "Download completed successfully.");
    }

    senderReply->deleteLater();
}

void Downloader::downloadResume(downloadInformations Info)
{
    info = Info;
    isPausing = false;
    isCancelling = false;
    m_workers.clear();
    m_workerThreads.clear();
    m_url = QUrl(info.url);
    m_tempPaths.clear();
    m_qdmTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/Quantum";
    isResuming = true;

    info.tempPath = m_qdmTempDir + "/" + info.ID + "/" + info.fileName + ".qdm";
    m_file.setFileName(info.tempPath);
    m_file.resize(info.fileByteSize);

    chunkProgress = info.chunkProgress;
    if (chunkProgress.size() != info.fileParts.size())
        chunkProgress.resize(info.fileParts.size());

    info.currentSize = 0;
    for (qint64 p : chunkProgress) info.currentSize += p;

    QNetworkRequest req(m_url);
    QNetworkReply *headReply = manager->head(req);
    connect(headReply, &QNetworkReply::finished, this, [this, headReply]() mutable {
        headReply->deleteLater();
        qint64 freshSize = headReply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
        if (freshSize > 0 && freshSize != info.fileByteSize)
        {
            emit downloadFinished(false, "File changed on server, cannot resume.");
            return;
        }

        StartDataTimer();

        for (int i = 0; i < info.fileParts.size(); i++)
            if (!info.fileParts[i].done)
                info.fileParts[i].used = false;

        int workerCount = qMin(info.chunkCount, info.fileParts.size());
        for (int w = 0; w < workerCount; w++)
        {
            int partIndex = -1;
            for (int i = 0; i < info.fileParts.size(); i++)
            {
                if (!info.fileParts[i].done && !info.fileParts[i].used) { partIndex = i; break; }
            }
            if (partIndex == -1) break;

            info.fileParts[partIndex].used = true;
            qint64 resumeStart = info.fileParts[partIndex].start + chunkProgress[partIndex];

            QThread *workerThread = new QThread(this);
            DownloadWorker *worker = new DownloadWorker();
            worker->moveToThread(workerThread);
            m_workers.append(worker);
            m_workerThreads.append(workerThread);

            connect(workerThread, &QThread::started, worker, [=]() {
                worker->StartDownload(partIndex, resumeStart, info.fileParts[partIndex].end, true, info);
            });
            connect(worker, &DownloadWorker::Progress, this, &Downloader::onChunkProgress);
            connect(worker, &DownloadWorker::Finished, this, &Downloader::onChunkFinished);
            connect(worker, &DownloadWorker::ErrorOcc, this, &Downloader::onWorkerError);
            workerThread->start();
        }

        if (m_workers.isEmpty())
            handleDownloadFinish();
    });

    emit progressChanged(info.currentSize, info.fileByteSize);
}

void Downloader::downloadStop()
{
    isPausing = false;
    isCancelling = true;

    if (saveTimer)
        saveTimer->stop();

    // ask every worker to stop.
    for (DownloadWorker *worker : m_workers)
        QMetaObject::invokeMethod(worker, "Stop", Qt::QueuedConnection);

    // wait until every worker thread exits on its own.
    QElapsedTimer timer;
    timer.start();

    while (!m_workerThreads.isEmpty() && timer.elapsed() < 3000)
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
    }

    m_workers.clear();
    m_workerThreads.clear();

    if (m_file.isOpen())
        m_file.close();

    QDir tempDir(m_qdmTempDir + "/" + info.ID);
    tempDir.removeRecursively();

    m_tempPaths.clear();

    emit downloadFinished(false, "User canceled download");
}

void Downloader::downloadPause()
{
    if (m_workers.isEmpty() || isPausing) return;

    isPausing = true;
    if (saveTimer) saveTimer->stop();

    for (DownloadWorker *worker : m_workers)
        QMetaObject::invokeMethod(worker, "Stop", Qt::QueuedConnection);

    QElapsedTimer timer;
    timer.start();
    while (!m_workers.isEmpty() && timer.elapsed() < 5000)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    m_workers.clear();
    m_workerThreads.clear();

    WriteDownloadData();
}

void Downloader::onWorkerError(QString errStr)
{
    if (m_workers.isEmpty()) return; // already tearing down

    if (saveTimer) saveTimer->stop();

    for (DownloadWorker *worker : m_workers)
        QMetaObject::invokeMethod(worker, "Stop", Qt::QueuedConnection);

    QElapsedTimer timer;
    timer.start();
    while (!m_workers.isEmpty() && timer.elapsed() < 3000)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);

    m_workers.clear();
    m_workerThreads.clear();

    emit downloadFinished(false, "Download failed: " + errStr);
}

qint64 Downloader::fileSize()
{
    return info.fileByteSize;
}

int Downloader::chunkNumber()
{
    return info.chunkCount;
}

QString Downloader::downloadID()
{
    return info.ID;
}

QVector<qint64> Downloader::chunkProgressData()
{
    return chunkProgress;
}

qint64 Downloader::bytesDownloaded()
{
    return info.currentSize;
}

QList<Part> Downloader::FilePartsData()
{
    return info.fileParts;
}