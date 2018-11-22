#include "agdb.h"

bool agDBConnect()
{
    db = QSqlDatabase::addDatabase("QMYSQL3");
    db.setHostName(AGHOSTNAME);
    db.setDatabaseName(AGDBNAME);
    db.setUserName(AGUSERNAEM);
    db.setPassword(AGPASSWORD);

    if(db.open())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool agLogin(QString username , QString password)
{
    QString md5;
    QByteArray bb = QCryptographicHash::hash ( password.toLatin1(), QCryptographicHash::Md5 );
    md5.append(bb.toHex());
    password=md5;

    QSqlQuery query("select * from info_admin");

    bool login_state = false;

    while(query.next())
    {
        QString user = query.value(0).toString();
        QString pass = query.value(1).toString();

        if(user==username && pass==password)
        {
            login_state=true;
            break;
        }
    }
    if(login_state)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void agQueryElecAndWater(int year,double * elec,double * water)
{
    QSqlQuery query("select * from stat_finance where Year(now_date) = "+QString::number(year));     //查询表的内容
    while(query.next())
    {
        double elec_num = query.value(2).toDouble();
        double water_num = query.value(3).toDouble();
        QString date = query.value(1).toString();
        date = date.mid(5,2);
        int month = date.toInt();
        elec[month-1]+=elec_num;
        water[month-1]+=water_num;
    }
}

void agQueryElecAndWaterByID(int year,double * elec,double * water,QString id)
{
    QSqlQuery query("select * from stat_finance where uid = "+id+" and Year(now_date) = "+QString::number(year));     //查询表的内容
    while(query.next())
    {
        double elec_num = query.value(2).toDouble();
        double water_num = query.value(3).toDouble();
        QString date = query.value(1).toString();
        date = date.mid(5,2);
        int month = date.toInt();
        elec[month-1]+=elec_num;
        water[month-1]+=water_num;
    }
}

void agUpdateUser(QString id,QString name, int sex , QString branch , int type)
{
    QSqlQuery query;

    if(type==1)
    {
        //学生
        query.prepare("update info_student set name=?,gender=?,class=? where uid=?");
    }
    else
    {
        //教工
        query.prepare("update info_teacher set name=?,gender=?,class=? where uid=?");
    }


    query.addBindValue(name);
    query.addBindValue(sex);
    query.addBindValue(branch);
    query.addBindValue(id);
    query.exec();

}

void agADDUser(QString name, int sex , QString branch , int type)
{
//    qDebug()<<name<<sex<<branch<<type;
    QSqlQuery query;
    if(type==1)
    {
        //学生
        query.prepare("INSERT INTO info_student (name,gender,birth_date,class,paid_elec,paid_water) VALUES(:name,:gender,:birth_date,:class,:paid_elec,:paid_water)");
    }
    else
    {
        //教工
        query.prepare("INSERT INTO info_teacher (name,gender,birth_date,dept,paid_elec,paid_water) VALUES(:name,:gender,:birth_date,:class,:paid_elec,:paid_water)");
    }

    query.bindValue(":name",name);
    query.bindValue(":gender",sex);
    query.bindValue(":birth_date","1998-10-16");
    query.bindValue(":class",branch);
    query.bindValue(":paid_elec",1);
    query.bindValue(":paid_water",1);

    query.exec();

}

void agQueryByUnPaid(QStandardItemModel * model)
{
    QSqlQuery query_stu("select * from info_student where paid_elec = 0 or paid_water = 0");     //查询表的内容

    while(query_stu.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Student"))<<new QStandardItem(query_stu.value(0).toString())<<new QStandardItem(query_stu.value(1).toString());

        int sex_num = query_stu.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_stu.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_stu.value(4).toString());
        model->appendRow(list);
    }

    QSqlQuery query_tc("select * from info_teacher where paid_elec = 0 or paid_water = 0 ");     //查询表的内容

    while(query_tc.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Teacher"))<<new QStandardItem(query_tc.value(0).toString())<<new QStandardItem(query_tc.value(1).toString());

        int sex_num = query_tc.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_tc.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_tc.value(4).toString());
        model->appendRow(list);
    }
}

void agDDeleteByID(QString id)
{
    QSqlQuery query;
    query.prepare(QString("DELETE FROM info_student WHERE uid=?"));
//    query.prepare(QString("DELETE FROM info_teacher WHERE uid=?"));
//    query.prepare(QString("DELETE FROM stat_finance WHERE uid=?"));
    query.addBindValue(id);
    query.exec();

//    query.prepare(QString("DELETE FROM info_student WHERE uid=?"));
    query.prepare(QString("DELETE FROM info_teacher WHERE uid=?"));
//    query.prepare(QString("DELETE FROM stat_finance WHERE uid=?"));
    query.addBindValue(id);
    query.exec();

//    query.prepare(QString("DELETE FROM info_student WHERE uid=?"));
//    query.prepare(QString("DELETE FROM info_teacher WHERE uid=?"));
    query.prepare(QString("DELETE FROM stat_finance WHERE uid=?"));
    query.addBindValue(id);
    query.exec();
}

void agQueryPayment(int * elec_payment,int * water_payment)
{
    QSqlQuery querystu("select * from info_student");     //查询表的内容
    while(querystu.next())
    {
        int elec = querystu.value(5).toInt();
        int water = querystu.value(6).toInt();

        elec_payment[elec]++;
        water_payment[water]++;
    }

    QSqlQuery querytc("select * from info_teacher");     //查询表的内容
    while(querytc.next())
    {
        int elec = querytc.value(5).toInt();
        int water = querytc.value(6).toInt();

        elec_payment[elec]++;
        water_payment[water]++;
    }
}

void agQueryAllUser(QStandardItemModel *model)
{
    QSqlQuery query_stu("select * from info_student");     //查询表的内容

    while(query_stu.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Student"))<<new QStandardItem(query_stu.value(0).toString())<<new QStandardItem(query_stu.value(1).toString());

        int sex_num = query_stu.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_stu.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_stu.value(4).toString());
        model->appendRow(list);
    }

    QSqlQuery query_tc("select * from info_teacher");     //查询表的内容

    while(query_tc.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Teacher"))<<new QStandardItem(query_tc.value(0).toString())<<new QStandardItem(query_tc.value(1).toString());

        int sex_num = query_tc.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_tc.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_tc.value(4).toString());
        model->appendRow(list);
    }
}

void agQueryByName(QStandardItemModel *model, QString keyword)
{
    QSqlQuery query_stu("select * from info_student where name like '%"+keyword+"%' ");     //查询表的内容

    while(query_stu.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Student"))<<new QStandardItem(query_stu.value(0).toString())<<new QStandardItem(query_stu.value(1).toString());

        int sex_num = query_stu.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_stu.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_stu.value(4).toString());
        model->appendRow(list);
    }

    QSqlQuery query_tc("select * from info_teacher where name like '%"+keyword+"%' ");     //查询表的内容

    while(query_tc.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Teacher"))<<new QStandardItem(query_tc.value(0).toString())<<new QStandardItem(query_tc.value(1).toString());

        int sex_num = query_tc.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_tc.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_tc.value(4).toString());
        model->appendRow(list);
    }
}

void agQueryByID(QStandardItemModel * model , QString keyword)
{
    QSqlQuery query_stu("select * from info_student where uid like '%"+keyword+"%' ");     //查询表的内容

    while(query_stu.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Student"))<<new QStandardItem(query_stu.value(0).toString())<<new QStandardItem(query_stu.value(1).toString());

        int sex_num = query_stu.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_stu.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_stu.value(4).toString());
        model->appendRow(list);
    }

    QSqlQuery query_tc("select * from info_teacher where uid like '%"+keyword+"%' ");     //查询表的内容

    while(query_tc.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Teacher"))<<new QStandardItem(query_tc.value(0).toString())<<new QStandardItem(query_tc.value(1).toString());

        int sex_num = query_tc.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_tc.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_tc.value(4).toString());
        model->appendRow(list);
    }
}


void agQueryByWater(QStandardItemModel * model , QString keyword)
{
    int min = keyword.toInt()-5;
    int max = keyword.toInt()+5;
    QSqlQuery query_stu("select distinct info_student.uid , info_student.name , info_student.gender , info_student.birth_date , info_student.class , info_student.paid_elec , info_student.paid_water from info_student , stat_finance where info_student.uid = stat_finance.uid and stat_finance.use_value_water between "+QString::number(min)+" and "+QString::number(max)+" and MONTH(now_date) = 1");     //查询表的内容

    while(query_stu.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Student"))<<new QStandardItem(query_stu.value(0).toString())<<new QStandardItem(query_stu.value(1).toString());

        int sex_num = query_stu.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_stu.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_stu.value(4).toString());
        model->appendRow(list);
    }

    QSqlQuery query_tc("select distinct  info_teacher.uid , info_teacher.name , info_teacher.gender , info_teacher.birth_date , info_teacher.dept , info_teacher.paid_elec , info_teacher.paid_water from info_teacher , stat_finance where info_teacher.uid = stat_finance.uid and stat_finance.use_value_water between "+QString::number(min)+" and "+QString::number(max)+" and MONTH(now_date) = 1");   //查询表的内容

    while(query_tc.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Teacher"))<<new QStandardItem(query_tc.value(0).toString())<<new QStandardItem(query_tc.value(1).toString());

        int sex_num = query_tc.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_tc.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_tc.value(4).toString());
        model->appendRow(list);
    }
}

void agQueryByElec(QStandardItemModel * model , QString keyword)
{
    int min = keyword.toInt()-100;
    int max = keyword.toInt()+100;
    QSqlQuery query_stu("select distinct info_student.uid , info_student.name , info_student.gender , info_student.birth_date , info_student.class , info_student.paid_elec , info_student.paid_water from info_student , stat_finance where info_student.uid = stat_finance.uid and stat_finance.use_value_elec between "+QString::number(min)+" and "+QString::number(max)+" and MONTH(now_date) = 1");     //查询表的内容

    while(query_stu.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Student"))<<new QStandardItem(query_stu.value(0).toString())<<new QStandardItem(query_stu.value(1).toString());

        int sex_num = query_stu.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_stu.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_stu.value(4).toString());
        model->appendRow(list);
    }

    QSqlQuery query_tc("select distinct  info_teacher.uid , info_teacher.name , info_teacher.gender , info_teacher.birth_date , info_teacher.dept , info_teacher.paid_elec , info_teacher.paid_water from info_teacher , stat_finance where info_teacher.uid = stat_finance.uid and stat_finance.use_value_elec between "+QString::number(min)+" and "+QString::number(max)+" and MONTH(now_date) = 1");   //查询表的内容

    while(query_tc.next())
    {
        QList<QStandardItem*> list;
        list<<new QStandardItem(QString::fromLocal8Bit("Teacher"))<<new QStandardItem(query_tc.value(0).toString())<<new QStandardItem(query_tc.value(1).toString());

        int sex_num = query_tc.value(2).toInt();
        QString sex="";
        if(sex_num==0)
        {
            sex=QString::fromLocal8Bit("Male");
        }
        else
        {
            sex=QString::fromLocal8Bit("Female");
        }
        list<<new QStandardItem(sex);
        QString date =  query_tc.value(3).toString();
        date = date.mid(0,4);
        int age = date.toInt();
        QDateTime dt = QDateTime::currentDateTime();
        int now = dt.toString("yyyy").toInt();
        age = now - age;
        list<<new QStandardItem(QString::number(age));
        list<<new QStandardItem(query_tc.value(4).toString());
        model->appendRow(list);
    }
}
