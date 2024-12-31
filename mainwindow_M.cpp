#include "mainwindow_M.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <cstdlib>
#include <ctime>
#include <QSet>
#include <QPair>

QSet<QPair<int, int>> usedNumbers_M;

MainWindow_M::MainWindow_M(QWidget *parent)
    : QMainWindow(parent), tableWidget_M(new QTableWidget(this)), lives_M(3), selectedRow_M(-1), selectedCol_M(-1)
{

    // 設置窗口標題與初始大小
    setWindowTitle("數獨遊戲");
    resize(700, 700);

    // 配置數獨表格
    tableWidget_M->setRowCount(9);
    tableWidget_M->setColumnCount(9);

    // 禁止多選功能，設置為單選模式
    tableWidget_M->setSelectionMode(QAbstractItemView::SingleSelection);
    tableWidget_M->setSelectionBehavior(QAbstractItemView::SelectItems);

    // 隱藏水平與垂直表頭
    tableWidget_M->horizontalHeader()->setVisible(false);
    tableWidget_M->verticalHeader()->setVisible(false);

    // 設置行與列的大小模式為自動調整
    tableWidget_M->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget_M->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 初始化數獨題目與答案
    srand(time(0));
    generateSudokuSolution_M();
    digHoles_M();

    // 設置九宮格邊線
    setupBorders_M();

    // 初始化生命值顯示
    livesLabel_M = new QLabel(QString("生命值：%1").arg(lives_M), this);
    livesLabel_M->setAlignment(Qt::AlignCenter);

    // 設置垂直布局，包含生命值顯示與數獨表格
    QVBoxLayout *verticalLayout = new QVBoxLayout;
    verticalLayout->addWidget(livesLabel_M);
    verticalLayout->addWidget(tableWidget_M);

    // 設置中央窗口部件並應用布局
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(verticalLayout);
    setCentralWidget(centralWidget);

    // 連接信號與槽，用於處理單元格選中與改變事件
    connect(tableWidget_M, &QTableWidget::cellChanged, this, &MainWindow_M::onCellChanged_M);
    connect(tableWidget_M, &QTableWidget::cellClicked, this, &MainWindow_M::onCellClicked_M);
}

MainWindow_M::~MainWindow_M() {}

void MainWindow_M::resizeEvent_M(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    int margin = 10; // 邊距
    int labelHeight = 30; // 生命值標籤的高度
    int tableMarginTop = margin + labelHeight + margin;

    // 調整生命值標籤的位置和大小
    if (livesLabel_M) {
        livesLabel_M->setGeometry(margin, margin, width() - 2 * margin, labelHeight);
    }

    // 調整數獨表格的位置和大小
    if (tableWidget_M) {
        tableWidget_M->setGeometry(
            margin,                    // X 起點
            tableMarginTop,            // Y 起點
            width() - 2 * margin,      // 寬度
            height() - tableMarginTop - margin // 高度
            );
    }
}

bool MainWindow_M::solveSudoku_M(int row, int col) {
    if (row == 9) return true;
    if (col == 9) return solveSudoku_M(row + 1, 0);
    if (puzzle_M[row][col] != 0) return solveSudoku_M(row, col + 1);

    for (int num = 1; num <= 9; ++num) {
        if (isSafe_M(row, col, num)) {
            puzzle_M[row][col] = num;
            if (solveSudoku_M(row, col + 1)) return true;
            puzzle_M[row][col] = 0;
        }
    }
    return false;
}

bool MainWindow_M::isSafe_M(int row, int col, int num) {
    for (int i = 0; i < 9; ++i)
        if (puzzle_M[row][i] == num || puzzle_M[i][col] == num)
            return false;

    int startRow = row - row % 3, startCol = col - col % 3;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (puzzle_M[startRow + i][startCol + j] == num)
                return false;

    return true;
}

void MainWindow_M::generateSudokuSolution_M() {
    memset(puzzle_M, 0, sizeof(puzzle_M));
    for (int i = 0; i < 11; ++i) {
        int row = rand() % 9;
        int col = rand() % 9;
        int num = rand() % 9 + 1;
        if (isSafe_M(row, col, num))
            puzzle_M[row][col] = num;
    }
    solveSudoku_M(0, 0);
    memcpy(solution_M, puzzle_M, sizeof(puzzle_M));
}

void MainWindow_M::digHoles_M() {
    int holes = 40;
    usedNumbers_M.clear();

    while (holes > 0) {
        int row = rand() % 9;
        int col = rand() % 9;
        if (puzzle_M[row][col] != 0 && !usedNumbers_M.contains(QPair<int, int>(row, col))) {
            puzzle_M[row][col] = 0;
            usedNumbers_M.insert(QPair<int, int>(row, col));
            --holes;
        }
    }

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item;
            if (puzzle_M[row][col] != 0) {
                item = new QTableWidgetItem(QString::number(puzzle_M[row][col]));
                item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            } else {
                item = new QTableWidgetItem("");
            }
            item->setTextAlignment(Qt::AlignCenter);
            tableWidget_M->setItem(row, col, item);
        }
    }
}

void MainWindow_M::setupBorders_M() {
    QVector<QString> colors =
        {
            "#FFCCCC", "#CCFFCC", "#CCCCFF",
            "#CC99FF", "#CCFFFF", "#FFD9CC",
            "#CCFFD9", "#D9CCFF", "#FFD9FF",

        };

    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = tableWidget_M->item(row, col);
            if (!item) {
                item = new QTableWidgetItem("");
                tableWidget_M->setItem(row, col, item);
            }

            int gridIndex = (row / 3) * 3 + (col / 3);
            QString style = QString("border: 1px solid gray; background-color: %1;").arg(colors[gridIndex]);

            if (col % 3 == 0) style += "border-left: 2px solid black;";
            if (row % 3 == 0) style += "border-top: 2px solid black;";

            item->setBackground(QBrush(QColor(colors[gridIndex])));
        }
    }
}

void MainWindow_M::onCellChanged_M(int row, int column) {
    QTableWidgetItem *item = tableWidget_M->item(row, column);
    if (!item || row != selectedRow_M || column != selectedCol_M) return;

    QString input = item->text();
    bool ok;
    int value = input.toInt(&ok);

    // 斷開信號連接，防止無窮循環
    disconnect(tableWidget_M, &QTableWidget::cellChanged, this, &MainWindow_M::onCellChanged_M);

    if (!ok || value != solution_M[row][column]) {
        --lives_M;
        livesLabel_M->setText(QString("生命值：%1").arg(lives_M));
        item->setText("");
        QMessageBox::warning(this, "錯誤", "輸入錯誤！");
        checkGameOver_M();
    } else {
        checkWinCondition_M();
    }
    connect(tableWidget_M, &QTableWidget::cellChanged, this, &MainWindow_M::onCellChanged_M);
}

void MainWindow_M::onCellClicked_M(int row, int column) {
    QTableWidgetItem *item = tableWidget_M->item(row, column);
    if (item && puzzle_M[row][column] == 0) {
        selectedRow_M = row;
        selectedCol_M = column;
    } else {
        selectedRow_M = -1;
        selectedCol_M = -1;
    }
}

void MainWindow_M::checkWinCondition_M() {
    for (int row = 0; row < 9; ++row) {
        for (int col = 0; col < 9; ++col) {
            QTableWidgetItem *item = tableWidget_M->item(row, col);
            if (!item || item->text().toInt() != solution_M[row][col])
                return;
        }
    }
    QMessageBox::information(this, "恭喜過關", "你完成了數獨！");
    QApplication::quit();
}

void MainWindow_M::checkGameOver_M() {
    if (lives_M <= 0) {
        QMessageBox::critical(this, "遊戲結束", "生命值耗盡，遊戲結束！");
        QApplication::quit();
    }
}
