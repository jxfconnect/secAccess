#ifndef LOG_H
#define LOG_H

#include <QObject>

class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = nullptr);

    void loadConfiguration(QString configPath);
    void write(int type, QString moduleName, QString message);

    static const int TYPE_INFO = 1;
    static const int TYPE_DEBUG = 2;
    static const int TYPE_WARNING = 3;
    static const int TYPE_ERROR = 4;


private:
    QString configPath;
    QString logPath;
    bool isConfigured;

signals:

};

#endif // LOG_H
