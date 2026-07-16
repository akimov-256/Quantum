#include "downloadinfo.h"
#include "ui_downloadinfo.h"

DownloadInfo::DownloadInfo(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownloadInfo)
{
    ui->setupUi(this);
    setFixedHeight(70);
}

void DownloadInfo::Downloading(QString Data)
{

}

void DownloadInfo::Paused(QString Data)
{

}

void DownloadInfo::Finished(QString Data)
{

}

DownloadInfo::~DownloadInfo()
{
    delete ui;
}

void DownloadInfo::on_pauseResume_clicked()
{
    if (ui->status->text() == "Downloading...")
        emit pauseRequested();
    else if (ui->status->text() == "Paused.")
        emit resumeRequested(m_info);
}

