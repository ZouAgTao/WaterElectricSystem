#include "win_mainsystem.h"
#include "ui_win_mainsystem.h"

Win_MainSystem::Win_MainSystem(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Win_MainSystem)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    InitWin();
}

Win_MainSystem::~Win_MainSystem()
{
    delete ui;
}

//展示三个界面
void Win_MainSystem::ShowOverview()
{
    showLineChart();
//    qDebug()<<"!!!!!!!";
    showBarChart(0);
    showPieChart(0);
}

void Win_MainSystem::ShowManage()
{
    searchmode=0;
    ui->ag_search_bg->setVisible(false);
    ui->ag_rbtn_name->setChecked(true);
    ui->jh_widget_dialog_personInfo->setVisible(false);
    SearchWord(ui->ag_edt_search->text());
}

void Win_MainSystem::SearchWord(QString kw)
{
    kw = kw.trimmed();

    if(searchmode==4)
    {
        agQueryByUnPaid(model);
    }

    if(kw=="")
    {
        agQueryAllUser(model);
    }
    else
    {
        if(searchmode==0)
        {
            agQueryByName(model,kw);
        }
        else if(searchmode==1)
        {
            agQueryByID(model,kw);
        }
        else if(searchmode==2)
        {
            agQueryByWater(model,kw);
        }
        else if(searchmode==3)
        {
            agQueryByElec(model,kw);
        }
    }
}

void Win_MainSystem::ShowSettings()
{
}
//展示三个界面

void Win_MainSystem::showLineChart()
{
    QDateTime dt = QDateTime::currentDateTime();
    int now_year = dt.toString("yyyy").toInt();
    ui->ag_title_overview_linechart->setText(dt.toString("yyyy")+" year usage statistics");
    ui->ag_title_overview_zhuchart->setText(dt.toString("yyyy")+" year usage gradient situation");
    ui->ag_title_overview_bingchart->setText(dt.toString("yyyy")+" year payment situation");

    double elec[12] = {0};
    double water[12] = {0};
    double sumelec = 0;
    double sumwater = 0;
    double summoney = 0;

    if(!agDBConnect())
    {
        qDebug()<<"MySQL Connect Fail";
        return;
    }
    agQueryElecAndWater(now_year,elec,water);

    for(int i=0;i<12;i++)
    {
        sumelec+=elec[i];
        sumwater+=water[i];
    }

    ui->ag_chart_linechart->chart()->removeAllSeries();

    QLineSeries *series = new QLineSeries();
    series->setUseOpenGL(true);
    series->setName("用电情况");
    QColor rl;
    rl.setRgb(220,20,60);
    series->setPen(QPen(rl,5,Qt::SolidLine)); //设置画笔
    for(int i=0;i<12;i++)
    {
        series->append(i+1,elec[i]);
    }

    QLineSeries *series1 = new QLineSeries();
    series1->setUseOpenGL(true);
    series1->setName("用电情况");
    QColor bl;
    bl.setRgb(92,172,238);
    series1->setPen(QPen(bl,5,Qt::SolidLine)); //设置画笔
    for(int i=0;i<12;i++)
    {
        series1->append(i+1,water[i]);
    }

    QChart *chart = new QChart();
//    chart->setAnimationOptions(QChart::AllAnimations);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->addSeries(series);
    chart->addSeries(series1);
    chart->createDefaultAxes();

    int elec_min=elec[0];
    int elec_max=elec[0];
    int water_min=water[0];
    int water_max=water[0];

    for(int i=1;i<12;i++)
    {
        elec_max=elec_max>elec[i]?elec_max:elec[i];
        elec_min=elec_min<elec[i]?elec_min:elec[i];
        water_max=water_max>water[i]?water_max:water[i];
        water_min=water_min<water[i]?water_min:water[i];
    }

    QValueAxis *axisE = new QValueAxis;
    axisE->setRange(elec_min,elec_max);    //设置范围
    axisE->setLabelFormat("%u");   //设置刻度的格式
    axisE->setGridLineVisible(true);   //网格线可见
    chart->setAxisY(axisE, series);       //为chart设置X轴,并把轴附加到series线上

    QValueAxis *axisW = new QValueAxis;
    axisW->setRange(water_min, water_max);    //设置范围
    axisW->setLabelFormat("%u");   //设置刻度的格式
    axisW->setGridLineVisible(true);   //网格线可见
    chart->setAxisY(axisW, series1);       //为chart设置X轴,并把轴附加到series线上

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(1, 12);    //设置范围
    axisX->setLabelFormat("%d");   //设置刻度的格式
    axisX->setGridLineVisible(true);   //网格线可见
    chart->setAxisX(axisX, series);       //为chart设置X轴,并把轴附加到series线上
    chart->setAxisX(axisX, series1);       //为chart设置X轴,并把轴附加到series线上

    ui->ag_chart_linechart->setChart(chart);
    ui->ag_chart_linechart->setRenderHint(QPainter::Antialiasing);
    ui->ag_chart_linechart->setRubberBand(QChartView::HorizontalRubberBand);

    summoney= sumelec*0.6 + sumwater*2.5;

    ui->ag_lab_1->setText(QString("%L1").arg(sumwater, 0 ,'f',0)+" t");
    ui->ag_lab_2->setText(QString("%L1").arg(sumelec, 0 ,'f',0)+" kwh");
    ui->ag_lab_3->setText(QString("%L1").arg(summoney, 0 ,'f',0)+" Y");

    int elec_payment[3]={0};
    int water_payment[3]={0};
    agQueryPayment(elec_payment,water_payment);
    int haspay = elec_payment[1]+water_payment[1];
    int unpay = elec_payment[0]+water_payment[0];
    int shutdown = elec_payment[2]+water_payment[2];

    float pay_persent = (float)haspay/(haspay+unpay+shutdown)*100;
    ui->ag_lab_4->setText(QString::number(pay_persent,'f',2)+" %");
}

void Win_MainSystem::showBarChart(int type)
{
    //Water
    //Elec

    QDateTime dt = QDateTime::currentDateTime();
    int now_year = dt.toString("yyyy").toInt();
    int now_month = dt.toString("MM").toInt();

    if(type==0)
    {
        barchartview = zx_buildBarChart("Water",now_year,now_month,ui->ag_chart_zhuchart);
    }
    else
    {
        barchartview = zx_buildBarChart("Elec",now_year,now_month,ui->ag_chart_zhuchart);
    }

    barchartview->resize(380, 230);
    barchartview->show();
}

void Win_MainSystem::showPieChart(int type)
{
    //Water
    //Elec

    QDateTime dt = QDateTime::currentDateTime();
    int now_year = dt.toString("yyyy").toInt();
    int now_month = dt.toString("MM").toInt();

    if(type==0)
    {
        piechartview = zx_buildPieChart("Water",now_year,now_month,ui->ag_chart_bingchart);
    }
    else
    {
        piechartview = zx_buildPieChart("Elec",now_year,now_month,ui->ag_chart_bingchart);
    }

    qDebug()<<"this";

    piechartview->resize(260, 230);
    piechartview->show();
}

void Win_MainSystem::InitWin()
{
    ui->ag_tab_overview->setVisible(true);
    ui->ag_tab_manage->setVisible(false);
    ui->ag_tab_settings->setVisible(false);
    ShowOverview();

    model = new QStandardItemModel(ui->tableView);
    model->clear();
    model->setColumnCount(6);
    model->setHeaderData(1,Qt::Horizontal,tr("ID"));
    model->setHeaderData(0,Qt::Horizontal,tr("类别"));
    model->setHeaderData(2,Qt::Horizontal,tr("姓名"));
    model->setHeaderData(3,Qt::Horizontal,tr("性别"));
    model->setHeaderData(4,Qt::Horizontal,tr("年龄"));
    model->setHeaderData(5,Qt::Horizontal,tr("班级/部门"));

    ui->tableView->setModel(model);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->setAlternatingRowColors(true);
    ui->tableView->resizeColumnsToContents();
    ui->jh_widget_dialog_confirmDelete->setVisible(false);
    ui->jh_widget_dialog_addorUpdatePeople->setVisible(false);

    show_chartview = new QChartView(ui->jh_graphicsView_Dialog_chart_personInfo);
}

void Win_MainSystem::ChangeTo(int index)
{
    switch (index)
    {
    case 0:
        ui->ag_tab_overview->setVisible(true);
        ui->ag_tab_manage->setVisible(false);
        ui->ag_tab_settings->setVisible(false);
        ShowOverview();
        break;
    case 1:
        ui->ag_tab_overview->setVisible(false);
        ui->ag_tab_manage->setVisible(true);
        ui->ag_tab_settings->setVisible(false);
        ShowManage();
        break;
    case 2:
        ui->ag_tab_overview->setVisible(false);
        ui->ag_tab_manage->setVisible(false);
        ui->ag_tab_settings->setVisible(true);
        ShowSettings();
        break;
    }
}

void Win_MainSystem::on_ag_btn_overview_clicked()
{
    ChangeTo(0);
}

void Win_MainSystem::on_ag_btn_manage_clicked()
{
    ChangeTo(1);
}

void Win_MainSystem::on_ag_btn_settings_clicked()
{
    ChangeTo(2);
}

void Win_MainSystem::on_ag_btn_exit_clicked()
{
    exit(0);
}

void Win_MainSystem::on_ag_btn_search_clicked()
{
    model->clear();
    model->setColumnCount(6);
    model->setHeaderData(1,Qt::Horizontal,tr("ID"));
    model->setHeaderData(0,Qt::Horizontal,tr("类别"));
    model->setHeaderData(2,Qt::Horizontal,tr("姓名"));
    model->setHeaderData(3,Qt::Horizontal,tr("性别"));
    model->setHeaderData(4,Qt::Horizontal,tr("年龄"));
    model->setHeaderData(5,Qt::Horizontal,tr("班级/部门"));

    SearchWord(ui->ag_edt_search->text());
}

void Win_MainSystem::on_ag_btn_showcondition_clicked()
{
    ui->ag_search_bg->setVisible(true);
}

void Win_MainSystem::on_ag_rbtn_name_clicked()
{
    searchmode=0;
    ui->ag_search_bg->setVisible(false);
}

void Win_MainSystem::on_ag_rbtn_id_clicked()
{
    searchmode=1;
    ui->ag_search_bg->setVisible(false);
}

void Win_MainSystem::on_ag_rbtn_water_clicked()
{
    searchmode=2;
    ui->ag_search_bg->setVisible(false);
}

void Win_MainSystem::on_ag_rbtn_elec_clicked()
{
    searchmode=3;
    ui->ag_search_bg->setVisible(false);
}

void Win_MainSystem::on_tableView_doubleClicked(const QModelIndex &index)
{
    QDateTime dt = QDateTime::currentDateTime();
    int now_year = dt.toString("yyyy").toInt();

    ui->jh_widget_dialog_personInfo->setVisible(true);
    show_chartview = new QChartView(ui->jh_graphicsView_Dialog_chart_personInfo);
    QString id = (model->index(index.row(),1,QModelIndex())).data().toString();

    double elec[12] = {0};
    double water[12] = {0};
    double sumelec = 0;
    double sumwater = 0;
    double summoney = 0;

    if(!agDBConnect())
    {
        qDebug()<<"MySQL Connect Fail";
        return;
    }
    agQueryElecAndWaterByID(now_year,elec,water,id);

    for(int i=0;i<12;i++)
    {
        sumelec+=elec[i];
        sumwater+=water[i];
    }

    show_chartview->chart()->removeAllSeries();

    QLineSeries *series2 = new QLineSeries();
    series2->setUseOpenGL(true);
    series2->setName("用电情况");
    QColor rl;
    rl.setRgb(220,20,60);
    series2->setPen(QPen(rl,2,Qt::SolidLine)); //设置画笔
    for(int i=0;i<12;i++)
    {
        series2->append(i+1,elec[i]);
    }

    QLineSeries *series3 = new QLineSeries();
    series3->setUseOpenGL(true);
    series3->setName("用电情况");
    QColor bl;
    bl.setRgb(92,172,238);
    series3->setPen(QPen(bl,2,Qt::SolidLine)); //设置画笔
    for(int i=0;i<12;i++)
    {
        series3->append(i+1,water[i]);
    }

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series2);
    chart->addSeries(series3);
    chart->createDefaultAxes();

    int elec_min=elec[0];
    int elec_max=elec[0];
    int water_min=water[0];
    int water_max=water[0];

    for(int i=1;i<12;i++)
    {
        elec_max=elec_max>elec[i]?elec_max:elec[i];
        elec_min=elec_min<elec[i]?elec_min:elec[i];
        water_max=water_max>water[i]?water_max:water[i];
        water_min=water_min<water[i]?water_min:water[i];
    }

    QValueAxis *axisE = new QValueAxis;
    axisE->setRange(elec_min,elec_max);    //设置范围
    axisE->setLabelFormat("%u");   //设置刻度的格式
    axisE->setGridLineVisible(true);   //网格线可见
    chart->setAxisY(axisE, series2);       //为chart设置X轴,并把轴附加到series线上

    QValueAxis *axisW = new QValueAxis;
    axisW->setRange(water_min, water_max);    //设置范围
    axisW->setLabelFormat("%u");   //设置刻度的格式
    axisW->setGridLineVisible(true);   //网格线可见
    chart->setAxisY(axisW, series3);       //为chart设置X轴,并把轴附加到series线上

    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(1, 12);    //设置范围
    axisX->setLabelFormat("%d");   //设置刻度的格式
    axisX->setGridLineVisible(true);   //网格线可见
    chart->setAxisX(axisX, series2);       //为chart设置X轴,并把轴附加到series线上
    chart->setAxisX(axisX, series3);       //为chart设置X轴,并把轴附加到series线上

    show_chartview->setChart(chart);
    show_chartview->setRenderHint(QPainter::Antialiasing);
    show_chartview->setRubberBand(QChartView::HorizontalRubberBand);
    show_chartview->resize(380, 180);
    show_chartview->show();
//    ui->jh_graphicsView_Dialog_chart_personInfo->setChart(new QChart);
    summoney= sumelec*0.6 + sumwater*2.5;

    ui->ag_show_sum_water->setText(QString::number((int)sumwater)+" t");
    ui->ag_show_sum_elec->setText(QString::number((int)sumelec)+" kwh");
    ui->ag_show_sum_water_money->setText(QString::number((int)sumelec*0.6)+" Y");
    ui->ag_show_sum_elec_money->setText(QString::number((int)sumwater*2.5)+" Y");
    ui->ag_show_sum_money->setText(QString::number((int)summoney)+" Y");
}

void Win_MainSystem::on_ag_btn_exit_show_clicked()
{
    delete show_chartview;
    ui->jh_widget_dialog_personInfo->setVisible(false);
}

void Win_MainSystem::on_tableView_clicked(const QModelIndex &index)
{
    id = (model->index(index.row(),1,QModelIndex())).data().toString();
    iIndex = index;
}

void Win_MainSystem::on_jh_label_confirmDelete_btn_cancel_clicked()
{
    ui->jh_widget_dialog_confirmDelete->setVisible(false);
}

void Win_MainSystem::on_ag_btn_remove_clicked()
{
    if(id=="")
    {
        return;
    }
    ui->jh_widget_dialog_confirmDelete->setVisible(true);
}


void Win_MainSystem::on_jh_label_confirmDelete_btn_confirm_clicked()
{
    agDDeleteByID(id);
    id="";

    model->clear();
    model->setColumnCount(6);
    model->setHeaderData(1,Qt::Horizontal,tr("ID"));
    model->setHeaderData(0,Qt::Horizontal,tr("类别"));
    model->setHeaderData(2,Qt::Horizontal,tr("姓名"));
    model->setHeaderData(3,Qt::Horizontal,tr("性别"));
    model->setHeaderData(4,Qt::Horizontal,tr("年龄"));
    model->setHeaderData(5,Qt::Horizontal,tr("班级/部门"));

    SearchWord(ui->ag_edt_search->text());
    ui->jh_widget_dialog_confirmDelete->setVisible(false);
}

void Win_MainSystem::on_ag_btn_update_clicked()
{
    if(id=="")
    {
        return;
    }
    changetype=1;
    ui->jh_lineEdit_people_id->setEnabled(true);
    ui->jh_comboBox_people_name_2->setEnabled(false);
    ui->jh_widget_dialog_addorUpdatePeople->setVisible(true);

    ui->jh_lineEdit_people_name->setText((model->index(iIndex.row(),2,QModelIndex())).data().toString());
    ui->jh_lineEdit_people_id->setText((model->index(iIndex.row(),1,QModelIndex())).data().toString());
    QString type = (model->index(iIndex.row(),3,QModelIndex())).data().toString();
    if(type=="Female")
    {
        ui->jh_comboBox_people_name->setCurrentIndex(1);
    }
    else
    {
        ui->jh_comboBox_people_name->setCurrentIndex(0);
    }

    QString sex = (model->index(iIndex.row(),0,QModelIndex())).data().toString();
    if(type=="Student")
    {
        ui->jh_comboBox_people_name_2->setCurrentIndex(0);
    }
    else
    {
        ui->jh_comboBox_people_name_2->setCurrentIndex(1);
    }

    ui->jh_lineEdit_people_class->setText((model->index(iIndex.row(),5,QModelIndex())).data().toString());
}

void Win_MainSystem::on_jh_label_confirmDelete_btn_confirm_2_clicked()
{
    if(changetype==0)
    {
        if(ui->jh_lineEdit_people_name->text()=="")
        {
            ui->label_9->setText("Name can't be null");
            return;
        }

        if(ui->jh_lineEdit_people_class->text()=="")
        {
            ui->label_9->setText("Class/Dept can't be null");
            return;
        }

        agADDUser(ui->jh_lineEdit_people_name->text(),ui->jh_comboBox_people_name->currentIndex(),ui->jh_lineEdit_people_class->text(),ui->jh_comboBox_people_name_2->currentIndex());
    }
    else if(changetype==1)
    {
        if(ui->jh_lineEdit_people_name->text()=="")
        {
            ui->label_9->setText("Name can't be null");
            return;
        }

        if(ui->jh_lineEdit_people_class->text()=="")
        {
            ui->label_9->setText("Class/Dept can't be null");
            return;
        }

        if(ui->jh_lineEdit_people_id->text()=="")
        {
            ui->label_9->setText("ID can't be null");
            return;
        }

        agUpdateUser(ui->jh_lineEdit_people_id->text(),ui->jh_lineEdit_people_name->text(),ui->jh_comboBox_people_name->currentIndex(),ui->jh_lineEdit_people_class->text(),ui->jh_comboBox_people_name_2->currentIndex());
    }

    model->clear();
    model->setColumnCount(6);
    model->setHeaderData(1,Qt::Horizontal,tr("ID"));
    model->setHeaderData(0,Qt::Horizontal,tr("类别"));
    model->setHeaderData(2,Qt::Horizontal,tr("姓名"));
    model->setHeaderData(3,Qt::Horizontal,tr("性别"));
    model->setHeaderData(4,Qt::Horizontal,tr("年龄"));
    model->setHeaderData(5,Qt::Horizontal,tr("班级/部门"));

    SearchWord(ui->ag_edt_search->text());

    ui->jh_widget_dialog_addorUpdatePeople->setVisible(false);
}

void Win_MainSystem::on_ag_btn_add_clicked()
{
    ui->jh_lineEdit_people_id->setEnabled(false);
    ui->jh_comboBox_people_name_2->setEnabled(true);
    changetype=0;
    ui->jh_widget_dialog_addorUpdatePeople->setVisible(true);
}

void Win_MainSystem::on_ag_rbtn_elec_2_clicked()
{
    searchmode=4;
    ui->ag_search_bg->setVisible(false);
}

void Win_MainSystem::on_ag_btn_exit_2_clicked()
{
    ui->jh_widget_dialog_addorUpdatePeople->setVisible(false);
}

void Win_MainSystem::on_ag_btn_change_barcharmode_clicked()
{
    if(barchartmode==0)
    {
        showBarChart(0);
        showPieChart(0);
        ui->ag_btn_change_barcharmode->setText("Water");
        ui->ag_btn_change_barcharmode->setStyleSheet("QWidget{background-color: rgba(92,172,238,230);color: white;border-radius:4px; }QWidget:hover{background-color: rgba(175,238,238,230);color: white;}QWidget:pressed{background-color: rgba(0,191,255,230);color: white;}");
        barchartmode=1;
    }
    else
    {
        showBarChart(1);
        showPieChart(1);
        ui->ag_btn_change_barcharmode->setText("Electric");
        ui->ag_btn_change_barcharmode->setStyleSheet("QWidget{background-color: rgba(255,59,59,230);color: white;border-radius:4px; }QWidget:hover{background-color: rgba(204,123,123,230);color: white;}QWidget:pressed{background-color: rgba(137,33,33,230);color: white;}");
        barchartmode=0;
    }
}

void db_dump(QString path)
{
    if(path=="")
    {
        return;
    }
    QString cmd = "mysqldump -hagcore.club -uroot -p2104898 test >";
    cmd += path;
    QByteArray ba = cmd.toLocal8Bit();
    const char* dir = ba.data();
    system(dir);
}

void Win_MainSystem::on_ag_btn_fileout_clicked()
{
    QString file_path = QFileDialog::getSaveFileName(this, "Export Sql", "dump", "Sql files (*.sql)");
    db_dump(file_path);
    qDebug()<< file_path;
}

void db_load(QString path)
{
    if(path=="")
    {
        return;
    }
    QString cmd = "mysql -hagcore.club -uroot -p2104898 core <";
    cmd += path;
    QByteArray ba = cmd.toLocal8Bit();
    const char* dir = ba.data();
    system(dir);
}

void Win_MainSystem::on_ag_btn_filein_clicked()
{
    QString file_path = QFileDialog::getOpenFileName(this, "Import Sql", "", "Sql files (*.sql)");
    db_load(file_path);
    qDebug()<<file_path;
}

