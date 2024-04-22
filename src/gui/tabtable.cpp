#include "tabtable.h"
#include "ui_tabtable.h"

TabTable::TabTable(Database *database, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabTable),
    m_database(database)
{
    ui->setupUi(this);
    connect(ui->buttonLoad, &QPushButton::clicked, this, &TabTable::showDatabaseViewer);

}

TabTable::~TabTable()
{
    delete ui;
}

QStandardItemModel* TabTable::transposeTable(QSqlTableModel *sqlTableModel)
{
    int rowCount = sqlTableModel->rowCount();
    int columnCount = sqlTableModel->columnCount();

    QStandardItemModel *transposedModel = new QStandardItemModel(columnCount - 1, rowCount);

    QStringList headerLabels;
    for (int i = 1; i < columnCount; ++i) {
        headerLabels << sqlTableModel->headerData(i, Qt::Horizontal).toString().toUpper();
    }
    transposedModel->setVerticalHeaderLabels(headerLabels);

    for (int i = 0; i < rowCount; ++i) {
        for (int j = 1; j < columnCount; ++j) {
            QModelIndex index = sqlTableModel->index(i, j);
            QVariant data = sqlTableModel->data(index);
            transposedModel->setItem(j - 1, i, new QStandardItem(data.toString()));
        }
    }

    return transposedModel;
}

void TabTable::showDatabaseViewer()
{
    m_dbViewer = new DatabaseViewer(m_database, this);
    connect(ui->buttonLoad, &QPushButton::clicked, m_dbViewer, &DatabaseViewer::updateTable);
    connect(this, &TabTable::forceUpdateTable, m_dbViewer, &DatabaseViewer::updateTable);

    if (!m_dialogWindow) {
        m_dialogWindow = new QDialog(this);
        m_dialogWindow->setModal(true);
        m_dialogWindow->setWindowTitle("Database viewer");
        m_dialogWindow->resize(400, 300);

        QVBoxLayout* layout = new QVBoxLayout(m_dialogWindow);
        layout->addWidget(m_dbViewer);
        m_dialogWindow->setLayout(layout);

        connect(m_dbViewer, &DatabaseViewer::tableSelected, this, &TabTable::onTableSelected);
        connect(m_dbViewer, &DatabaseViewer::closeWindow, m_dialogWindow, &QDialog::close);
    }

    m_dialogWindow->exec();
}

void TabTable::onTableSelected(const QString &tableName)
{
    if (m_transposeTableModel) {
        delete m_transposeTableModel;
    }

    m_transposeTableModel = transposeTable(m_database->getTableModel(tableName));
    ui->tableView->setModel(m_transposeTableModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    m_tableName = tableName;

    emit tableSelected(tableName);
}

void TabTable::onTabChanged()
{
    emit forceUpdateTable();
}


const QString &TabTable::tableName() const
{
    return m_tableName;
}
