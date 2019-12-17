#include "Algorithm_loader.h"

#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileInfo>

#include <QDebug>

Algorithm_loader::Algorithm_loader()
{

}

Algorithm_loader::~Algorithm_loader()
{
    for(int i=0;i<m_libArray.size();i++){
        delete m_libArray[i];
        m_libArray[i] = nullptr;
    }
}

int Algorithm_loader::json_load(const QString path)
{
    QFile loadfile(path);

    if(!loadfile.open(QIODevice::ReadOnly)){
        //qDebug() << "could not open project json";
        return Json_err_fileInvalid;
    }

    QByteArray allData = loadfile.readAll();
    loadfile.close();

    QJsonParseError json_error;
    QJsonDocument jsonDoc(QJsonDocument::fromJson(allData,&json_error));

    if(json_error.error != QJsonParseError::NoError){
        //qDebug() << "parse json error!";
        return Json_err_parseInvalid;
    }

    QJsonObject rootObj = jsonDoc.object();

//    QStringList keys = rootObj.keys();
//    for(int i = 0; i < keys.size(); i++)
//    {
//        qDebug() << "key" << i << " is:" << keys.at(i);
//    }

    if(rootObj.contains("message")){
        QJsonObject subObj = rootObj.value("message").toObject();
        m_Algorithm_obj.version = subObj["version"].toString();
        m_Algorithm_obj.describe = subObj["describe"].toString();
    }

    if(rootObj.contains("machine array")){
        QJsonArray subArray = rootObj.value("machine array").toArray();
        if(subArray.size() >0){
            m_Algorithm_obj.list_machine.clear();
            for(int i=0;i<subArray.size();i++){
                //qDebug() << i<<" value is:" << subArray.at(i).toString();
               m_Algorithm_obj.list_machine.append(subArray.at(i).toString());
            }
        }
    }

    if(m_Algorithm_obj.list_machine.size() >0){
        m_Algorithm_obj.machine_param.clear();

        foreach (QString obj, m_Algorithm_obj.list_machine) {
            if(rootObj.contains(obj)){
                QJsonObject subObj = rootObj.value(obj).toObject();
                Machine_parameter_t machine_param;
                if(subObj.contains("DEFAULT_STEPS_PER_UNIT")){
                    QJsonArray subArray1 = subObj.value("DEFAULT_STEPS_PER_UNIT").toArray();
                    for(int i=0;i<subArray1.size();i++){
                        //qDebug() << i<<" value is:" << subArray1.at(i).toDouble();
//                       m_Algorithm_obj.list_machine.append(subArray.at(i).toString());
                        machine_param.step_per_unit.append(subArray1.at(i).toDouble());
                    }
                }

                if(subObj.contains("MANUAL_HOME_POS")){
                     QJsonArray subArray2 = subObj.value("MANUAL_HOME_POS").toArray();
                     for(int i=0;i<subArray2.size();i++){
//                         qDebug() << i<<" value is:" << subArray2.at(i).toDouble();
//                        m_Algorithm_obj.list_machine.append(subArray.at(i).toString());
                         machine_param.manual_home_pos.append(subArray2.at(i).toDouble());
                     }
                }

                m_Algorithm_obj.machine_param.append(machine_param);
            }//if
        }//foreach
    }

    return Json_err_none;
}

Algorithm_obj_t Algorithm_loader::get_Algorithm_obj()
{
    return m_Algorithm_obj;
}

bool Algorithm_loader::libArray_load(const QString path)
{
    bool ret = false;

    int lib_num = m_Algorithm_obj.list_machine.size();
    if(lib_num == 0) return false;

    m_Algorithm_dll_array.resize(lib_num);
    m_libArray.resize(lib_num);

    ret = true;
    QString file_name;
    for(int i=0;i<m_libArray.size();i++){//注意path最后要加上斜杠
        m_libArray[i] = new QLibrary();
        file_name = m_Algorithm_obj.list_machine.at(i);
#ifndef Q_OS_WIN
        file_name = "lib"+file_name;//linux会加上lib的前缀
#endif
        m_Algorithm_dll_array[i].is_load = lib_load(m_libArray[i],path,file_name,&m_Algorithm_dll_array[i]);
        m_Algorithm_dll_array[i].model_name = m_Algorithm_dll_array[i].is_load? file_name:"";//传入模型的名称
        ret &=  m_Algorithm_dll_array[i].is_load;
    }

    return ret;
}

bool Algorithm_loader::libArray_unload()
{
    bool ret = false;

    ret = true;
    for(int i=0;i<m_libArray.size();i++){
        ret &= lib_unload(m_libArray[i]);
        m_Algorithm_dll_array[i].is_load = false;
    }

    return ret;
}

QVector<Algorithm_dll_t> Algorithm_loader::get_algorithm_dll_array()
{
    return m_Algorithm_dll_array;
}

bool Algorithm_loader::lib_load(QLibrary *lib, const QString path ,const QString fileName, Algorithm_dll_t *algorithm_dll)
{
    bool ret = false;

    lib->setFileName(path+fileName);
//    qDebug() << (path+fileName);

    if(lib->load()){
//        calculate_arm_dll = (calculate_dll)m_libs.resolve("calculate_arm");
//        calculate_forward_dll = (calculate_dll)m_libs.resolve("calculate_forward");
//        qDebug() << "load OK";
        algorithm_dll->calculate_arm_dll = (calculate_dll)lib->resolve("calculate_arm");
        algorithm_dll->calculate_forward_dll = (calculate_dll)lib->resolve("calculate_forward");

        if(algorithm_dll->calculate_arm_dll && algorithm_dll->calculate_forward_dll){
           ret = true;
        }
        else{
            ret = false;
        }
    }
    else{
       ret = false;
    }

    if(ret == false){//调试用的
        qDebug() << lib->fileName() << "load error!";
    }

    return ret;
}

bool Algorithm_loader::lib_unload(QLibrary *lib)
{
   bool ret = false;

   ret = lib->unload();

   if(ret == false){//调试用的
       qDebug() << lib->fileName() << "unload error!";
   }

   return ret;
}

void Algorithm_loader::set_robotDH_parameters(const QVector<Robot_parameter_s> array_robot_info)
{
    m_array_robot_info = array_robot_info;
//    foreach (Robot_parameter_s param, m_array_robot_info) {
//        qDebug() << param.toString();
//    }
}

///
/// \brief 测试函数
///
void Algorithm_loader::test()
{
    for(int i=0;i<m_libArray.size();i++){
        if(m_Algorithm_dll_array[i].is_load){
            (*(m_Algorithm_dll_array[i].calculate_arm_dll))(nullptr,nullptr,nullptr,0,0);
            (*(m_Algorithm_dll_array[i].calculate_forward_dll))(nullptr,nullptr,nullptr,0,0);
        }
    }

}

QVector<Robot_parameter_s> Algorithm_loader::get_robotDH_parameters()
{
  return m_array_robot_info;
}

bool Algorithm_loader::isFileExist(const QString &fullFileName)
{
    QFileInfo fileInfo(fullFileName);
    if(fileInfo.isFile())
    {
        return true;
    }
    return false;
}

