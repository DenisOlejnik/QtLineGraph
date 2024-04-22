#ifndef TABBAR_H
#define TABBAR_H

#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QTabWidget>
#include <QTabBar>
#include <QWidget>

#include <base/database.h>
#include <gui/tabtable.h>
#include <gui/linegraph.h>

namespace Ui {
class TabBar;
}

class TabBar : public QWidget
{
    Q_OBJECT

public:
    explicit TabBar(Database *database, LineGraph *graph ,QWidget *parent = nullptr);
    ~TabBar();

public slots:
    void onTableSelected(const QString& tableName);
    void updateTabName(int index, const QString &newTabName);
    void addDataSeries(const QString &tableName);
    void clearTabContent(int index);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

signals:
    void tableSelected(const QString &name, const QVector<QPointF> &points);
    void tabRenamed(const QString &oldName, const QString &newName);
    void tabClosed(const QString &tabName);
    void tabChanged();

private:
    void setupTabWidget();
    void setupConnections();
    void setDisplayName(int index, const QString &tableName, const QString& newName);
    QString getTableNameAtIndex(int index);

private slots:
    void addNewTab();
    void closeTab(int index);
    void renameTab(int index);
    void endTabEditing();

private:
    Ui::TabBar *ui;
    Database *m_database;
    QTabWidget *m_tabWidget;
    QPushButton *m_addTabButton;
    QLineEdit *m_lineRenameTab = nullptr;
    LineGraph *m_lineGraph;
};

#endif // TABBAR_H
