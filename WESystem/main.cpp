#include "win_login.h"
#include "win_mainsystem.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Win_Login w;
//    Win_MainSystem w;

    w.show();

    return a.exec();
}
