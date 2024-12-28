#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>

// MainWindow.h
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCellChanged(int row, int column);  // 原有槽函數
    void checkWinCondition();  // 新增的函數聲明

private:
    void generateSudokuSolution();
    void digHoles();
    void setupBorders();
    bool solveSudoku(int row, int col);
    bool isSafe(int row, int col, int num);
    void checkGameOver();

    QLabel *livesLabel;
    int puzzle[16][16] = {0};
    int solution[16][16] = {0};
    QSet<QPair<int, int>> usedNumbers;
    int lives;
    QTableWidget *tableWidget;
};

#endif // MAINWINDOW_H
