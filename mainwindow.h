#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QResizeEvent> // 引入以支持重寫 resizeEvent
#include <QSet>
#include <QPair>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override; // 新增，用於自適應窗口大小



private slots:
    void onCellChanged(int row, int col); // 單元格改變時的槽函數
    void onCellClicked(int row, int column); // 單元格被點擊時的槽函數
    void checkWinCondition();               // 檢查是否完成遊戲

private:
    void generateSudokuSolution();          // 生成完整的數獨解答
    void digHoles();                        // 挖空生成數獨題目
    void setupBorders();                    // 設置九宮格邊線
    bool solveSudoku(int row, int col);     // 使用回溯法解決數獨
    bool isSafe(int row, int col, int num); // 檢查在指定位置填入數字是否安全
    void checkGameOver();                   // 檢查遊戲是否結束

    QLabel *livesLabel;                     // 顯示生命值的標籤
    int puzzle[16][16] = {0};               // 數獨題目
    int solution[16][16] = {0};             // 數獨答案
    QSet<QPair<int, int>> usedNumbers;      // 記錄已使用過的行與列組合
    int lives;                              // 剩餘生命值
    QTableWidget *tableWidget;              // 數獨表格部件
    int selectedRow;                        // 記錄當前選中的行
    int selectedCol;                        // 記錄當前選中的列
};

#endif // MAINWINDOW_H
