#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete m_database;
    delete m_linegraph;
    delete m_tabBar;
    delete ui;
}

void MainWindow::init()
{
    m_database = new Database(this);
    m_linegraph = new LineGraph(this);
    m_tabBar = new TabBar(m_database, m_linegraph, this);

    connect(m_tabBar, &TabBar::tableSelected, m_linegraph, &LineGraph::addSeries);

    ui->centralwidget->layout()->addWidget(m_linegraph);
    ui->centralwidget->layout()->addWidget(m_tabBar);
    ui->verticalLayout->setStretch(0, 3);
    ui->verticalLayout->setStretch(1, 1);
}

