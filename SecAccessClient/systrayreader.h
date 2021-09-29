#ifndef SYSTRAYREADER_H
#define SYSTRAYREADER_H

#include <QObject>
#include <QList>

#include "windowinfo.h"

#include <iostream>
#include <windows.h>
#include <string>
#include <commctrl.h>
//#include <atlstr.h>

class SysTrayReader : public QObject
{
    Q_OBJECT
public:
    explicit SysTrayReader(QObject *parent = nullptr);

    BOOL Is64bitSystem();
    HWND FindTrayWnd();
    HWND FindNotifyIconOverflowWindow();
    BOOL EnumNotifyWindow(HWND hWnd, QList<WindowInfo *> *winInfoList);

signals:

};

#endif // SYSTRAYREADER_H
