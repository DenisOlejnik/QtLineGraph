#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <base/database.h>
#include <gui/linegraph.h>
#include <gui/tabbar.h>
#include <gui/tabtable.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void init();

private:
    Ui::MainWindow *ui;
    Database* m_database;
    LineGraph* m_linegraph;
    TabBar* m_tabBar;
};
#endif // MAINWINDOW_H
