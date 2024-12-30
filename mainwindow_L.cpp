#include "mainwindow_L.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <ctime>
#include <QSet>
#include <QPair>

// 用於記錄已使用過的行與列組合，避免重複
MainWindow_L::MainWindow_L(QWidget *parent)
    : QMainWindow(parent), tableWidget(new QTableWidget(this)), lives(3)
{
    // 設置窗口標題
    setWindowTitle("數獨遊戲");

    // 配置數獨表格
    tableWidget->setRowCount(9);
    tableWidget->setColumnCount(9);

    // 設置初始窗口大小
    int cellSize = 50; // 預設每個單元格大小
    int initialWidth = cellSize * 9 + 20;  // 預設表格加上邊框
    int initialHeight = cellSize * 9 + 60; // 加上標題和控件空間
    resize(initialWidth, initialHeight);   // 設定初始大小


    // 禁止多選功能，設置為單選模式
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);

    // 隱藏水平與垂直表頭
    tableWidget->horizontalHeader()->setVisible(false);
    tableWidget->verticalHeader()->setVisible(false);

    // 設置行與列的大小模式為自動調整
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    // 初始化數獨題目與答案
    srand(time(0)); // 初始化隨機數生成器
    generateSudokuSolution(); // 生成完整的數獨解答
    digHoles(); // 挖空生成數獨題目

    // 設置九宮格邊線
    setupBorders();

    // 初始化生命值顯示
    livesLabel = new QLabel(QString("生命值：%1").arg(lives), this);
    livesLabel->setAlignment(Qt::AlignCenter); // 文字置中顯示

    // 設置垂直布局，包含生命值顯示與數獨表格
    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(livesLabel);
    verticalLayout->addWidget(tableWidget);

    // 設置中央窗口部件並應用布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);

    // 連接信號與槽
    connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow_L::onCellChanged);
}



MainWindow_L::~MainWindow_L() {}

// 窗口大小變化事件
void MainWindow_L::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    // 獲取當前窗口大小
    int windowWidth = event->size().width();
    int windowHeight = event->size().height();

    // 動態調整行高和列寬，使表格填滿窗口
    int cellSize = qMin(windowWidth / 9, windowHeight / 9); // 每個格子的大小
    for (int i = 0; i < 9; ++i) {
        tableWidget->setRowHeight(i, cellSize);
        tableWidget->setColumnWidth(i, cellSize);
    }
}




// 使用回溯法解決數獨
bool MainWindow_L::solveSudoku(int row, int col)
{
    if (row == 9) return true; // 若已完成所有行，返回成功
    if (col == 9) return solveSudoku(row + 1, 0); // 若已完成當前行，進入下一行
    if (puzzle[row][col] != 0) return solveSudoku(row, col + 1); // 若當前格子已填，移至下一列

    for (int num = 1; num <= 9; ++num) {
        if (isSafe(row, col, num)) { // 檢查是否可以填入數字
            puzzle[row][col] = num; // 填入數字
            if (solveSudoku(row, col + 1)) return true; // 遞迴求解
            puzzle[row][col] = 0; // 還原格子內容
        }
    }
    return false; // 無法解決返回失敗
}

// 檢查在指定位置填入數字是否安全
bool MainWindow_L::isSafe(int row, int col, int num)
{
    // 檢查行與列中是否已存在該數字
    for (int i = 0; i < 9; ++i)
        if (puzzle[row][i] == num || puzzle[i][col] == num)
            return false;

    // 檢查九宮格內是否已存在該數字
    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (puzzle[startRow + i][startCol + j] == num)
                return false;

    return true; // 若安全返回true
}

// 生成完整的數獨解答
void MainWindow_L::generateSudokuSolution()
{
    memset(puzzle, 0, sizeof(puzzle)); // 清空數獨解答表

    // 隨機填入初始數字，構建基礎解答
    for (int i = 0; i < 11; ++i) {
        int row = rand() % 9;
        int col = rand() % 9;
        int num = rand() % 9 + 1;
        if (isSafe(row, col, num))
            puzzle[row][col] = num;
    }

    // 使用回溯法生成完整解答
    solveSudoku(0, 0);
    memcpy(solution, puzzle, sizeof(puzzle)); // 保存解答
}

// 挖空生成數獨題目
void MainWindow_L::digHoles()
{
    int holes = 20; // 設置要挖的空格數量
    usedNumbers.clear(); // 清空已使用記錄

    while (holes > 0) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (puzzle[row][col] != 0 && !usedNumbers.contains(QPair<int, int>(row, col))) {
            puzzle[row][col] = 0; // 將該位置挖空
            usedNumbers.insert(QPair<int, int>(row, col));
            --holes; // 減少剩餘空格數量
        }
    }

    // 將生成的題目填入表格
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item;
            if (puzzle[row][col] != 0) {
                item = new QTableWidgetItem(QString::number(puzzle[row][col]));
                item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 禁止編輯
            } else {
                item = new QTableWidgetItem("");
            }
            item->setTextAlignment(Qt::AlignCenter); // 置中顯示文字
            tableWidget->setItem(row, col, item);
        }
    }
}

// 設置九宮格邊線和背景色樣式
void MainWindow_L::setupBorders()
{
    // 定義9種不同的顏色，用於分別標記每個3x3宮格
    QVector<QString> colors = {
        "#FFCCCC", "#CCFFCC", "#CCCCFF", "#FFFFCC",
        "#FFCCFF", "#CCFFFF", "#FFD9CC", "#D9FF99", "#99CCFF"
    };

    // 為每個單元格設置樣式
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (!item) {
                item = new QTableWidgetItem("");
                tableWidget->setItem(row, col, item);
            }

            // 計算當前格子所在的3x3宮格索引
            int blockIndex = (row / 3) * 3 + (col / 3);
            QString style = QString("border: 1px solid gray; background-color: %1;").arg(colors[blockIndex]);

            // 設置九宮格粗邊線
            if (col % 3 == 0) style += "border-left: 2px solid black;";
            if (row % 3 == 0) style += "border-top: 2px solid black;";

            // 儲存樣式並應用
            item->setData(Qt::UserRole, style);
            item->setBackground(QBrush(QColor(colors[blockIndex])));
        }
    }
}

// 單元格改變時的處理邏輯
void MainWindow_L::onCellChanged(int row, int column)
{
    QTableWidgetItem *item = tableWidget->item(row, column);
    if (!item) return; // 確保項目有效

    disconnect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow_L::onCellChanged); // 暫時斷開信號

    QString input = item->text(); // 獲取用戶輸入
    bool ok;
    int value = input.toInt(&ok);

    if (!ok || value != solution[row][column]) { // 若輸入無效或錯誤
        --lives; // 減少生命值
        livesLabel->setText(QString("生命值：%1").arg(lives));
        item->setText(""); // 清空錯誤輸入
        QMessageBox::warning(this, "錯誤", "輸入錯誤！");
        checkGameOver(); // 檢查遊戲是否結束
    } else {
        checkWinCondition(); // 檢查是否完成遊戲
    }

    connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow_L::onCellChanged); // 恢復信號連接
}

// 檢查是否完成遊戲
void MainWindow_L::checkWinCondition()
{
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (!item || item->text().toInt() != solution[row][col])
                return; // 若尚未完成返回
        }
    }
    QMessageBox::information(this, "恭喜過關", "你完成了數獨！");
    QApplication::quit(); // 遊戲結束
}

// 檢查遊戲是否結束
void MainWindow_L::checkGameOver()
{
    if (lives <= 0) {
        QMessageBox::critical(this, "遊戲結束", "生命值耗盡，遊戲結束！");
        QApplication::quit(); // 結束遊戲
    }
}
