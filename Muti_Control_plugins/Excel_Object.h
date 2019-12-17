#ifndef EXCEL_OBJECT_H
#define EXCEL_OBJECT_H

#include <QObject>
#include <QVector>

#include "algorithm_common.h"

class Excel_Object: public QObject
{
    Q_OBJECT
public:
    Excel_Object(QObject *parent = 0);
    bool readExcel(const QString &path);
    QVector<Robot_parameter_s> get_Array_robotParameter();
private:
    QVector<Robot_parameter_s> m_array_robotObj_fromExcel;
    bool isFileExist(QString fullFileName);
signals:

public slots:

};

#endif // EXCEL_OBJECT_H
