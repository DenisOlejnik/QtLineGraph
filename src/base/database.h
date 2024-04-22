#ifndef DATABASE_H
#define DATABASE_H


#include <QDateTime>
#include <QRandomGenerator>
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>
#include <QPointF>
#include <QVector>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    void initDatabase();
    bool executeQuery(const QString &query);

    bool createTableCoordinates(const QString &tableName);
    bool createTableNames();

    bool dropTable(const QString &tableName);
    bool deleteRow(const QString &tableName, const QString &fieldName, const QVariant &value);

    bool insertData(const QString &tableName, const QVector<QPointF> &points);
    bool setTableDisplayName(const QString &tableName, const QString &tableDisplayName);

    QString getTableDisplayName(const QString &tableName);
    QStringList getTableSystemNames();
    QSqlTableModel* getTableModel(const QString &tableName);
    QVector<QPointF> getData(const QString &tableName);
    int getRowCount(const QString &tableName);

    bool isTableExists(const QString &tableName);

public slots:
    void generateRandomData(int count = 25, int bound = 100);



signals:
    void DatabaseReady(const Database &database);
    void tableDropped(const QString& tableName);
    void rowDeleted(const QString &tableName, const QString &fieldName, const QVariant &value);
    void tableDisplayNameSet(const QString &tableName, const QString &tableDisplayName);
    void tableRandGenerated(const QString &tableName);

private:
    QSqlDatabase m_database;
    const QString m_tableNames = "tableNames";
};

#endif // DATABASE_H
