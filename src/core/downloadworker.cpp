#include "downloadworker.h"

DownloadWorker::DownloadWorker()
{}

void DownloadWorker::StartDownload(int chunkIndex, qint64 start, qint64 end, bool isResuming, downloadInformations Info)
{
    // set variables
    m_chunkIndex = chunkIndex;
    m_start = start;
    m_end = end;
    m_isResuming = isResuming;
    m_info = Info;

    m_Stopped = false;
    qDebug() << "StartDownload called for chunk" << m_chunkIndex;
    m_file.setFileName(m_info.tempPath);

    qDebug() << "fileName" + m_info.fileName;

    m_downloadOffset = m_start;

    if (!m_file.open(QIODevice::ReadWrite))
    {
        emit ErrorOcc(m_file.errorString());
        return;
    }

    if (!manager)
        manager = new QNetworkAccessManager(this);

    QNetworkRequest request(m_info.url);
    QByteArray rangeHeader = "bytes=" + QByteArray::number(m_start) + "-" + QByteArray::number(m_end);
    request.setRawHeader("Range", rangeHeader);

    reply = manager->get(request);
    qDebug() << "GET issued for chunk" << m_chunkIndex << "Range:" << rangeHeader;

    connect(reply, &QNetworkReply::readyRead, this, &DownloadWorker::OnReadReady);
    connect(reply, &QNetworkReply::finished, this, &DownloadWorker::OnReplyFinished);
}

void DownloadWorker::Stop()
{
    if (m_Stopped)
        return;

    m_Stopped = true;

    if (reply)
        reply->abort();

    if (manager)
        manager->disconnect(this);
}

void DownloadWorker::OnReadReady()
{
    if (m_Stopped) return;
    QByteArray data = reply->readAll();
    m_writeBuffer.append(data);
    emit Progress(m_chunkIndex, data.size());

    if (m_writeBuffer.size() >= BUFFER_SIZE)
    {
        m_file.seek(m_downloadOffset);
        m_file.write(m_writeBuffer);
        m_downloadOffset += m_writeBuffer.size();
        m_writeBuffer.clear();
    }
}

void DownloadWorker::OnReplyFinished()
{
    if (!m_writeBuffer.isEmpty())
    {
        m_file.seek(m_downloadOffset);
        m_file.write(m_writeBuffer);
        m_writeBuffer.clear();
    }
    m_file.close();

    auto error = reply->error();
    QString errorStr = reply->errorString();
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();
    reply = nullptr;

    if (m_Stopped)
    {
        emit Finished(this, true);
        return;
    }

    qDebug() << "Chunk" << m_chunkIndex << "finished, error:" << error << "status:" << status;

    if (error != QNetworkReply::NoError)
    {
        if (retryCount < retryMax)
        {
            retryCount++;
            m_isResuming = true;
            StartDownload(m_chunkIndex, m_start, m_end, m_isResuming, m_info);
            return;
        }
        emit ErrorOcc(errorStr);
    }
    else if (status != 206)
    {
        emit ErrorOcc("Server ignored range request");
    }
    else
    {
        retryCount = 0;
        emit Finished(this, false);
    }
}