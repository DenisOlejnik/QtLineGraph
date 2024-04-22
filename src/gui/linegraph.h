#ifndef LINEGRAPH_H
#define LINEGRAPH_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QPointF>
#include <QVector>

namespace Ui {
class LineGraph;
}

class LineGraph : public QWidget
{
    Q_OBJECT

public:
    explicit LineGraph(QWidget *parent = nullptr);
    ~LineGraph();

public slots:
    void addSeries(const QString &name, const QVector<QPointF> &data);
    void renameLegend(const QString &oldName, const QString &newName);
    void removeSeries(const QString &Name);
    bool hasSeries(const QString &name);

private:
    Ui::LineGraph *ui;
    QChart* m_chart;
};

#endif // LINEGRAPH_H
