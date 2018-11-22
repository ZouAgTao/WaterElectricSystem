#ifndef WIN_LOGIN_H
#define WIN_LOGIN_H

#pragma once

#include <QMainWindow>
#include <QPainter>
#include <QDebug>

#include "agdb.h"
#include "win_mainsystem.h"

namespace Ui {
class Win_Login;
}

class Win_Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit Win_Login(QWidget *parent = nullptr);
    ~Win_Login();
    Win_MainSystem * win_mainsystem;

private slots:
    void on_ag_btn_login_clicked();

    void on_ag_btn_close_clicked();

private:
    Ui::Win_Login *ui;
};

#endif // WIN_LOGIN_H
