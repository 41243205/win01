#ifndef MAINWINDOW_M_H
#define MAINWINDOW_M_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QResizeEvent> // 引入以支持重寫 resizeEvent

// MainWindow.h
class MainWindow_M : public QMainWindow
{
    Q_OBJECT

public:
    void generateSudokuSolution_M();          // 生成完整的數獨解答
    void digHoles_M();                        // 挖空生成數獨題目
    void setupBorders_M();                    // 設置九宮格邊線
    explicit MainWindow_M(QWidget *parent = nullptr);
    ~MainWindow_M();

protected:
    void resizeEvent_M(QResizeEvent *event) ; // 新增，用於自適應窗口大小

private slots:
    void onCellChanged_M(int row, int column); // 單元格改變時的槽函數
    void onCellClicked_M(int row, int column); // 單元格被點擊時的槽函數
    void checkWinCondition_M();               // 檢查是否完成遊戲

private:
    bool solveSudoku_M(int row, int col);     // 使用回溯法解決數獨
    bool isSafe_M(int row, int col, int num); // 檢查在指定位置填入數字是否安全
    void checkGameOver_M();                   // 檢查遊戲是否結束

    QLabel *livesLabel_M;                     // 顯示生命值的標籤
    int puzzle_M[9][9] = {0};                 // 數獨題目
    int solution_M[9][9] = {0};               // 數獨答案
    QSet<QPair<int, int>> usedNumbers_M;      // 記錄已使用過的行與列組合
    int lives_M;                              // 剩餘生命值
    QTableWidget *tableWidget_M;              // 數獨表格部件
    int selectedRow_M;                        // 記錄當前選中的行
    int selectedCol_M;                        // 記錄當前選中的列
};

#endif // MAINWINDOW_H
