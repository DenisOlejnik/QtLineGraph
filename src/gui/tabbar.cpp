#include "tabbar.h"
#include "ui_tabbar.h"

TabBar::TabBar(Database *database, LineGraph *graph, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TabBar),
    m_database(database),
    m_lineGraph(graph)
{
    ui->setupUi(this);
    setupTabWidget();
    setupConnections();

    addNewTab();
}

TabBar::~TabBar()
{
    delete ui;
}

void TabBar::onTableSelected(const QString& tableName)
{
    int index = m_tabWidget->currentIndex();
    QString tabName = m_tabWidget->tabText(index);
    QString tableDisplayName = m_database->getTableDisplayName(tableName);

    if (tabName == tableDisplayName) {
        return;
    }
    clearTabContent(index);
    addDataSeries(tableName);
    updateTabName(index, tableDisplayName);

    emit tabChanged();
}

void TabBar::updateTabName(int index, const QString &newTabName)
{
    QString tabTableName = getTableNameAtIndex(index);
    QString currentTabName = m_tabWidget->tabText(index);
    m_tabWidget->setTabText(index, newTabName);
    m_database->setTableDisplayName(tabTableName, newTabName);

    emit tabRenamed(currentTabName, newTabName);
}

void TabBar::addDataSeries(const QString &tableName)
{
    QString displayName = m_database->getTableDisplayName(tableName);
    QVector<QPointF> points = m_database->getData(tableName);
    m_lineGraph->addSeries(displayName, points);
}

void TabBar::clearTabContent(int index)
{
    QString tabDisplayName = m_tabWidget->tabText(index);

    if(!tabDisplayName.isEmpty()) {
        m_lineGraph->removeSeries(tabDisplayName);
    }
}

QString TabBar::getTableNameAtIndex(int index)
{
    if (index < 0 || index >= m_tabWidget->count()) {
        return QString();
    }

    TabTable *tabTable = qobject_cast<TabTable *>(m_tabWidget->widget(index));
    if(tabTable) {
        return tabTable->tableName();
    }
    return QString();
}

bool TabBar::eventFilter(QObject *watched, QEvent *event) {
    if (watched == m_tabWidget->tabBar() && event->type() == QEvent::MouseButtonDblClick) {
        auto mouseEvent = static_cast<QMouseEvent*>(event);
        int index = m_tabWidget->tabBar()->tabAt(mouseEvent->pos());
        if (index != -1) {
            renameTab(index);
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonPress) {
        if (watched != m_lineRenameTab) {
            endTabEditing();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void TabBar::focusOutEvent(QFocusEvent *event)
{
    endTabEditing();
    QWidget::focusOutEvent(event);
}

void TabBar::setupTabWidget()
{
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->installEventFilter(this);
    m_tabWidget->tabBar()->installEventFilter(this);

    m_addTabButton = new QPushButton("+", this);
    m_tabWidget->setCornerWidget(m_addTabButton);

    ui->vBoxMainLayout->layout()->addWidget(m_tabWidget);

}

void TabBar::setupConnections()
{
    connect(m_addTabButton, &QPushButton::clicked, this, &TabBar::addNewTab);
    connect(m_tabWidget, &QTabWidget::tabCloseRequested, this, &TabBar::closeTab);
    connect(this, &TabBar::tabRenamed, m_lineGraph, &LineGraph::renameLegend);
}

void TabBar::addNewTab() {
    TabTable *newTab = new TabTable(m_database, this);

    connect(newTab, &TabTable::tableSelected, this, &TabBar::onTableSelected);
    connect(this, &TabBar::tabChanged, newTab, &TabTable::onTabChanged);

    int index = m_tabWidget->addTab(newTab, QString("Tab %1").arg(m_tabWidget->count()+1));
    m_tabWidget->setCurrentIndex(index);

    if (m_tabWidget->count() > 1) {
        m_tabWidget->setTabsClosable(true);
    }
}

void TabBar::closeTab(int index)
{
    if (m_lineRenameTab) {
        endTabEditing();
    }

    m_lineGraph->removeSeries(m_tabWidget->tabText(index));

    emit tabClosed(m_tabWidget->tabText(index));

    QWidget *widget = m_tabWidget->widget(index);
    widget->deleteLater();

    m_tabWidget->removeTab(index);

    if (m_tabWidget->count() < 2) {
        m_tabWidget->setTabsClosable(false);
        return;
    }
}

void TabBar::renameTab(int index)
{
    if (m_lineRenameTab) {
        endTabEditing();
    }

    m_lineRenameTab = new QLineEdit(m_tabWidget);
    QRect rect = m_tabWidget->tabBar()->tabRect(index);
    m_lineRenameTab->setGeometry(rect);
    m_lineRenameTab->setText(m_tabWidget->tabText(index));
    m_lineRenameTab->setFocus();
    m_lineRenameTab->selectAll();
    m_lineRenameTab->show();

    connect(m_lineRenameTab, &QLineEdit::editingFinished, this, [this, index]() {
        QString newTabName = m_lineRenameTab->text();
        updateTabName(index, newTabName);

        endTabEditing();
        emit tabChanged();
    });
}

void TabBar::endTabEditing()
{
    if (m_lineRenameTab) {
        m_lineRenameTab->deleteLater();
        m_lineRenameTab = nullptr;
    }
}
