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

//    ui->tableWidget->setFocusPolicy(Qt::NoFocus);

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
    memset(tableVal, 0, sizeof(tableVal));

    ui->tableWidget->blockSignals(false);

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
void MainWindow::UpdateTableContents(bool replace)
{
    if (replace)
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                tableVal[i][j] = game.at(i, j);
            }
        }

    ui->tableWidget->blockSignals(true);
    ui->tableWidget->setCurrentItem(nullptr);

    QFont Reg, Bold;
    Reg = Bold = ui->tableWidget->font();
    Bold.setBold(true);

    remcnt = 81;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            auto *it = ui->tableWidget->item(i, j);
            if (game.at(i, j)) {
                --remcnt;
                it->setText(QString::number(tableVal[i][j]));
                it->setFlags(it->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
                it->setFont(Bold);
            } else {
                if (tableVal[i][j]) {
                    --remcnt;
                    it->setText(QString::number(tableVal[i][j]));
                } else {
                    it->setText(QString(""));
                }
                it->setFlags(it->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable);
                it->setFont(Reg);
            }
        }
    }
    ui->label_cnt->setText(QString::number(remcnt));

    ui->tableWidget->blockSignals(false);
}

void MainWindow::SwitchPane(int to)
{
    ui->tableWidget->blockSignals(true);
    ui->stackedWidget->setCurrentIndex(to);
    this->nowPane = to;
    switch (to) {
    case 0:
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                auto *it = ui->tableWidget->item(i, j);
                it->setText(QString(""));
                it->setFlags(it->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
                it->setFont(ui->tableWidget->font());
            }
        }
        ui->statusbar->showMessage("Ready.");
        break;
    case 1:
        // do nothing
        break;
    case 2:
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                auto *it = ui->tableWidget->item(i, j);
                it->setText(QString(""));
                it->setFlags(it->flags() | Qt::ItemIsEditable | Qt::ItemIsSelectable);
                it->setFont(ui->tableWidget->font());
            }
        }
        break;
    case 3:
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                auto *it = ui->tableWidget->item(i, j);
                it->setFlags(it->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
            }
        }
        ui->statusbar->showMessage("Congratulations!");
        break;
    case 4:
        // 放弃了
        break;
    }
    ui->tableWidget->blockSignals(false);
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

    remcnt += !!tableVal[a][b];

    qDebug() << a << b << "celltext" << cellText << "int" << cellVal;

    if (purified.length() == 0 || (purified.length() == 1 && purified[0] == '0')) {
        Stream << "Cleared contents in (" << a + 1 << ", " << b + 1 << ").";
        tableVal[a][b] = 0;

        ui->tableWidget->blockSignals(true);
        ui->tableWidget->item(a, b)->setText("");
        ui->tableWidget->blockSignals(false);

        ui->statusbar->showMessage(status);
    } else if (QString::number(cellVal) != purified || cellVal > 9 || cellVal < 1) {
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
        // perform duplicate check
        bool dplc = false; int di, dj;
        for (int i = 0; i < 9; i++) {
            if (i == a) continue;
            if (tableVal[i][b] == cellVal) {
                dplc = true; di = i, dj = b; break;
            }
        }
        if (!dplc) for (int j = 0; j < 9; j++) {
            if (j == b) continue;
            if (tableVal[a][j] == cellVal) {
                dplc = true; di = a, dj = j; break;
            }
        }
        if (!dplc) {
            int id = (a/3) * 3 + b/3;
            for (int i = 0; i < 9 && !dplc; i++) {
                for (int j = 0; j < 9; j++) {
                    if (i == a && j == b) continue;
                    if ((i/3)*3+j/3 == id && tableVal[i][j] == cellVal) {
                        dplc = true; di = i, dj = j; break;
                    }
                }
            }
        }
        if (!dplc) {
            if (a == b) for (int i = 0; i < 9; i++) {
                if (i == a) continue;
                if (tableVal[i][i] == cellVal) {
                    dplc = true; di = dj = i; break;
                }
            } else if (a+b==8) for (int i = 0; i < 9; i++) {
                if (i == a) continue;
                if (tableVal[i][8-i] == cellVal){
                    dplc = true; di = i, dj = 8-i; break;
                }
            }
        }
        if (dplc) {
            Stream << "Warning: duplicate number at (" << di + 1 << ", " << dj + 1 << ").";
        } else {
            Stream << "Change (" << a + 1 << ", " << b + 1 << ") to " << cellVal << ".";
        }
        --remcnt;
        tableVal[a][b] = cellVal;

        ui->tableWidget->blockSignals(true);
        ui->tableWidget->item(a, b)->setText(QString::number(tableVal[a][b]));
        ui->tableWidget->blockSignals(false);

        ui->statusbar->showMessage(status);
    }
    ui->label_cnt->setText(QString::number(remcnt));
    if (!remcnt) {
        this->on_actionValidate_Answer_triggered();
    }
}

void SudokuGridPainter::paint(QPainter *painter,
                       const QStyleOptionViewItem &option,
                       const QModelIndex &index) const {
    // draws a typical 3x3x9 Sudoku grid line
    int a = index.column(), b = index.row();
    auto &rect = option.rect;
    painter->setPen(QPen(Qt::gray, 1));
    painter->drawRect(rect);
    painter->setPen(QPen(Qt::black, 2));
    QBrush shade(Qt::gray);
    if (a == b || a+b==8) { painter->fillRect(rect, shade); }
    for (int k = 0; k < 6; k += 3) {
        if (b == 2 + k) painter->drawLine(rect.bottomLeft(), rect.bottomRight());
        if (b == 3 + k) painter->drawLine(rect.topLeft(),    rect.topRight());
        if (a == 2 + k) painter->drawLine(rect.topRight(),   rect.bottomRight());
        if (a == 3 + k) painter->drawLine(rect.topLeft(),    rect.bottomLeft());
    }
    QItemDelegate::paint(painter, option, index);
}

void MainWindow::on_actionValidate_Answer_triggered()
{
    bool valid = game.verify(tableVal);
    QString status = "Answer ";
    QMessageBox msgbox;
    msgbox.setStandardButtons(QMessageBox::Ok);
    msgbox.setDefaultButton(QMessageBox::Ok);
    if (valid) {
        status += "valid!";
        msgbox.setText("Congratulations!");
        msgbox.setInformativeText("Answer valid.");
    } else {
        status += "invalid!";
        msgbox.setText("Sorry;");
        msgbox.setInformativeText("Answer invalid. Check your input.");
    }
    ui->statusbar->showMessage(status);
    msgbox.exec();
    if (valid) { this->SwitchPane(3); }
}

void MainWindow::on_actionNew_Game_triggered()
{
    this->game.GeneratePuzzle();
    this->game.RemoveDigits(81 - ui->spinBox->value());
    this->UpdateTableContents();
    SwitchPane(1);
}

void MainWindow::on_pushButton_generate_base_clicked()
{
    this->game.GeneratePuzzle(false, 81 - ui->spinBox->value());
    this->UpdateTableContents();
    SwitchPane(1);
}

void MainWindow::on_actionNew_Game_from_user_input_triggered()
{
    // change buttons
    // change cell style -> editable
    this->SwitchPane(2);
    memset(tableVal, 0, sizeof(tableVal));
    ui->statusbar->showMessage(QString("Please input puzzle..."));
}

void MainWindow::on_pushButton_input_end_clicked()
{
    uint16_t flag[4][9];
    int cnt = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            cnt += !!tableVal[i][j];
        }
    }
    if (cnt < 17) {
        QMessageBox msgbox;
        msgbox.setText("Too few numbers");
        msgbox.setInformativeText("Please input more than 17 numbers.");
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.setDefaultButton(QMessageBox::Ok);
        msgbox.exec();
        return;
    }
    if (!game.cal_flag(tableVal, flag)) {
        QMessageBox msgbox;
        msgbox.setText("Illegal input");
        msgbox.setInformativeText("Possibly due to duplicate numbers. Check your input.");
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.setDefaultButton(QMessageBox::Ok);
        msgbox.exec();
        return;
    }
    game.SetPuzzle(&tableVal);
    this->UpdateTableContents();
    this->SwitchPane(1);
}

void MainWindow::on_pushButton_input_giveup_clicked()
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            auto *it = ui->tableWidget->item(i, j);
            it->setText(QString(""));
            it->setFlags(it->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
        }
    }
    memset(tableVal, 0, sizeof(tableVal));

    this->SwitchPane(0);
}

void MainWindow::GetHint(bool all)
{
    ui->tableWidget->setCurrentItem(nullptr);
    qDebug() << "hint in progress";
    static int soln[9][9];
    static bool calced = false;
    static int prevmod = 0;
    bool changed = false;
    int totcnt = 0;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (!tableVal[i][j]) ++totcnt;
            if (tableVal[i][j] && tableVal[i][j] != soln[i][j]) {
                changed = true;
            }
        }
    }
    if (changed || !calced || prevmod != ui->comboBox->currentIndex() ) {
        prevmod = ui->comboBox->currentIndex();
        memcpy(soln, tableVal, sizeof(soln));
        calced = SudokuGame::GetSolution(&soln, prevmod);
        if (!calced) {
            qDebug() << "no solution";
            QMessageBox msgbox;
            msgbox.setText("Hint unavailable");
            msgbox.setInformativeText("Check your input.");
            msgbox.setStandardButtons(QMessageBox::Ok);
            msgbox.setDefaultButton(QMessageBox::Ok);
            msgbox.exec();
            return;
        }
    }
    if (!totcnt) {
        QMessageBox msgbox;
        msgbox.setText("Game board full");
        msgbox.setStandardButtons(QMessageBox::Ok);
        msgbox.setDefaultButton(QMessageBox::Ok);
        msgbox.exec();
        return;
    }
    qDebug() << "hint success";
//    game.SetPuzzle(&soln);
//    this->UpdateTableContents();
    if (all) {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (!tableVal[i][j]) {
                    --remcnt;
                    tableVal[i][j] = soln[i][j];
                    ui->tableWidget->item(i, j)->setText(QString::number(tableVal[i][j]));
                }
            }
        }
    } else {
        bool f = true;
        int randcnt = arc4random() % totcnt + 1;
        for (int i = 0; i < 9 && f; i++) {
            for (int j = 0; j < 9 && f; j++) {
                if (!tableVal[i][j]) { --randcnt; }
                if (!randcnt) {
                    --remcnt;
                    tableVal[i][j] = soln[i][j];
                    ui->tableWidget->item(i, j)->setText(QString::number(tableVal[i][j]));
                    f = false;
                }
            }
        }
    }
    ui->label_cnt->setText(QString::number(remcnt));
}

void MainWindow::on_actionHint_triggered()
{
    this->GetHint(false);
}

void MainWindow::on_pushButton_printsoln_clicked()
{
    this->GetHint(true);
}

void MainWindow::on_pushButton_giveup_clicked()
{
    QMessageBox msgbox;
    msgbox.setText("Give up");
    msgbox.setInformativeText("Do you want to discard current game?");
    msgbox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgbox.setDefaultButton(QMessageBox::Ok);
    int ret = msgbox.exec();
    if (ret == QMessageBox::Cancel) return;

    // clean grid
    memset(tableVal, 0, sizeof(tableVal));

    this->SwitchPane(0);
}

void MainWindow::on_pushButton_save_clicked()
{
    bool ok;
    QString text =
        QInputDialog::getText(this,
                              "Save file", "Please input file name:",
                              QLineEdit::Normal, "saved_game", &ok);
    if (ok)
        game.Save(("../../../" + text).toStdString().c_str(), &tableVal);
    // needs to get out of the nested folders in an app bundle
}

void MainWindow::on_pushButton_readfile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        "Load sudoku", "../../../", "Sudoku data files (*.sudokudat)");
    if (!fileName.isNull() && !fileName.isEmpty()) {
        game.Read(fileName.toStdString().c_str(), &tableVal);       // TODO
        this->UpdateTableContents(false);
        this->SwitchPane(1);
    }
}

void MainWindow::on_pushButton_goback_clicked()
{
    memset(tableVal, 0, sizeof(tableVal));
    this->SwitchPane(0);
}
