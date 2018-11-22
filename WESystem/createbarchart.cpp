#include "createbarchart.h"

bool zx_db_connect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
    db.setHostName(ZX_IP);
    db.setDatabaseName(ZX_DatabaseName);
    db.setUserName(ZX_UserName);
    db.setPassword(ZX_Password);
    bool ok = db.open();
    return ok;
}

int zx_query_tiered_use(QString type, QString year, QString month, QString min, QString max, bool ok){

    if(ok){
//        qDebug()<<"Database connection established.";
        QSqlQuery query;
        QString qstr = "SELECT COUNT(*) FROM stat_finance WHERE now_date = '"+ year +"-"+ month +"-01' AND use_value_" + type + " >= "+ min +" AND use_value_" + type + " < " + max;
        query.exec(qstr);
        query.first();
        int count = query.value(0).toInt();
//        qDebug()<<count;
//        qDebug()<<query.lastQuery();
        return count;

    }
    else{
//        qDebug()<<"Database connection failed";
        return -1;
    }
}

QChartView* zx_buildBarChart(QString type, int year_input, int month_input , QtCharts::QChartView* parent)
{
    QString year = QString::number(year_input);
    QString month = QString::number(month_input);
    int gap = 0;
    if(type == "Elec")
        gap = 100;
    else if(type == "Water")
        gap = 10;
    else{
        qDebug()<<"Type Error.";
        return nullptr;
    }

    bool ok = zx_db_connect();
    int value0 = zx_query_tiered_use(type, year, month, "0", QString::number(gap*2), ok);
    int value1 = zx_query_tiered_use(type, year, month, QString::number(gap*2), QString::number(gap*3), ok);
    int value2 = zx_query_tiered_use(type, year, month, QString::number(gap*3), QString::number(gap*4), ok);
    int value3 = zx_query_tiered_use(type, year, month, QString::number(gap*4), QString::number(gap*5), ok);

    QString x0 = "0-" + QString::number(gap*2);
    QString x1 = QString::number(gap*2) + "-" + QString::number(gap*3);
    QString x2 = QString::number(gap*3) + "-" + QString::number(gap*4);
    QString x3 = QString::number(gap*4) + "-" + QString::number(gap*5);

    QBarSet *set0 = new QBarSet("set0");

    *set0 << value0 << value1 << value2 << value3;

    QHorizontalBarSeries *series = new QHorizontalBarSeries();


    if(type=="Water")
    {
        QColor bl;
        bl.setRgb(92,172,238);
        set0->setColor(bl);
    }
    else
    {
        QColor rl;
        rl.setRgb(220,20,60);
        set0->setColor(rl);
    }

    series->append(set0);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << x0 << x1 << x2 << x3;
    QBarCategoryAxis *axisY = new QBarCategoryAxis();
    axisY->append(categories);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QValueAxis *axisX = new QValueAxis();
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    axisX->applyNiceNumbers();

    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QChartView *chartView = new QChartView(parent);
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}
