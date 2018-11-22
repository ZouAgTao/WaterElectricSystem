#include "win_login.h"
#include "ui_win_login.h"

Win_Login::Win_Login(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Win_Login)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    if(!agDBConnect())
    {
        qDebug()<<"MySQL Connect Fail";
        exit(-1);
    }
}

Win_Login::~Win_Login()
{
    delete ui;
}

void Win_Login::on_ag_btn_login_clicked()
{
    ui->ag_hint->setText("");

    QString username = ui->ag_edt_username->text();
    QString password = ui->ag_edt_password->text();

    if(agLogin(username,password))
    {
        win_mainsystem = new Win_MainSystem;
        win_mainsystem->show();
//        qDebug()<<"2";
//        win_mainsystem->show();
//        qDebug()<<"3";
        this->setVisible(false);
//        exit(0);
    }
    else
    {
        ui->ag_hint->setText("Login Unsuccessfully , Please check input");
    }

}

void Win_Login::on_ag_btn_close_clicked()
{
    exit(0);
}
