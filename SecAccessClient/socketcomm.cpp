#include "socketcomm.h"

#include <QFile>
#include <QtNetwork/QNetworkInterface>

SocketComm::SocketComm(QObject *parent) : QObject(parent)
{
    s = new QTcpSocket(this);
}

bool SocketComm::connectToServer(QString servName, int port)
{
    mServerName = servName;
    mPort = port;

    s->connectToHost(servName, port);
    if(!s->waitForConnected(3000))
        return false;

    /*********** init verification ***************/

    /*********** get connection info *************/
    localAddr = new QHostAddress(s->localAddress());

    /*********** wait for data transtion *********/
    connect(s,SIGNAL(readyRead()),this,SLOT(read()));

    return true;
}

bool SocketComm::close() {
    s->disconnectFromHost();
    if(s->state() == QAbstractSocket::ConnectedState)
        s->waitForDisconnected(3000);

    return true;
}

void SocketComm::read()
{
    QByteArray buf = s->readAll();

    qDebug() << buf;

    DumpDataToFile(buf);

    emit readyToPump(buf);
}

bool SocketComm::write(QString request)
{
    QByteArray buf = request.toUtf8();
    s->write(buf);

    return true;
}

bool SocketComm::writeBytes(QByteArray bytes)
{
    int sentBytesCount = 0;
    sentBytesCount = s->write(bytes);

    return sentBytesCount == bytes.size();
}

void SocketComm::DumpDataToFile(QByteArray buf)
{
    QString filePath = "socket_buffer.dmp";
    QFile dmpFile(filePath);
    if(!dmpFile.open( QIODevice::Append | QIODevice::Text )) {
        qDebug() << QString("fail open log file %1").arg(filePath);
    } else {
        QTextStream ts(&dmpFile);
        ts << buf;
        dmpFile.close();
    }
}

QString SocketComm::getServerName()
{
    return mServerName;
}

int SocketComm::getPort()
{
    return mPort;
}

QString SocketComm::getStatus()
{
    if(s->state() == QAbstractSocket::ConnectedState)
    {
        return "已连接";
    } else {
        return "已断开";
    }
}

bool SocketComm::isOpen()
{
    if(s->isOpen())
        return true;
    else
        return false;
}

bool SocketComm::isConnected()
{
    if(s->state() == QAbstractSocket::ConnectedState)
        return true;
    else
        return false;
}

QString SocketComm::getHostIpAddress()
{
    QString strIpAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // 获取第一个本主机的IPv4地址
    int nListSize = ipAddressesList.size();
    for (int i = 0; i < nListSize; ++i)
    {
           if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
               ipAddressesList.at(i).toIPv4Address()) {
               strIpAddress = ipAddressesList.at(i).toString();
               break;
           }
     }
     // 如果没有找到，则以本地IP地址为IP
     if (strIpAddress.isEmpty())
        strIpAddress = QHostAddress(QHostAddress::LocalHost).toString();
     return strIpAddress;
}

// 枚举本机的网络连接并获取其属性
void SocketComm::getInterfaces()
{
    int i=0, j=0;
    QList<QNetworkInterface> networkInterface = QNetworkInterface::allInterfaces();
    for (QList<QNetworkInterface>::const_iterator it = networkInterface.constBegin(); it != networkInterface.constEnd(); ++it)
    {
        qDebug() << "[" << i << "] : " << (*it).name();
        qDebug() << "  " << (*it).humanReadableName();
        qDebug() << "  " << (*it).hardwareAddress();

        //获取连接地址列表
        QList<QNetworkAddressEntry> addressEntriesList = (*it).addressEntries();
        for (QList<QNetworkAddressEntry>::const_iterator jIt = addressEntriesList.constBegin(); jIt != addressEntriesList.constEnd(); ++jIt)
        {
            qDebug() << "\t(" << j << ") :";
            //输出 ip
            qDebug() << "\t\tIP : " <<(*jIt).ip().toString();
            //输出 netmask
            qDebug() << "\t\tnetmask(子网掩码) : " << (*jIt).netmask().toString();
            qDebug() << "\t\tBroadcast(广播地址) : "<< (*jIt).broadcast().toString();
            qDebug() << "";
            j ++;
        }
        i ++;
    }
}

QString SocketComm::getLocalMACfromConnection()
{
    if(localAddr->isNull())
    {
        qDebug() << "no connection.";
        return nullptr;
    }

    int i=0, j=0;
    QList<QNetworkInterface> networkInterface = QNetworkInterface::allInterfaces();
    for (QList<QNetworkInterface>::const_iterator it = networkInterface.constBegin(); it != networkInterface.constEnd(); ++it)
    {
//        qDebug() << "[" << i << "] : " << (*it).name();
//        qDebug() << "  " << (*it).humanReadableName();
//        qDebug() << "  " << (*it).hardwareAddress();

        //获取连接地址列表
        QList<QNetworkAddressEntry> addressEntriesList = (*it).addressEntries();
        for (QList<QNetworkAddressEntry>::const_iterator jIt = addressEntriesList.constBegin(); jIt != addressEntriesList.constEnd(); ++jIt)
        {
//            qDebug() << "\t(" << j << ") :";
//            //输出 ip
//            qDebug() << "\t\tIP : " <<(*jIt).ip().toString();
//            //输出 netmask
//            qDebug() << "\t\tnetmask(子网掩码) : " << (*jIt).netmask().toString();
//            qDebug() << "\t\tBroadcast(广播地址) : "<< (*jIt).broadcast().toString();
//            qDebug() << "";
            QString jIP = (*jIt).ip().toString();
            if((*jIt).ip() == *localAddr)
            {
                return (*it).hardwareAddress();
            }
            j ++;
        }
        i ++;
    }

    return nullptr;
}

QString SocketComm::getLocalIPfromConnection()
{
    if(localAddr->isNull())
    {
        return nullptr;
    }
    return localAddr->toString();
}
