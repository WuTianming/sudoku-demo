#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sudokugame.h"

#include <QMainWindow>
#include <QtDebug>
#include <QTextStream>
#include <QItemDelegate>
#include <QPainter>
#include <QFont>

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
    void UpdateTableContents();
    // from userinput to tableWidget to game;
    // will call UpdateTableContents after input.
    void InputTableContents();

    void SwitchPane(int to);

private slots:
    // callback
    void on_tableWidget_cellChanged(int, int);

    // File menu
    void on_actionNew_Game_triggered();
    void on_actionNew_Game_from_user_input_triggered();

    void on_actionValidate_Answer_triggered();
    void on_actionHint_triggered();

    void on_actionExit_triggered();
    void on_actionClose_triggered();

private:
    // a `qStackedWidget' manages buttons in different modes.
    // different panes indicate different modes:
    //   pane #0: game not initiated
    //   pane #1: game in progress
    //   pane #2: user inputting custom puzzle
    //   pane #3:
    int nowPane;

    Ui::MainWindow *ui;
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
