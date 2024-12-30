#include "MainWindow_L.h"
#include "mainwindow_M.h"
#include "MainWindow_H.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 初始化Qt應用程式
    QApplication app(argc, argv);

    // 創建並顯示MainWindow
    MainWindow_M Mwin;
    Mwin.show();

    MainWindow_H Hwin;
    Hwin.show();

    MainWindow_L Lwin;
    Lwin.show();

    // 啟動Qt事件循環
    return app.exec();
}
