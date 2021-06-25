#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sudokugame.h"

#include <QMainWindow>
#include <QtDebug>
#include <QTextStream>
#include <QItemDelegate>
#include <QPainter>
#include <QFont>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // from `game' to tableVal to tableWidget;
    // sets cell attributes as well.
    void UpdateTableContents(bool replace = true);
    void SwitchPane(int to);
    void GetHint(bool all);

private slots:
    // callback
    void on_tableWidget_cellChanged(int, int);

    // File menu
    void on_actionNew_Game_triggered();
    // from userinput to tableWidget to game;
    // will call UpdateTableContents after input.
    void on_actionNew_Game_from_user_input_triggered();
    void on_pushButton_input_end_clicked();
    void on_pushButton_input_giveup_clicked();
    void on_pushButton_generate_base_clicked();
    void on_pushButton_readfile_clicked();

    void on_actionValidate_Answer_triggered();
    void on_pushButton_save_clicked();
    void on_actionHint_triggered();
    void on_pushButton_printsoln_clicked();
    void on_pushButton_giveup_clicked();

    void on_actionExit_triggered();
    void on_actionClose_triggered();

    void on_pushButton_goback_clicked();

private:
    // a `qStackedWidget' manages buttons in different modes.
    // different panes indicate different modes:
    //   pane #0: game not initiated
    //   pane #1: game in progress
    //   pane #2: user inputting custom puzzle
    //   pane #3: game ended (successfully)
    //   pane #4: display multiple solutions -- 放弃了
    int nowPane;

    Ui::MainWindow *ui;
    int remcnt;
    int tableVal[9][9];     // tableVal is synchronized with the tableWidget for easy access to shown contents
    SudokuGame game;
};

class SudokuGridPainter : public QItemDelegate
{
public:
    SudokuGridPainter(QObject *parent) : QItemDelegate(parent) {}
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // MAINWINDOW_H
