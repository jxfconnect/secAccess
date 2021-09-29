#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include "log.h"

#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = nullptr);

    QString url;
    QString destPath;
    QString fileName;
    QNetworkAccessManager *manager;

    QString execCmd(QString cmd, QString args);
    QString execCmd(QString cmd, QStringList args);
    void start();
    void ftpget(QString url, QString destPath);
    void httpget(QString url, QString destPath);
    QString powershellDownload(QString url, QString destPath);
    QString bitsadminDownload(QString url, QString destPath);
    void fetch(QString url, QString destPath);

    QString getUrl() const;
    void setUrl(const QString &value);

    QString getDestPath() const;
    void setDestPath(const QString &value);

    QString getFileName() const;
    void setFileName(const QString &value);

signals:
    void downloadComplete(Downloader*);

private:
    Log *log;

    void download(QString url, QString destPath);

private slots:
    void fetchCompleted(QNetworkReply *reply);
};

#endif // DOWNLOADER_H
