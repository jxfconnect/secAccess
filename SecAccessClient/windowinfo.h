#ifndef WINDOWINFO_H
#define WINDOWINFO_H

#include <QObject>

class WindowInfo : public QObject
{
    Q_OBJECT
public:
    explicit WindowInfo(QObject *parent = nullptr);

    QString getMainWndID() const;
    void setMainWndID(const QString &value);

    QString getProcPath() const;
    void setProcPath(const QString &value);

    QString getWinTitle() const;
    void setWinTitle(const QString &value);

signals:


private:
    QString mainWndID;
    QString procPath;
    QString winTitle;
};

#endif // WINDOWINFO_H
