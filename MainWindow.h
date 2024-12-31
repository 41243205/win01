#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "mainwindow_H.h"  // 引入數獨遊戲類
#include "mainwindow_L.h"  // 引入數獨遊戲類
#include "mainwindow_M.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartGameClicked();
    void onRulesClicked();

private:
    QLabel *titleLabel;        // 標題
    QPushButton *startButton;  // 開始遊戲按鈕
    QPushButton *rulesButton;  // 規則按鈕
    QComboBox *difficultyComboBox;  // 難度選擇下拉選單
    MainWindow_H *sudokuGame_H;  // 數獨遊戲視窗
    MainWindow_L *sudokuGame_L;  // 數獨遊戲視窗
    MainWindow_M *sudokuGame_M;
};


#endif // MAINWINDOW_H
