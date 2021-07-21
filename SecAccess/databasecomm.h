#ifndef DATABASECOMM_H
#define DATABASECOMM_H

#include "log.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QTableWidget>

class DatabaseComm : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseComm(QObject *parent = nullptr);

    bool loadConfiguration(QString configPath);
    bool open();
    bool close();
    bool push(QMap<QString, QString> data);
    bool read(QString sql, int pageSize, QTableWidget *table);
    QList<QMap<QString, QString>> read(QString sql);
    bool isOpen();
    QString getLastError();
    QString getServerName();
    int getServerPort();
    QString getUserName();
    QString getDBName();
    void insert(QString tabName, QMap<QString, QString> data);
    void update(QString sql);

private:
    QString databaseType;
    QString serverName;
    int serverPort;
    QString userName;
    QString password;
    QString dbName;

    QSqlDatabase db;
    QSqlQuery * query;
    QString lastError;

    bool isCfgFileExist;

    Log *log;

signals:

private slots:
    void push(QString tabName, QMap<QString, QString> data);

};

#endif // DATABASECOMM_H
