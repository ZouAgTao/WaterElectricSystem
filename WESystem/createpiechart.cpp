#include "createpiechart.h"

bool zx2_db_connect()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL3");
    db.setHostName(ZX_IP);
    db.setDatabaseName(ZX_DatabaseName);
    db.setUserName(ZX_UserName);
    db.setPassword(ZX_Password);
    bool ok = db.open();
    return ok;
}

int zx2_query_tiered_use(QString type, QString year, QString month, QString min, QString max, bool ok){

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

QChartView* zx_buildPieChart(QString type, int year_input, int month_input , QtCharts::QChartView* parent)
{
    bool ok = zx2_db_connect();
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

    int under = zx2_query_tiered_use(type, year, month, "0", QString::number(gap*2), ok);
    int above = zx2_query_tiered_use(type, year, month, QString::number(gap*2), QString::number(gap*5), ok);

    QPieSeries *series = new QPieSeries();
    series->append(QString::number((int)(under*1.0/(under*1.0+above*1.0)*100))+"%", under);
    series->append(QString::number((int)(above*1.0/(under*1.0+above*1.0)*100))+"%", above);

    QPieSlice *slice1 = series->slices().at(0);
    QPieSlice *slice2 = series->slices().at(1);
    slice1->setExploded(false);
    slice1->setLabelVisible();
    slice2->setExploded(false);
    slice2->setLabelVisible();

    if(type=="Water")
    {
        QColor c1;
        c1.setRgb(17,38,79);
        QColor c2;
        c2.setRgb(51,163,220);

        slice1->setColor(c1);
        slice2->setColor(c2);
    }
    else
    {
        QColor c1;
        c1.setRgb(175,19,24);
        QColor c2;
        c2.setRgb(255,36,44);

        slice1->setColor(c1);
        slice2->setColor(c2);
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("");
    chart->legend()->hide();
    chart->setAnimationOptions(QChart::AllAnimations);

    QChartView *chartView = new QChartView(parent);
    chartView->setChart(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    return chartView;
}
