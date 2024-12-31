#include "mainwindow_L.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <ctime>
#include <QSet>
#include <QPair>

QSet<QPair<int, int>> usedNumbers_L;

MainWindow_L::MainWindow_L(QWidget *parent)
    : QMainWindow(parent), tableWidget_L(new QTableWidget(this)), lives_L(3), selectedRow_L(-1), selectedCol_L(-1)
{

    // 設置窗口標題與初始大小
    setWindowTitle("數獨遊戲");
    resize(700, 700);

    // 配置數獨表格
    tableWidget_L->setRowCount(9);
    tableWidget_L->setColumnCount(9);

    // 禁止多選功能，設置為單選模式
    tableWidget_L->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_L->setSelectionBehavior(QAbstractItemView::SelectItems);

    // 隱藏水平與垂直表頭
    tableWidget_L->horizontalHeader()->setVisible(false);
    tableWidget_L->verticalHeader()->setVisible(false);

    // 設置行與列的大小模式為自動調整
    tableWidget_L->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget_L->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 初始化數獨題目與答案
    srand(time(0));
    generateSudokuSolution_L();
    digHoles_L();

    // 設置九宮格邊線
    setupBorders_L();

    // 初始化生命值顯示
    livesLabel_L = new QLabel(QString("生命值：%1").arg(lives_L), this);
    livesLabel_L->setAlignment(Qt::AlignCenter);

    // 設置垂直布局，包含生命值顯示與數獨表格
    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(livesLabel_L);
    verticalLayout->addWidget(tableWidget_L);

    // 設置中央窗口部件並應用布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);

    // 連接信號與槽，用於處理單元格選中與改變事件
    connect(tableWidget_L, &QTableWidget::cellChanged, this, &MainWindow_L::onCellChanged_L);
    connect(tableWidget_L, &QTableWidget::cellClicked, this, &MainWindow_L::onCellClicked_L);
}

MainWindow_L::~MainWindow_L() {}

void MainWindow_L::resizeEvent_L(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    int margin = 10; // 邊距
    int labelHeight = 30; // 生命值標籤的高度
    int tableMarginTop = margin + labelHeight + margin;

    // 調整生命值標籤的位置和大小
    if (livesLabel_L) {
        livesLabel_L->setGeometry(margin, margin, width() - 2 * margin, labelHeight);
    }

    // 調整數獨表格的位置和大小
    if (tableWidget_L) {
        tableWidget_L->setGeometry(
            margin,                    // X 起點
            tableMarginTop,            // Y 起點
            width() - 2 * margin,      // 寬度
            height() - tableMarginTop - margin // 高度
            );
    }
}

bool MainWindow_L::solveSudoku_L(int row, int col) {
    if (row == 9) return true;
    if (col == 9) return solveSudoku_L(row + 1, 0);
    if (puzzle_L[row][col] != 0) return solveSudoku_L(row, col + 1);

    for (int num = 1; num <= 9; ++num) {
        if (isSafe_L(row, col, num)) {
            puzzle_L[row][col] = num;
            if (solveSudoku_L(row, col + 1)) return true;
            puzzle_L[row][col] = 0;
        }
    }
    return false;
}

bool MainWindow_L::isSafe_L(int row, int col, int num) {
    for (int i = 0; i < 9; ++i)
        if (puzzle_L[row][i] == num || puzzle_L[i][col] == num)
            return false;

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (puzzle_L[startRow + i][startCol + j] == num)
                return false;

    return true;
}

void MainWindow_L::generateSudokuSolution_L() {
    memset(puzzle_L, 0, sizeof(puzzle_L));
    for (int i = 0; i < 11; ++i) {
        int row = rand() % 9;
        int col = rand() % 9;
        int num = rand() % 9 + 1;
        if (isSafe_L(row, col, num))
            puzzle_L[row][col] = num;
    }
    solveSudoku_L(0, 0);
    memcpy(solution_L, puzzle_L, sizeof(puzzle_L));
}

void MainWindow_L::digHoles_L() {
    int holes = 20;
    usedNumbers_L.clear();

    while (holes > 0) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (puzzle_L[row][col] != 0 && !usedNumbers_L.contains(QPair<int, int>(row, col))) {
            puzzle_L[row][col] = 0;
            usedNumbers_L.insert(QPair<int, int>(row, col));
            --holes;
        }
    }

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item;
            if (puzzle_L[row][col] != 0) {
                item = new QTableWidgetItem(QString::number(puzzle_L[row][col]));
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            } else {
                item = new QTableWidgetItem("");
            }
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget_L->setItem(row, col, item);
        }
    }
}

void MainWindow_L::setupBorders_L() {
    QVector<QString> colors =
        {
            "#FFCCCC", "#CCFFCC", "#CCCCFF",
            "#CC99FF", "#CCFFFF", "#FFD9CC",
            "#CCFFD9", "#D9CCFF", "#FFD9FF",

        };

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = tableWidget_L->item(row, col);
            if (!item) {
                item = new QTableWidgetItem("");
                tableWidget_L->setItem(row, col, item);
            }

            int gridIndex = (row / 3) * 3 + (col / 3);
            QString style = QString("border: 1px solid gray; background-color: %1;").arg(colors[gridIndex]);

            if (col % 3 == 0) style += "border-left: 2px solid black;";
            if (row % 3 == 0) style += "border-top: 2px solid black;";

            item->setBackground(QBrush(QColor(colors[gridIndex])));
        }
    }
}

void MainWindow_L::onCellChanged_L(int row, int column) {
    QTableWidgetItem *item = tableWidget_L->item(row, column);
    if (!item || row != selectedRow_L || column != selectedCol_L) return;

    QString input = item->text();
    bool ok;
    int value = input.toInt(&ok);

    // 斷開信號連接，防止無窮循環
    disconnect(tableWidget_L, &QTableWidget::cellChanged, this, &MainWindow_L::onCellChanged_L);

    if (!ok || value != solution_L[row][column]) {
        --lives_L;
        livesLabel_L->setText(QString("生命值：%1").arg(lives_L));
        item->setText("");
        QMessageBox::warning(this, "錯誤", "輸入錯誤！");
        checkGameOver_L();
    } else {
        checkWinCondition_L();
    }
    connect(tableWidget_L, &QTableWidget::cellChanged, this, &MainWindow_L::onCellChanged_L);
}

void MainWindow_L::onCellClicked_L(int row, int column) {
    QTableWidgetItem *item = tableWidget_L->item(row, column);
    if (item && puzzle_L[row][column] == 0) {
        selectedRow_L = row;
        selectedCol_L = column;
    } else {
        selectedRow_L = -1;
        selectedCol_L = -1;
    }
}

void MainWindow_L::checkWinCondition_L() {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = tableWidget_L->item(row, col);
            if (!item || item->text().toInt() != solution_L[row][col])
                return;
        }
    }
    QMessageBox::information(this, "恭喜過關", "你完成了數獨！");
    QApplication::quit();
}

void MainWindow_L::checkGameOver_L() {
    if (lives_L <= 0) {
        QMessageBox::critical(this, "遊戲結束", "生命值耗盡，遊戲結束！");
        QApplication::quit();
    }
}
