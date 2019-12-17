#include "Excel_Object.h"

#include "xlsxdocument.h"
#include "xlsxworkbook.h"
#include "xlsxworksheet.h"

#include <QDebug>
#include <QFileInfo>

Excel_Object::Excel_Object(QObject *parent):QObject(parent)
{

}

bool Excel_Object::readExcel(const QString &path)
{
    bool ret = false;

    ret = isFileExist(path);
    if(ret == false){
        return ret;
    }

    QXlsx::Document xlsx(path);
        QXlsx::Workbook *workBook = xlsx.workbook();
        QXlsx::Worksheet *workSheet = static_cast<QXlsx::Worksheet*>(workBook->sheet(0));

        m_array_robotObj_fromExcel.clear();
        QXlsx::Cell *cell = NULL;
        int tmp_index = 0;

        for(int i=2;i<= workSheet->dimension().rowCount();i++){ //从2开始，1是表格的抬头
            Robot_parameter_s robot_p;
            tmp_index = 1;
            cell = workSheet->cellAt(i, tmp_index++);
            if(cell==NULL){ //如果ID为空，则为无效
                continue;
            }
            robot_p.value_id = cell==NULL? -1: cell->value().toInt();
            cell = workSheet->cellAt(i, tmp_index++);
            robot_p.value_name = cell==NULL? "": cell->value().toString();
            cell = workSheet->cellAt(i, tmp_index++);
            robot_p.value_type = cell==NULL? -1: cell->value().toInt();
            cell = workSheet->cellAt(i, tmp_index++);
            robot_p.value_DOF = cell==NULL? -1: cell->value().toInt();
            robot_p.resize_DOF();

            for(int j=0;j<robot_p.value_DOF;j++){
                cell = workSheet->cellAt(i, tmp_index+(3*j+0));
                robot_p.array_DH_parameter[j].value_d = cell==NULL? -1: cell->value().toFloat();
                cell = workSheet->cellAt(i, tmp_index+(3*j+1));
                robot_p.array_DH_parameter[j].value_a = cell==NULL? -1: cell->value().toFloat();
                cell = workSheet->cellAt(i, tmp_index+(3*j+2));
                robot_p.array_DH_parameter[j].value_alpha = cell==NULL? -1: cell->value().toFloat();
            }

            m_array_robotObj_fromExcel.push_back(robot_p);
        }

//        for(int i=0;i<m_array_robotObj_fromExcel.size();i++){
//            qDebug() << m_array_robotObj_fromExcel[i].toString();
//        }
      return ret;
}

QVector<Robot_parameter_s> Excel_Object::get_Array_robotParameter()
{
    return m_array_robotObj_fromExcel;
}

bool Excel_Object::isFileExist(QString fullFileName)
{
    QFileInfo fileInfo(fullFileName);
    if(fileInfo.isFile())
    {
        return true;
    }
    return false;
}
