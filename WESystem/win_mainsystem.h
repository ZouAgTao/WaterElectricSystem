#ifndef WIN_MAINSYSTEM_H
#define WIN_MAINSYSTEM_H

#pragma once

#include <QMainWindow>
#include <QDebug>
#include <QDateTime>
#include "agdb.h"
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QValueAxis>
#include <QtAlgorithms>
#include <QColor>
#include <QStandardItemModel>
#include <QStandardItem>
#include "createbarchart.h"
#include "createpiechart.h"
#include <QFileDialog>



QT_CHARTS_USE_NAMESPACE

namespace Ui {
class Win_MainSystem;
}

class Win_MainSystem : public QMainWindow
{
    Q_OBJECT

public:
    explicit Win_MainSystem(QWidget *parent = nullptr);
    ~Win_MainSystem();

private slots:
    void on_ag_btn_overview_clicked();

    void on_ag_btn_manage_clicked();

    void on_ag_btn_settings_clicked();

    void on_ag_btn_exit_clicked();

    void on_ag_btn_search_clicked();

    void on_ag_btn_showcondition_clicked();

    void on_ag_rbtn_name_clicked();

    void on_ag_rbtn_id_clicked();

    void on_ag_rbtn_water_clicked();

    void on_ag_rbtn_elec_clicked();

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_ag_btn_exit_show_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_jh_label_confirmDelete_btn_cancel_clicked();

    void on_ag_btn_remove_clicked();

    void on_jh_label_confirmDelete_btn_confirm_clicked();

    void on_ag_btn_update_clicked();

    void on_jh_label_confirmDelete_btn_confirm_2_clicked();

    void on_ag_btn_add_clicked();

    void on_ag_rbtn_elec_2_clicked();

    void on_ag_btn_exit_2_clicked();

    void on_ag_btn_change_barcharmode_clicked();

    void on_ag_btn_fileout_clicked();

    void on_ag_btn_filein_clicked();

private:
    int searchmode=0;

    Ui::Win_MainSystem *ui;
    QStandardItemModel * model;
    QChartView * show_chartview;
    QChartView * barchartview;
    QChartView * piechartview;

    QString id;
    QModelIndex iIndex;
    int changetype=-1;
    int barchartmode=1;
    int piechartmode=1;

    void InitWin();
    void ChangeTo(int index);
    void ShowOverview();
    void ShowManage();
    void ShowSettings();
    void SearchWord(QString kw);

    void showLineChart();
    void showBarChart(int type);
    void showPieChart(int type);
};

#endif // WIN_MAINSYSTEM_H
