#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <ctime>
#include <QSet>
#include <QPair>

QSet<QPair<int, int>> usedNumbers;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), tableWidget(new QTableWidget(this)), lives(3), selectedRow(-1), selectedCol(-1)
{
    // 設置窗口標題與初始大小
    setWindowTitle("數獨遊戲");
    resize(700, 700);

    // 配置數獨表格
    tableWidget->setRowCount(16);
    tableWidget->setColumnCount(16);

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
    srand(time(0));
    generateSudokuSolution();
    digHoles();

    // 設置九宮格邊線
    setupBorders();

    // 初始化生命值顯示
    livesLabel = new QLabel(QString("生命值：%1").arg(lives), this);
    livesLabel->setAlignment(Qt::AlignCenter);

    // 設置垂直布局，包含生命值顯示與數獨表格
    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(livesLabel);
    verticalLayout->addWidget(tableWidget);

    // 設置中央窗口部件並應用布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);

    // 連接信號與槽，用於處理單元格選中與改變事件
    connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged);
    connect(tableWidget, &QTableWidget::cellClicked, this, &MainWindow::onCellClicked);
}

MainWindow::~MainWindow() {}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    int margin = 10; // 邊距
    int labelHeight = 30; // 生命值標籤的高度
    int tableMarginTop = margin + labelHeight + margin;

    // 調整生命值標籤的位置和大小
    if (livesLabel) {
        livesLabel->setGeometry(margin, margin, width() - 2 * margin, labelHeight);
    }

    // 調整數獨表格的位置和大小
    if (tableWidget) {
        tableWidget->setGeometry(
            margin,                    // X 起點
            tableMarginTop,            // Y 起點
            width() - 2 * margin,      // 寬度
            height() - tableMarginTop - margin // 高度
            );
    }
}


bool MainWindow::solveSudoku(int row, int col) {
    if (row == 16) return true;
    if (col == 16) return solveSudoku(row + 1, 0);
    if (puzzle[row][col] != 0) return solveSudoku(row, col + 1);

    for (int num = 1; num <= 16; ++num) {
        if (isSafe(row, col, num)) {
            puzzle[row][col] = num;
            if (solveSudoku(row, col + 1)) return true;
            puzzle[row][col] = 0;
        }
    }
    return false;
}

bool MainWindow::isSafe(int row, int col, int num) {
    for (int i = 0; i < 16; ++i)
        if (puzzle[row][i] == num || puzzle[i][col] == num)
            return false;

    int startRow = row - row % 4, startCol = col - col % 4;
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            if (puzzle[startRow + i][startCol + j] == num)
                return false;

    return true;
}

void MainWindow::generateSudokuSolution() {
    memset(puzzle, 0, sizeof(puzzle));
    for (int i = 0; i < 18; ++i) {
        int row = rand() % 16;
        int col = rand() % 16;
        int num = rand() % 16 + 1;
        if (isSafe(row, col, num))
            puzzle[row][col] = num;
    }
    solveSudoku(0, 0);
    memcpy(solution, puzzle, sizeof(puzzle));
}

void MainWindow::digHoles() {
    int holes = 100;
    usedNumbers.clear();
    while (holes > 0) {
        int row = rand() % 16;
        int col = rand() % 16;
        if (puzzle[row][col] != 0 && !usedNumbers.contains(QPair<int, int>(row, col))) {
            puzzle[row][col] = 0;
            usedNumbers.insert(QPair<int, int>(row, col));
            --holes;
        }
    }
    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
            QTableWidgetItem *item;
            if (puzzle[row][col] != 0) {
                item = new QTableWidgetItem(QString::number(puzzle[row][col]));
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            } else {
                item = new QTableWidgetItem("");
            }
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(row, col, item);
        }
    }
}

void MainWindow::setupBorders() {
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

            int gridIndex = (row / 4) * 4 + (col / 4);
            QString style = QString("border: 1px solid gray; background-color: %1;").arg(colors[gridIndex]);

            if (col % 4 == 0) style += "border-left: 2px solid black;";
            if (row % 4 == 0) style += "border-top: 2px solid black;";

            item->setBackground(QBrush(QColor(colors[gridIndex])));
        }
    }
}

void MainWindow::onCellChanged(int row, int column) {
    QTableWidgetItem *item = tableWidget->item(row, column);
    if (!item || row != selectedRow || column != selectedCol) return;

    disconnect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged);

    QString input = item->text();
    bool ok;
    int value = input.toInt(&ok);

    if (!ok || value != solution[row][column]) {
        --lives;
        livesLabel->setText(QString("生命值：%1").arg(lives));
        item->setText("");
        QMessageBox::warning(this, "錯誤", "輸入錯誤！");
        checkGameOver();
    } else {
        checkWinCondition();
    }

    connect(tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged);
}

void MainWindow::onCellClicked(int row, int column) {
    QTableWidgetItem *item = tableWidget->item(row, column);
    if (item && puzzle[row][column] == 0) {
        selectedRow = row;
        selectedCol = column;
    } else {
        selectedRow = -1;
        selectedCol = -1;
    }
}

void MainWindow::checkWinCondition() {
    for (int row = 0; row < 16; ++row) {
        for (int col = 0; col < 16; ++col) {
            QTableWidgetItem *item = tableWidget->item(row, col);
            if (!item || item->text().toInt() != solution[row][col])
                return;
        }
    }
    QMessageBox::information(this, "恭喜過關", "你完成了數獨！");
    QApplication::quit();
}

void MainWindow::checkGameOver() {
    if (lives <= 0) {
        QMessageBox::critical(this, "遊戲結束", "生命值耗盡，遊戲結束！");
        QApplication::quit();
    }
}
