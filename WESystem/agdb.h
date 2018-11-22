#ifndef AGDB_H
#define AGDB_H

#pragma once

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QCryptographicHash>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QList>
#include <QDateTime>

const QString AGHOSTNAME = "agcore.club";
const QString AGUSERNAEM = "root";
const QString AGPASSWORD = "2104898";
const QString AGDBNAME = "test";

static QSqlDatabase db;

bool agDBConnect();
bool agLogin(QString username , QString password);
void agQueryElecAndWater(int year,double * elec,double * water);
void agQueryPayment(int * elec_payment,int * water_payment);
void agQueryAllUser(QStandardItemModel * model);
void agQueryByName(QStandardItemModel * model , QString keyword);
void agQueryByID(QStandardItemModel * model , QString keyword);
void agQueryByWater(QStandardItemModel * model , QString keyword);
void agQueryByElec(QStandardItemModel * model , QString keyword);
void agQueryByUnPaid(QStandardItemModel * model);
void agQueryElecAndWaterByID(int year,double * elec,double * water,QString id);
void agDDeleteByID(QString id);
void agADDUser(QString name, int sex , QString branch , int type);
void agUpdateUser(QString id,QString name, int sex , QString branch , int type);

#endif // AGDB_H
