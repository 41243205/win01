#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <ctime>
#include <QSet>
#include <QPair>

// 用於記錄已使用過的行與列組合，避免重複
QSet<QPair<int, int>> usedNumbers;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), tableWidget(new QTableWidget(this)), lives(3)
{
    // 設置窗口標題與大小
    setWindowTitle("數獨遊戲");
    resize(425, 475);

    // 配置數獨表格
    tableWidget->setRowCount(16);  // 設置表格行數
    tableWidget->setColumnCount(16);  // 設置表格列數
    tableWidget->setFixedSize(500, 500);  // 固定表格大小
    tableWidget->setStyleSheet("QTableWidget { gridline-color: black; }");

    // 隱藏水平與垂直表頭
    tableWidget->horizontalHeader()->setVisible(false);
    tableWidget->verticalHeader()->setVisible(false);

    // 設置行與列的大小模式為自動調整
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 初始化數獨題目與答案
    srand(time(0)); // 初始化隨機數生成器（基於當前時間）
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

    // 使用水平布局將內容居中
    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));
    horizontalLayout->addLayout(verticalLayout);
    horizontalLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Expanding, QSizePolicy::Minimum));

    // 設置中央窗口部件並應用布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(horizontalLayout);
    setCentralWidget(centralWidget);

    // 連接信號與槽，用於處理單元格改變事件
    connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged);
}

MainWindow::~MainWindow() {}

// 使用回溯法解決數獨
bool MainWindow::solveSudoku(int row, int col)
{
    if (row == 16) return true; // 若已完成所有行，返回成功
    if (col == 16) return solveSudoku(row + 1, 0); // 若已完成當前行，進入下一行
    if (puzzle[row][col] != 0) return solveSudoku(row, col + 1); // 若當前格子已填，移至下一列

    for (int num = 1; num <= 16; ++num) {
        if (isSafe(row, col, num)) { // 檢查是否可以填入數字
            puzzle[row][col] = num; // 填入數字
            if (solveSudoku(row, col + 1)) return true; // 遞迴求解
            puzzle[row][col] = 0; // 還原格子內容
        }
    }
    return false; // 無法解決返回失敗
}

// 檢查在指定位置填入數字是否安全
bool MainWindow::isSafe(int row, int col, int num)
{
    // 檢查行與列中是否已存在該數字
    for (int i = 0; i < 16; ++i)
        if (puzzle[row][i] == num || puzzle[i][col] == num)
            return false;

    // 檢查九宮格內是否已存在該數字
    int startRow = row - row % 4, startCol = col - col % 4;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (puzzle[startRow + i][startCol + j] == num)
                return false;

    return true; // 若安全返回true
}

// 生成完整的數獨解答
void MainWindow::generateSudokuSolution()
{
    memset(puzzle, 0, sizeof(puzzle)); // 清空數獨解答表

    // 隨機填入初始數字，構建基礎解答
    for (int i = 0; i < 18; ++i) {
        int row = rand() % 16;
        int col = rand() % 16;
        int num = rand() % 16 + 1;
        if (isSafe(row, col, num))
            puzzle[row][col] = num;
    }

    // 使用回溯法生成完整解答
    solveSudoku(0, 0);
    memcpy(solution, puzzle, sizeof(puzzle)); // 保存解答
}

// 挖空生成數獨題目
void MainWindow::digHoles()
{
    int holes = 100; // 設置要挖的空格數量
    usedNumbers.clear(); // 清空已使用記錄

    while (holes > 0) {
        int row = rand() % 16;
        int col = rand() % 16;
        if (puzzle[row][col] != 0 && !usedNumbers.contains(QPair<int, int>(row, col))) {
            puzzle[row][col] = 0; // 將該位置挖空
            usedNumbers.insert(QPair<int, int>(row, col));
            --holes; // 減少剩餘空格數量
        }
    }

    // 將生成的題目填入表格
    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
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

// 設置十六宮格邊線樣式
void MainWindow::setupBorders()
{
    // 定義16種顏色，每個4x4宮格用不同的顏色
    QVector<QString> colors = {
        "#FFCCCC", "#CCFFCC", "#CCCCFF", "#FFFFCC",
        "#CC99FF", "#CCFFFF", "#FFD9CC", "#D9FFCC",
        "#CCFFD9", "#D9CCFF", "#FFD9FF", "#FFCC99",
        "#99CCFF", "#FF99CC", "#99FFCC", "#FFCCFF"
    };

    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (!item) {
                item = new QTableWidgetItem("");
                tableWidget->setItem(row, col, item);
            }

            // 計算所在的小宮格編號
            int gridIndex = (row / 4) * 4 + (col / 4);
            QString style = QString("border: 1px solid gray; background-color: %1;").arg(colors[gridIndex]);

            // 設置九宮格邊線
            if (col % 4 == 0) style += "border-left: 2px solid black;";
            if (row % 4 == 0) style += "border-top: 2px solid black;";
            item->setData(Qt::UserRole, style); // 儲存樣式資料

            // 設置樣式到單元格
            item->setBackground(QBrush(QColor(colors[gridIndex])));
        }
    }
}


// 單元格改變時的處理邏輯
void MainWindow::onCellChanged(int row, int column)
{
    QTableWidgetItem *item = tableWidget->item(row, column);
    if (!item) return; // 確保項目有效

    disconnect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged); // 暫時斷開信號

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

    connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged); // 恢復信號連接
}

// 檢查是否完成遊戲
void MainWindow::checkWinCondition()
{
    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (!item || item->text().toInt() != solution[row][col])
                return; // 若尚未完成返回
        }
    }
    QMessageBox::information(this, "恭喜過關", "你完成了數獨！");
    QApplication::quit(); // 遊戲結束
}

// 檢查遊戲是否結束
void MainWindow::checkGameOver()
{
    if (lives <= 0) {
        QMessageBox::critical(this, "遊戲結束", "生命值耗盡，遊戲結束！");
        QApplication::quit(); // 結束遊戲
    }
}
