#ifndef UPDATEWIN_H
#define UPDATEWIN_H

#include "log.h"
#include "downloader.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class UpdateWin; }
QT_END_NAMESPACE

class UpdateWin : public QWidget
{
    Q_OBJECT

public:
    UpdateWin(QWidget *parent = nullptr);
    ~UpdateWin();

    Log *log;
    QList<Downloader *> *downloaders;
    QString remoteLocation;
    QString targetLocation;
    QString updaterNameShort;
    QString updaterNameLong;
    QString tmpUpdaterNameShort;
    QString tmpUpdaterNameLong;
    int dlCount;
    int fileNum;
    QString appNameShort;
    QString mainAppLong;

    QJsonObject loadUpdateTask();
    bool isFilesDownloaded();
    void runUpdateTask();
    void getNewUpdater();
    bool downloadFiles(QJsonObject cfg);
    bool copyAndRunUpdater();
    bool moveUpdater();
    bool copyNewVersion();
    bool removeTmpFiles();
    void exitUpdater();
    void stopMainApp();
    bool removeTmpUpdater();
    void startAppInCurrDir(QString appName);

private:
    Ui::UpdateWin *ui;

private slots:
    void OnDownloadComplete(Downloader *dlr);
    void on_pushButton_Start_clicked();
};
#endif // UPDATEWIN_H
