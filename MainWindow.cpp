#include "MainWindow.h"
#include "mainwindow_H.h"
#include "mainwindow_L.h"
#include "mainwindow_M.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), sudokuGame_H(nullptr),sudokuGame_L(nullptr),sudokuGame_M(nullptr)
{
    setWindowTitle("數獨遊戲");
    resize(400, 300);

    titleLabel = new QLabel("數獨遊戲", this);
    titleLabel->setAlignment(Qt::AlignCenter);

    startButton = new QPushButton("開始遊戲", this);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartGameClicked);

    difficultyComboBox = new QComboBox(this);
    difficultyComboBox->addItem("選擇難度");
    difficultyComboBox->addItem("初級");
    difficultyComboBox->addItem("中級");
    difficultyComboBox->addItem("高級");

    rulesButton = new QPushButton("規則", this);
    connect(rulesButton, &QPushButton::clicked, this, &MainWindow::onRulesClicked);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(titleLabel);
    layout->addWidget(startButton);
    layout->addWidget(difficultyComboBox);
    layout->addWidget(rulesButton);

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    this->show();
}

MainWindow::~MainWindow()
{
    if (sudokuGame_H != nullptr) {
        delete sudokuGame_H;
    }
    if (sudokuGame_L != nullptr) {
        delete sudokuGame_L;
    }
    if (sudokuGame_M != nullptr) {
        delete sudokuGame_M;
    }
}

void MainWindow::onStartGameClicked()
{
    QString difficulty = difficultyComboBox->currentText();

    if (difficulty == "選擇難度") {
        QMessageBox::warning(this, "警告", "請選擇難度！");
        return;
    }


    // 根據難度創建不同的數獨遊戲



    if (difficulty == "初級") {
        if (sudokuGame_L != nullptr) {
            delete sudokuGame_L;
        }

        sudokuGame_L = new MainWindow_L(this);
        sudokuGame_L->generateSudokuSolution_L();
        sudokuGame_L->digHoles_L();
        sudokuGame_L->setupBorders_L();
        sudokuGame_L->show();
    } else if (difficulty == "中級") {
        // 中級難度的生成邏輯
        if (sudokuGame_M != nullptr) {
            delete sudokuGame_M;
        }

        sudokuGame_M = new MainWindow_M(this);
        sudokuGame_M->generateSudokuSolution_M();
        sudokuGame_M->digHoles_M();
        sudokuGame_M->setupBorders_M();
        sudokuGame_M->show();
    } else if (difficulty == "高級") {
        // 高級難度的生成邏輯
        if (sudokuGame_H != nullptr) {
            delete sudokuGame_H;
        }
        sudokuGame_H = new MainWindow_H(this);
        sudokuGame_H->generateSudokuSolution();
        sudokuGame_H->digHoles();
        sudokuGame_H->setupBorders();
        sudokuGame_H->show();
    }
}

void MainWindow::onRulesClicked()
{
    QMessageBox::information(this, "數獨遊戲規則",
                             "1. (高級)每個數字 1 至 16 在每行、每列、每 4x4 區域中只能出現一次。\n"
                             "   (初、中級)每個數字 1 至 16 在每行、每列、每 4x4 區域中只能出現一次。\n"
                             "2. 開始遊戲後，你需要填入正確的數字，直到解開數獨。\n"
                             "3. 每次輸入錯誤會扣除一條生命，生命為 3 條，當生命值為 0 時，遊戲結束。");
}
