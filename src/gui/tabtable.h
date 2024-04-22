#ifndef TABTABLE_H
#define TABTABLE_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QSqlTableModel>

#include <gui/databaseviewer.h>
#include <base/database.h>

namespace Ui {
class TabTable;
}

class TabTable : public QWidget
{
    Q_OBJECT

public:
    explicit TabTable(Database *database, QWidget *parent = nullptr);
    ~TabTable();
    QStandardItemModel* transposeTable(QSqlTableModel *sqlTableModel);

    const QString &tableName() const;


public slots:
    void showDatabaseViewer();
    void onTableSelected(const QString &tableName);
    void onTabChanged();

signals:
    void tableSelected(const QString &tableName);
    void forceUpdateTable();

private:
    Ui::TabTable *ui;
    Database* m_database;
    QDialog* m_dialogWindow = nullptr;
    QStandardItemModel *m_transposeTableModel = nullptr;

    DatabaseViewer *m_dbViewer;

    QString m_tableName;
};

#endif // TABTABLE_H
