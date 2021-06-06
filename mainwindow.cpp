#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->statusbar->showMessage("Loading...");

    this->setWindowTitle("数独");
//    this->setCentralWidget(ui->tableWidget);
    this->setFixedHeight(453+28);
    this->setFixedSize(this->size());
//    ui->tableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
//    ui->tableWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);

    // enable custom grid drawing
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setItemDelegate(new SudokuGridPainter(this));

    // initialize table items (empty yet)
    ui->tableWidget->blockSignals(true);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            ui->tableWidget->setItem(i, j, new QTableWidgetItem);
            ui->tableWidget->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }
    ui->tableWidget->blockSignals(false);

    // init table items (generate puzzle)
    // fill in given digits, stylize as !editable & !selectable & bold
//    this->UpdateTableContents();
    this->SwitchPane(0);            // game not initiated

    ui->statusbar->showMessage("Ready.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionClose_triggered()
{
    QApplication::quit();
}

// SudokuGame -> tableVal -> tableWidget
void MainWindow::UpdateTableContents()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (game.isGiven(i, j))
                tableVal[i][j] = game.at(i, j);
            else
                tableVal[i][j] = 0;
        }
    }

    ui->tableWidget->blockSignals(true);

    QFont Reg, Bold;
    Reg = Bold = ui->tableWidget->font();
    Bold.setBold(true);

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            auto *it = ui->tableWidget->item(i, j);
            if (game.isGiven(i, j)) {
                it->setText(QString::number(tableVal[i][j]));
                it->setFlags(it->flags()
                             & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
                it->setFont(Bold);
            } else {
                it->setText(QString(""));
                it->setFlags(it->flags()
                             | Qt::ItemIsEditable | Qt::ItemIsSelectable);
                it->setFont(Reg);
            }
        }
    }

    ui->tableWidget->blockSignals(false);
}

// tableWidget -> tableVal -> SudokuGame
void MainWindow::InputTableContents()
{
}

void MainWindow::SwitchPane(int to)
{
    ui->stackedWidget->setCurrentIndex(to);
    this->nowPane = to;
}

void MainWindow::on_actionNew_Game_triggered()
{
    // TODO remove digits cnt
    this->game.GeneratePuzzle();
    this->game.RemoveDigits(81 - 10);
    this->UpdateTableContents();
}

// synchronizes tableVal with shown contents
void MainWindow::on_tableWidget_cellChanged(int a, int b)
{
    QString status;
    QTextStream Stream(&status);

    QString cellText = ui->tableWidget->item(a, b)->text();
    QString purified = cellText;
    purified.replace(QRegExp("\\s*"), "");
    int cellVal = purified.toInt();

    qDebug() << a << b << "celltext" << cellText << "int" << cellVal;

    if (purified.length() == 0
            || (purified.length() == 1 && purified[0] == '0')) {
        Stream << "Cleared contents in (" << a + 1 << ", " << b + 1 << ").";
        tableVal[a][b] = 0;

        ui->tableWidget->blockSignals(true);
        ui->tableWidget->item(a, b)->setText("");
        ui->tableWidget->blockSignals(false);

        ui->statusbar->showMessage(status);
    } else if (QString::number(cellVal) != purified
               || cellVal > 9 || cellVal < 1) {
        Stream << "Invalid input: " << cellText << ".";
        qDebug() << "status" << status;

        ui->tableWidget->blockSignals(true);
        if (!tableVal[a][b])
            ui->tableWidget->item(a, b)->setText("");
        else
            ui->tableWidget->item(a, b)->
                    setText(QString::number(tableVal[a][b]));
        ui->tableWidget->blockSignals(false);

        ui->statusbar->showMessage(status);
    } else {
        Stream << "Change (" << a + 1 << ", " << b + 1 << ") to ";
        Stream << cellVal << ".";
        tableVal[a][b] = cellVal;

        ui->tableWidget->blockSignals(true);
        ui->tableWidget->item(a, b)->setText(QString::number(tableVal[a][b]));
        ui->tableWidget->blockSignals(false);

        ui->statusbar->showMessage(status);
    }
}

void SudokuGridPainter::paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const {
    // draws a typical 3x3x9 Sudoku grid line
    QItemDelegate::paint(painter, option, index);
    int a = index.column(), b = index.row();
    auto &rect = option.rect;
    painter->setPen(QPen(Qt::gray, 1));
    painter->drawRect(rect);
    painter->setPen(QPen(Qt::black, 2));
    for (int k = 0; k < 6; k += 3) {
        if (b == 2 + k)
            painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        if (b == 3 + k)
            painter->drawLine(rect.topLeft(), rect.topRight());
        if (a == 2 + k)
            painter->drawLine(rect.topRight(), rect.bottomRight());
        if (a == 3 + k)
            painter->drawLine(rect.topLeft(), rect.bottomLeft());
    }
}

void MainWindow::on_actionValidate_Answer_triggered()
{
    bool valid = game.isValid(&tableVal);
    QString status = "Answer ";
    if (valid) status += "valid!";
    else       status += "invalid!";
    ui->statusbar->showMessage(status);
}

void MainWindow::on_actionNew_Game_from_user_input_triggered()
{
    // TODO show msgbox: do you want to discard current game & start new?
    // change cell style -> editable
    QFont Reg, Bold;
    Reg = Bold = ui->tableWidget->font();
    Bold.setBold(true);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            auto *it = ui->tableWidget->item(i, j);
            it->setText(QString(""));
            it->setFlags(it->flags()
                         | Qt::ItemIsEditable | Qt::ItemIsSelectable);
            it->setFont(Reg);
        }
    }
    ui->statusbar->showMessage(QString("Please input puzzle..."));
}

void MainWindow::on_actionHint_triggered()
{

}
