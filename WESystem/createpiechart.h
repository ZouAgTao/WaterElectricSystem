#ifndef CREATEPIECHART_H
#define CREATEPIECHART_H

#include <QObject>
#include <QtCharts>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QString>

#define ZX_IP       "agcore.club"
#define ZX_UserName    "root"
#define ZX_Password    "2104898"
#define ZX_DatabaseName  "test"

bool zx2_db_connect();
int zx2_query_tiered_use(QString type, QString year, QString month, QString min, QString max, bool ok);

QChartView* zx_buildPieChart(QString type, int year_input, int month_input , QtCharts::QChartView* parent);

#endif // CREATEPIECHART_H
