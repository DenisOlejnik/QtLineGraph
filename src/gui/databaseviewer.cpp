#include "databaseviewer.h"
#include "ui_databaseviewer.h"

DatabaseViewer::DatabaseViewer(Database *database, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseViewer),
    m_database(database)
{
    ui->setupUi(this);
    setupConnections();
    updateTable();
}

DatabaseViewer::~DatabaseViewer()
{
    delete ui;
}

void DatabaseViewer::updateTable()
{
    ui->tableView->setModel(createTableItemModel());

    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void DatabaseViewer::createTable()
{
    m_database->generateRandomData();
    updateTable();
}

void DatabaseViewer::selectTable()
{
    QModelIndex selectedIndex = ui->tableView->currentIndex();
    if(selectedIndex.isValid()) {
        QString tableName = tableNameAtIndex(selectedIndex);

        emit tableSelected(tableName);
        emit closeWindow();
    }
    else {
        qWarning() << "Nothing selected!";
    }
}

void DatabaseViewer::removeTable()
{
    QModelIndex selectedIndex = ui->tableView->currentIndex();
    if (selectedIndex.isValid()) {
        QString tableName = tableNameAtIndex(selectedIndex);
        if(m_database->isTableExists(tableName)) {
            if (m_database->dropTable(tableName)) {
                updateTable();
                return;
            }
        }
    }

    updateTable();
}

void DatabaseViewer::setupConnections()
{
    connect(ui->buttonSelect, &QPushButton::clicked, this, &DatabaseViewer::selectTable);
    connect(ui->buttonGenerate, &QPushButton::clicked, this, &DatabaseViewer::createTable);
    connect(ui->buttonRemove, &QPushButton::clicked, this, &DatabaseViewer::removeTable);
}

QString DatabaseViewer::tableNameAtIndex(const QModelIndex &index)
{
    if (index.isValid()) {
        return ui->tableView->model()->data(index.sibling(index.row(), 0)).toString();
    }
    return "";
}

QStandardItemModel *DatabaseViewer::createTableItemModel()
{
    QStringList tables = m_database->getTableSystemNames();

    QStandardItemModel *model = qobject_cast<QStandardItemModel*>(ui->tableView->model());
    if(!model) {
        model = new QStandardItemModel(this);
        ui->tableView->setModel(model);
    }
    model->clear();

    model->setHorizontalHeaderItem(0, new QStandardItem("Table"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Display name"));
    model->setHorizontalHeaderItem(2, new QStandardItem("Rows"));

    for(int i = 0; i < tables.count(); ++i) {
        QString tableName = tables.at(i);
        QString tableDisplayName = m_database->getTableDisplayName(tableName);
        int rowCount = m_database->getRowCount(tableName);

        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(tableName)
                 << new QStandardItem(tableDisplayName)
                 << new QStandardItem(QString::number(rowCount));

        model->appendRow(rowItems);
    }
    return model;
}
