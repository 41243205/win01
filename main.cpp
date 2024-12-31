#include "MainWindow.h"
#include "mainwindow_L.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 初始化Qt應用程式
    QApplication app(argc, argv);

    // 創建並顯示MainWindow
qDebug() << "開始啟動主視窗";  // 這行是調試輸出
    MainWindow win;
    win.show();

    /*MainWindow_L win_L;
    win_L.show();*/

    // 啟動Qt事件循環
    return app.exec();
}
