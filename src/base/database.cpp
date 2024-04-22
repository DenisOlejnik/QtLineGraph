#include "database.h"

Database::Database(QObject *parent)
    : QObject{parent}
{
    initDatabase();
}

void Database::initDatabase()
{
    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("./data.db");

    if(!m_database.open()) {
        qDebug() << "Error: Cant open database! Reason: " << m_database.lastError().text();
        return;
    }

    createTableNames();

    emit DatabaseReady(*this);
}

bool Database::executeQuery(const QString &query)
{
    m_database.transaction();

    QSqlQuery sqlQuery;
    if(!sqlQuery.exec(query)) {
        qCritical() << "executeQuery | Error: " << sqlQuery.lastError().text();
        qCritical() << "Query: " << query;
        m_database.rollback();
        return false;
    }

    m_database.commit();
    return true;
}

bool Database::createTableCoordinates(const QString &tableName)
{
    QSqlQuery sqlQuery;
    QString query = QString("CREATE TABLE IF NOT EXISTS %1 ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                            "x REAL, "
                            "y REAL);").arg(tableName);

    if (sqlQuery.exec(query)) {
        setTableDisplayName(tableName, tableName);
        return true;
    }

    return false;
}

bool Database::createTableNames()
{
    QSqlQuery sqlQuery;
    QString query = QString("CREATE TABLE IF NOT EXISTS %1 ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "table_name VARCHAR,"
                            "display_name VARCHAR)").arg(m_tableNames);

    if (sqlQuery.exec(query)) {
        return true;
    }

    return false;
}

bool Database::dropTable(const QString &tableName)
{
    QSqlQuery sqlQuery;
    QString query = QString("DROP TABLE IF EXISTS %1;").arg(tableName);

    if (sqlQuery.exec(query)) {
        if (deleteRow(m_tableNames, "table_name", tableName)) {
            emit tableDropped(tableName);
            return true;
        }
    }

    return false;
}

bool Database::deleteRow(const QString &tableName, const QString &fieldName, const QVariant &value)
{
    QSqlQuery sqlQuery;
    QString query = QString("DELETE FROM %1 WHERE %2 = :value;").arg(tableName, fieldName);

    sqlQuery.prepare(query);
    sqlQuery.bindValue(":value", value);

    if (sqlQuery.exec()) {
        emit rowDeleted(tableName, fieldName, value);
        return true;
    }

    return false;
}

bool Database::insertData(const QString &tableName, const QVector<QPointF> &points)
{
    m_database.transaction();

    createTableCoordinates(tableName);

    QString query = QString("INSERT INTO %1 (x, y) VALUES ").arg(tableName);
    QStringList queryValues;


    for (const QPointF &point : points) {
        queryValues.append(QString("(%1, %2)").arg(point.x()).arg(point.y()));
    }

    QString finalQuery = query + queryValues.join(", ") + ";";
    QSqlQuery sqlQuery;

    if (sqlQuery.exec(finalQuery)) {
        m_database.commit();
        return true;
    }

    m_database.rollback();
    return false;
}

bool Database::setTableDisplayName(const QString &tableName, const QString &tableDisplayName)
{
    createTableNames();

    QSqlQuery checkSqlQuery;
    QString checkQuery = QString("SELECT COUNT(*) FROM %1 WHERE table_name = :tableName").arg(m_tableNames);

    checkSqlQuery.prepare(checkQuery);
    checkSqlQuery.bindValue(":tableName", tableName);

    if (!checkSqlQuery.exec() || !checkSqlQuery.next()) {
        return false;
    }

    int rowCount = checkSqlQuery.value(0).toInt();
    if (rowCount > 0) {
        QSqlQuery updateSqlQuery;
        QString updateQuery = QString("UPDATE %1 SET display_name = :displayName WHERE table_name = :tableName;").arg(m_tableNames);
        updateSqlQuery.prepare(updateQuery);
        updateSqlQuery.bindValue(":displayName", tableDisplayName);
        updateSqlQuery.bindValue(":tableName", tableName);

        return updateSqlQuery.exec();

    } else {
        QSqlQuery insertSqlQuery;
        QString insertQuery = QString("INSERT INTO %1 (table_name, display_name) VALUES (:tableName, :displayName);").arg(m_tableNames);
        insertSqlQuery.prepare(insertQuery);
        insertSqlQuery.bindValue(":tableName", tableName);
        insertSqlQuery.bindValue(":displayName", tableDisplayName);

        return insertSqlQuery.exec();
    }
}

void Database::generateRandomData(int count, int bound)
{
    QString tableName = QString("gen_%1").arg(QDateTime::currentDateTime().toString("hh_mm_ss_z"));

    createTableCoordinates(tableName);

    QVector<QPointF> points;
    for (int i = 0; i < count; ++i) {
        qreal x = i;
        qreal y = QRandomGenerator::global()->bounded(bound);
        points.push_back(QPointF(x, y));
    }

    if (insertData(tableName, points)) {
        emit tableRandGenerated(tableName);
    }
}

QString Database::getTableDisplayName(const QString &tableName)
{
    QSqlQuery sqlQuery;
    QString query = QString("SELECT display_name FROM %1 "
                            "WHERE table_name = :tableName;").arg(m_tableNames);

    sqlQuery.prepare(query);
    sqlQuery.bindValue(":tableName", tableName);

    if (!sqlQuery.exec()) {
        return QString();
    }

    if (sqlQuery.next()) {
        return sqlQuery.value(0).toString();
    }

    return QString();
}

QStringList Database::getTableSystemNames()
{
    QStringList tableNames;
    QSqlQuery sqlQuery("SELECT name FROM sqlite_master WHERE type='table';");

    while (sqlQuery.next()) {
        QString tableName = sqlQuery.value(0).toString();
        tableNames.append(tableName);
    }

    return tableNames;
}

QVector<QPointF> Database::getData(const QString &tableName)
{
    QVector<QPointF> points;

    if (!isTableExists(tableName)) {
        qDebug() << "Table" << tableName << "does not exist!";
        return points;
    }

    QSqlQuery query(m_database);
    QString queryString = QString("SELECT x, y FROM %1").arg(tableName);
    if (!query.exec(queryString)) {
        qDebug() << "Error executing query:" << query.lastError().text();
        return points;
    }

    while (query.next()) {
        double x = query.value(0).toDouble();
        double y = query.value(1).toDouble();
        points.append(QPointF(x, y));
    }

    return points;
}

QSqlTableModel *Database::getTableModel(const QString &tableName)
{
    QSqlTableModel* model = new QSqlTableModel(nullptr, m_database);
    model->setTable(tableName);
    model->select();

    return model;
}

int Database::getRowCount(const QString &tableName)
{
    QSqlQuery sqlQuery;
    QString query = QString("SELECT COUNT(*) FROM %1").arg(tableName);

    sqlQuery.prepare(query);

    if (!sqlQuery.exec()) {
        qDebug() << "Error executing query:" << sqlQuery.lastError().text();
        return -1;
    }
    if (sqlQuery.next()) {
        return sqlQuery.value(0).toInt();
    } else {
        qDebug() << "No rows returned";
        return 0;
    }
}

bool Database::isTableExists(const QString &tableName)
{
    if (m_database.tables().contains(tableName)) {
        return true;
    }
    return false;
}
