#include "linegraph.h"
#include "ui_linegraph.h"

LineGraph::LineGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LineGraph),
    m_chart(new QChart)
{
    ui->setupUi(this);

    QChartView* chartView = new QChartView(m_chart);
    chartView->setParent(this);
    m_chart->legend()->setVisible(true);
    m_chart->legend()->setAlignment(Qt::AlignRight);

    ui->vBoxMainLayout->layout()->addWidget(chartView);

}

LineGraph::~LineGraph()
{
    delete ui;
}

void LineGraph::addSeries(const QString &name, const QVector<QPointF> &data)
{
    QLineSeries *series = new QLineSeries(this);
    series->setName(name);

    for (const QPointF &point: data) {
        series->append(point);
    }

    m_chart->addSeries(series);
    m_chart->createDefaultAxes();
}

void LineGraph::renameLegend(const QString &oldName, const QString &newName)
{
    QList<QAbstractSeries *> seriesList = m_chart->series();
    for (QAbstractSeries *series : seriesList) {
        if (series->name() == oldName) {
            series->setName(newName);
            break;
        }
    }
}

void LineGraph::removeSeries(const QString &name)
{
    QList<QAbstractSeries *> seriesList = m_chart->series();
    for (QAbstractSeries *series : seriesList) {
        if (series->name() == name) {
            m_chart->removeSeries(series);
            delete series;
            break;
        }
    }
}

bool LineGraph::hasSeries(const QString &name)
{
    QList<QAbstractSeries *> seriesList = m_chart->series();
    for (QAbstractSeries *series : seriesList) {
        if (series->name() == name) {
            return true;
        }
    }
    return false;
}

