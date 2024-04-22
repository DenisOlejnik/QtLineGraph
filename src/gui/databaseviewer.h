#ifndef DATABASEVIEWER_H
#define DATABASEVIEWER_H

#include <QWidget>
#include <QStandardItemModel>
#include <base/database.h>

namespace Ui {
class DatabaseViewer;
}

class DatabaseViewer : public QWidget
{
    Q_OBJECT

public:
    explicit DatabaseViewer(Database *database, QWidget *parent = nullptr);
    ~DatabaseViewer();

public slots:
    void updateTable();
    void createTable();
    void selectTable();
    void removeTable();

signals:
    void tableSelected(const QString &tableName);
    void closeWindow();

private:
    void setupConnections();
    QString tableNameAtIndex(const QModelIndex &index);
    QStandardItemModel* createTableItemModel();

private:
    Ui::DatabaseViewer *ui;
    Database *m_database;
};

#endif // DATABASEVIEWER_H
