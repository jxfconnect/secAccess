#include "databasecomm.h"
#include <QFile>
#include <QtDebug>
#include <QSettings>
#include <QSqlError>

#include <QTableWidgetItem>

DatabaseComm::DatabaseComm(QObject *parent) : QObject(parent)
{
    log = new Log;
    log->loadConfiguration("logger.ini");
}

bool DatabaseComm::loadConfiguration(QString configPath)
{
    if (!QFile::exists(configPath)) {
        qDebug() << QString("db configuration file %1 does not exist.").arg(configPath);
        isCfgFileExist = false;
        return false;
    }

    QSettings setting(configPath,QSettings::IniFormat);
    databaseType = setting.value("database").toString();
    if(databaseType == "mysql") {
        serverName = setting.value("mysql.server.addr").toString();
        serverPort = setting.value("mysql.server.port").toInt();
        userName = setting.value("mysql.user").toString();
        password = setting.value("mysql.password").toString();
        dbName = setting.value("mysql.db_name").toString();
    } else {
        serverName = "";
        serverPort = 0;
        userName = "";
        password = "";
        dbName = "";
    }

    return true;
}

bool DatabaseComm::open()
{
    QString dbType = "";
    log->write(Log::TYPE_DEBUG, "DB", databaseType);
    if(databaseType == "mysql") {
        dbType = "QMYSQL";

        db = QSqlDatabase::addDatabase(dbType);
        db.setHostName(serverName);
        db.setPort(serverPort);
        db.setDatabaseName(dbName);
        db.setUserName(userName);
        db.setPassword(password);
//        log->write(Log::TYPE_DEBUG, "DB", QString("server: %1; port:%2; dbName:%3; user:%4; password:%5").arg(serverName).arg(serverPort).arg(dbName).arg(userName).arg(password));
        if(!db.open()) {
            lastError = QString("Connect to database failed: %1").arg(db.lastError().text());
//            log->write(Log::TYPE_DEBUG, "DB", lastError);
            return false;
        }
        else
        {
            query = new QSqlQuery(db);
        }
    } else {
        return false;
    }
    return true;
}

bool DatabaseComm::close() {
    db.close();
    return true;
}

bool DatabaseComm::read(QString sql, int pageSize, QTableWidget *table)
{
    bool success = query->exec(sql);
    if(!success)
    {
        lastError = "Fail to read from database.";
        return false;
    }

    int colCount = table->columnCount();
    int colCountDB = query->record().count();
    table->setRowCount(0);
    table->clearContents();
    int row = 0;
    while(query->next() && row < pageSize)
    {
        table->insertRow(row);
        for(int i=0; i<colCountDB && i<colCount; i++)
        {
            table->setItem(row, i,new QTableWidgetItem(query->value(i).toString()));
        }
        row++;
    }
    return true;
}

QList<QMap<QString, QString>>  DatabaseComm::read(QString sql)
{
    QList<QMap<QString, QString>> result;
    bool success = query->exec(sql);
    if(!success)
    {
        lastError = "Fail to read from database.";
        return result;
    }

    int colCountDB = query->record().count();
    while(query->next())
    {
        QMap<QString, QString> record;
        for(int i=0; i<colCountDB; i++)
        {
            record.insert(query->record().fieldName(i), query->value(i).toString());
        }
        result.append(record);
    }
    return result;
}

void DatabaseComm::push(QString tabName, QMap<QString, QString> data)
{
    insert(tabName, data);
}

void DatabaseComm::insert(QString tabName, QMap<QString, QString> data)
{
    if(data.isEmpty())
        return;

    QString sql1 = tr("INSERT INTO %1(").arg(tabName);
    QString sql2 = ") VALUES(";
    QMap<QString, QString>::iterator it = data.begin();
    while(it != data.end()) {
        sql1 += it.key()+", ";
        sql2 += "'"+it.value()+"', ";
        it++;
    }
    sql1 = sql1.left(sql1.size()-2);
    sql2 = sql2.left((sql2.size()-2));
    QString sql = sql1 + sql2 + ")";
    qDebug() << sql;
    query->exec(sql);

}

void DatabaseComm::update(QString sql)
{
    qDebug() << sql;
    query->exec(sql);
}

bool DatabaseComm::push(QMap<QString, QString> data)
{
    data.isEmpty();
    return false;
}

bool DatabaseComm::isOpen()
{
    return db.isOpen();
}

QString DatabaseComm::getLastError()
{
    return lastError;
}

QString DatabaseComm::getServerName()
{
    return serverName;
}

int DatabaseComm::getServerPort()
{
    return serverPort;
}

QString DatabaseComm::getUserName()
{
    return userName;
}

QString DatabaseComm::getDBName()
{
    return dbName;
}
