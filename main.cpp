#include "MainWindow_L.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    // 初始化Qt應用程式
    QApplication app(argc, argv);

    // 創建並顯示MainWindow
    MainWindow mainWindow;
    mainWindow.show();

    // 啟動Qt事件循環
    return app.exec();
}
