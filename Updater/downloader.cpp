#include "downloader.h"
#include "processexecution.h"

#include <QDebug>
#include <QFile>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    log = new Log;
    log->loadConfiguration("logger.ini");
    fileName = nullptr;
    url = nullptr;
    destPath = nullptr;
}

QString Downloader::getFileName() const
{
    return fileName;
}

void Downloader::setFileName(const QString &value)
{
    fileName = value;
}

QString Downloader::getDestPath() const
{
    return destPath;
}

void Downloader::setDestPath(const QString &value)
{
    destPath = value;
}

QString Downloader::getUrl() const
{
    return url;
}

void Downloader::setUrl(const QString &value)
{
    url = value;
}

QString Downloader::execCmd(QString cmd, QString args)
{
    ProcessExecution pe;
    return pe.execCmd(cmd, args);
}

QString Downloader::execCmd(QString cmd, QStringList args)
{
    ProcessExecution pe;
    return pe.execCmd(cmd, args);
}

void Downloader::start()
{
    qDebug() << "remote url:" << url;
    qDebug() << "destination path:" << destPath;
    log->write(Log::TYPE_DEBUG, "Downloader::start", "remote url:"+url+"; destination path:"+destPath);
    download(url, destPath);
}

void Downloader::download(QString url, QString destPath)
{
    if(url.startsWith("ftp://"))
    {
        ftpget(url, destPath);
    } else if(url.startsWith("http://"))
    {
        httpget(url, destPath);
    }
}

void Downloader::httpget(QString url, QString destPath)
{
    if(!url.startsWith("http://"))
        return;

    fetch(url, destPath);
}

void Downloader::ftpget(QString url, QString destPath)
{
    if(!url.startsWith("ftp://"))
        return;

    qDebug() << powershellDownload(url, destPath);

//    QString remotePath = url.right(url.length() - 6);
//    int splitPos = remotePath.indexOf("/");
//    QString server = remotePath.left(splitPos);
//    QString filePath = remotePath.right(remotePath.length() - splitPos -1);
//    qDebug() << server << filePath;
//    QString bat = tr("ftpget.bat %1 %2 %3").arg(server).arg(filePath).arg(destPath);
//    qDebug() << bat;
//    QString cmdResult = execCmd("cmd", bat);
//    qDebug() << cmdResult;
}

void Downloader::fetch(QString url, QString destPath)
{
    manager = new QNetworkAccessManager(this);
    connect(manager,SIGNAL(finished(QNetworkReply*)),this,SLOT(fetchCompleted(QNetworkReply*)));
    manager->get(QNetworkRequest(QUrl(url)));
}

void Downloader::fetchCompleted(QNetworkReply *reply)
{
    if(reply->error()!=QNetworkReply::NoError){
        //处理中的错误信息
        qDebug()<<"reply error:"<<reply->errorString();
    }else{
        //请求方式
        qDebug()<<"operation:"<<reply->operation();
        //状态码
        qDebug()<<"status code:"<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug()<<"url:"<<reply->url();

        const QByteArray replyData=reply->readAll();
        QFile file(destPath);
        if(file.exists())
        {
            if(!file.remove())
            {
                log->write(Log::TYPE_DEBUG, "Downloader::fetchCompleted", "failed to remove exist file:"+destPath);
                return;
            }
        }
        if(file.open(QIODevice::NewOnly))
        {
            file.write(replyData);
            file.close();
        }

        emit downloadComplete(this);
    }
}

QString Downloader::powershellDownload(QString url, QString destPath)
{
    QString urlMod = url.right(url.size()-4);
//    QString powershell = "powershell (new-object Net.WebClient).DownloadFile('ht'+'tp"+urlMod+"','"+destPath+"')";
    QString powershell = "echo (new-object Net.WebClient).DownloadFile('"+url+"','"+destPath+"')|powershell -";
    qDebug() << powershell;

//    QStringList args;
//    args << powershell;
//    QString cmd = "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe";
    QString cmdResult = execCmd("cmd", powershell);
    qDebug() << cmdResult;
    return cmdResult;
}

QString Downloader::bitsadminDownload(QString url, QString destPath)
{
    QStringList args;
    args << "/c" << "bitsadmin" << "/transfer" << "myDownLoadJob" << "/download" << "/priority" << "normal" << url << destPath;
    QString cmdResult = execCmd("cmd", args);
    qDebug() << cmdResult;
    return cmdResult;
}
