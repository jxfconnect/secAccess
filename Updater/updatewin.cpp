#include "updatewin.h"
#include "ui_updatewin.h"
#include "processexecution.h"

#include <QDesktopServices>
#include <QFile>
#include <QDir>

UpdateWin::UpdateWin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UpdateWin)
{
    ui->setupUi(this);

    downloaders = new QList<Downloader*>;
    remoteLocation = nullptr;
    targetLocation = nullptr;
    updaterNameShort = nullptr;
    appNameShort = nullptr;
    mainAppLong = nullptr;
    dlCount = 0;
    log = new Log;
    log->loadConfiguration("logger.ini");


    //    QString appName = QCoreApplication::applicationName();
    //    QString appPath = QCoreApplication::applicationFilePath();
    QStringList args = QCoreApplication::instance()->arguments();
    QString appUrl = args.takeFirst();
    QString appNameLong = appUrl.right(appUrl.size() - appUrl.lastIndexOf("\\") - 1);
    appNameShort = appNameLong.left(appNameLong.size()-4).toLower();
    log->write(Log::TYPE_DEBUG, "UpdateWin::UpdateWin", "app name short:"+appNameShort);
    //get new update program
//    getNewUpdater();

    runUpdateTask();

//    ProcessExecution pe;
//    pe.execCmd("D:\\WeChat\\WeChat.exe", "");

//    QDesktopServices::openUrl(QUrl("file:///D:/WeChat/WeChat.exe"));
}

UpdateWin::~UpdateWin()
{
    delete ui;
}


void UpdateWin::runUpdateTask()
{
    QJsonObject cfg = loadUpdateTask();
    if(remoteLocation == nullptr)
        return;

    if(appNameShort == updaterNameShort)
    {
        if(!isFilesDownloaded())
        {
            stopMainApp();
            if(!downloadFiles(cfg))
                return;
        } else {
            startAppInCurrDir(mainAppLong);
            removeTmpUpdater();
            exitUpdater();
        }
    } else if(appNameShort == tmpUpdaterNameShort)
    {
        moveUpdater();
        startAppInCurrDir(updaterNameLong);
        exitUpdater();
    }
}

QJsonObject UpdateWin::loadUpdateTask()
{
    log->write(Log::TYPE_DEBUG, "UpdateWin::loadUpdateTask", "load json config file.");
    QJsonObject cfg;
    QString cfgFilePath = "update.json";
    if (!QFile::exists(cfgFilePath)) {
        log->write(Log::TYPE_DEBUG, "UpdateWin::loadUpdateTask", "cfg file update.json does not exist.");
        return cfg;
    }
    QFile file(cfgFilePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        log->write(Log::TYPE_DEBUG, "UpdateWin::loadUpdateTask", "couldn't open cfg file update.json.");
        return cfg;
    }
    QByteArray data=file.readAll();
    file.close();
    QJsonParseError err;
    cfg = QJsonDocument::fromJson(data, &err).object();
    log->write(Log::TYPE_DEBUG, "UpdateWin::loadUpdateTask", "Json error:"+err.errorString());
    qDebug() << "JSON error: " << err.errorString();
    remoteLocation = cfg.value("remoteLocation").toString();
    targetLocation = ".\\"+cfg.value("targetLocation").toString()+"\\";
    updaterNameShort = cfg.value("updaterName").toString();
    updaterNameLong = updaterNameShort+".exe";
    tmpUpdaterNameShort = updaterNameShort+".new";
    tmpUpdaterNameLong = tmpUpdaterNameShort+".exe";
    fileNum = cfg.value("fileNum").toInt();
    mainAppLong = cfg.value("mainApp").toString();

    return cfg;
}

bool UpdateWin::downloadFiles(QJsonObject cfg)
{
    log->write(Log::TYPE_DEBUG, "UpdateWin::downloadFiles", "start files download.");
    if(remoteLocation == nullptr)
        return false;
    QString folderName = targetLocation.right(targetLocation.size() - 2).left(targetLocation.size()-1);
    QString tmpPath = QCoreApplication::applicationDirPath()+"\\"+folderName;
    QDir tmpdir(tmpPath);
    if(!tmpdir.exists())
    {
        if(!tmpdir.mkdir(tmpPath))
        {
            log->write(Log::TYPE_DEBUG, "UpdateWin::downloadFiles", "failed to make dir:"+tmpPath);
            return false;
        }
    }
    QJsonObject files = cfg.take("files").toObject();
    QJsonObject::iterator it;
    it = files.begin();
    while(it != files.end())
    {
        QJsonObject file = it->toObject();
        QString filename = file.value("filename").toString();
        QString targetName = file.value("targetName").toString();
        QString fileUrl = remoteLocation+"/"+filename;
        QString destUrl = targetLocation+targetName;
        qDebug() << "File Name: " << filename;
        qDebug() << "fileUrl: " << fileUrl;
        qDebug() << "destUrl: " << destUrl;
        Downloader *dl = new Downloader;
        downloaders->append(dl);
        connect(dl, SIGNAL(downloadComplete(Downloader*)), this, SLOT(OnDownloadComplete(Downloader*)));
        dl->setFileName(targetName);
        dl->setUrl(fileUrl);
        dl->setDestPath(destUrl);
        dl->start();
        it++;
    }
    return true;
}

//not used
void UpdateWin::getNewUpdater()
{
}

bool UpdateWin::copyAndRunUpdater()
{
    log->write(Log::TYPE_DEBUG, "UpdateWin::copyAndRunUpdater", "copy new updater to main app dir and runs it.");
    if(appNameShort == updaterNameShort)
    {
        //copy the executable
        QString fileName = tmpUpdaterNameLong;
        QFile dstFile(".\\"+fileName);
        if(dstFile.exists())
        {
            if(!dstFile.remove())
            {
                log->write(Log::TYPE_DEBUG, "UpdateWin::copyAndRunUpdater", "failed to replace Updater.exe. tmp file exists.");
                return false;
            }
        }
        if(!QFile::rename(targetLocation+fileName, ".\\"+fileName))
        {
            return false;
        }
        //run new updater
        startAppInCurrDir(tmpUpdaterNameLong);
    }

    return true;
}

bool UpdateWin::moveUpdater()
{
    log->write(Log::TYPE_DEBUG, "UpdateWin::moveUpdater", "replace updater file.");
    if(appNameShort == tmpUpdaterNameShort)
    {
        QFile destFile(".\\"+updaterNameLong);
        if(destFile.exists())
        {
            log->write(Log::TYPE_DEBUG, "UpdateWin::moveUpdater", "delete file.");
            destFile.remove();
        }
        if(!QFile::copy(".\\"+tmpUpdaterNameLong, ".\\"+updaterNameLong))
        {
            log->write(Log::TYPE_DEBUG, "UpdateWin::moveUpdater", "failed to copy file.");
            return false;
        }
    }
    return true;
}

bool UpdateWin::removeTmpUpdater()
{
    log->write(Log::TYPE_DEBUG, "UpdateWin::removeTmpUpdater", "remove tmp updater file(ie. updater.new.exe).");
    QString tmpUpdaterName = ".\\"+tmpUpdaterNameLong;
    QFile tmpUpdaterFile(tmpUpdaterName);
    if(tmpUpdaterFile.exists())
    {
        if(!tmpUpdaterFile.remove())
        {
            log->write(Log::TYPE_DEBUG, "UpdateWin::removeTmpUpdater", "failed to remove tmp updater file:"+tmpUpdaterName);
            return false;
        }
    }
    return true;
}

bool UpdateWin::isFilesDownloaded()
{
    QString tmpUpdaterName = ".\\"+tmpUpdaterNameLong;
    log->write(Log::TYPE_DEBUG, "UpdateWin::isFilesDownloaded", "tmp updater file:"+tmpUpdaterName);
    QFile tmpUpdaterFile(tmpUpdaterName);
    if(tmpUpdaterFile.exists())
    {
        return true;
    } else {
        return false;
    }
}

bool UpdateWin::copyNewVersion()
{
    log->write(Log::TYPE_DEBUG, "UpdateWin::copyNewVersion", "move tmp files to main app directory");
    for(int i=0; i<downloaders->size(); i++)
    {
        Downloader *dl = downloaders->at(i);
        QString fileName = dl->getFileName();
        if(fileName == "update.json" || fileName == tmpUpdaterNameLong)
            continue;
        QFile dstFile(".\\"+fileName);
        if(dstFile.exists())
        {
            if(!dstFile.remove())
            {
                log->write(Log::TYPE_DEBUG, "UpdateWin::copyNewVersion", "failed to delete file:"+dl->getFileName());
                return false;
            }
        }
        QFile srcFile(dl->destPath);
        if(!srcFile.rename(".\\"+fileName))
        {
            log->write(Log::TYPE_DEBUG, "UpdateWin::copyNewVersion", "failed to move file:"+dl->getFileName());
            return false;
        }
    }
//    QString fileLocation = QCoreApplication::applicationDirPath();
//    QString fileUrl = "file:///"+fileLocation+"/"+mainAppLong;
//    qDebug() << "New updater run as:" << fileUrl;
//    QDesktopServices::openUrl(QUrl(fileUrl));
    return true;
}

bool UpdateWin::removeTmpFiles()
{
    log->write(Log::TYPE_DEBUG, "UpdateWin::removeTmpFiles", "remove tmp files.");
    QDir tmpDir(targetLocation);
    if(!tmpDir.removeRecursively())
    {
        log->write(Log::TYPE_DEBUG, "UpdateWin::removeTmpFiles", "failed to remove tmp folder:"+targetLocation);
        return false;
    }
    return true;
}

void UpdateWin::OnDownloadComplete(Downloader *dlr)
{
//    if(dlr->url.contains(updaterNameShort))
//        copyAndRunUpdater();
    dlCount++;
    if(dlCount == fileNum)
    {
        copyNewVersion();
        copyAndRunUpdater();
        removeTmpFiles();
        exitUpdater();
    }

//    int pos =  downloaders->indexOf(dlr);
//    qDebug() << "downloader index:" << pos;
//    qDebug() << "url of this downloader:" << downloaders->at(pos)->url;
//    downloaders->removeOne(dlr);
//    dlr->deleteLater();
}

void UpdateWin::exitUpdater()
{
    exit(0);
}

void UpdateWin::stopMainApp()
{
    ProcessExecution pe;
    QString cmd = "taskkill";
    QStringList args;
    args << "/im" << mainAppLong << "/f";
    QString cmdResult = pe.execCmd(cmd, args);
    log->write(Log::TYPE_DEBUG, "UpdateWin::stopMainApp", "command result:"+cmdResult);
}

void UpdateWin::startAppInCurrDir(QString appName)
{
    QString fileLocation = QCoreApplication::applicationDirPath();
    QString fileUrl = "file:///"+fileLocation+"/"+appName;
    log->write(Log::TYPE_DEBUG, "UpdateWin::startAppInCurrDir", "App run as:"+fileUrl);
    qDebug() << "New updater run as:" << fileUrl;
    QDesktopServices::openUrl(QUrl(fileUrl));
}

void UpdateWin::on_pushButton_Start_clicked()
{
//    QString fileLocation = QCoreApplication::applicationDirPath();
//    QString fileUrl = "file:///"+fileLocation+"/updater.new.exe";
//    qDebug() << "test:" << fileUrl;
//    QDesktopServices::openUrl(QUrl(fileUrl));

    QDir tmpDir("tmp");
    if(tmpDir.exists())
        qDebug() << "dir exists";
}
