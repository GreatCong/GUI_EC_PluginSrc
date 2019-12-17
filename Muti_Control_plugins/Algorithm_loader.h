#ifndef ALGORITHM_LOADER_H
#define ALGORITHM_LOADER_H

#include <QLibrary>
#include <QString>
#include <QList>
#include <QVector>

#include "algorithm_common.h"

typedef struct
{
    QString version;
    QString describe;
    QList<QString> list_machine;
    QVector<Machine_parameter_t> machine_param;
}Algorithm_obj_t;

class Algorithm_loader
{
public:
    Algorithm_loader();
    virtual ~Algorithm_loader();
public:
    typedef enum{
       Json_err_none,
       Json_err_fileInvalid,
       Json_err_parseInvalid
    }JSON_LOAD_ERR;

    int json_load(const QString path);
    Algorithm_obj_t get_Algorithm_obj();
    bool libArray_load(const QString path);
    bool libArray_unload();

    QVector<Algorithm_dll_t> get_algorithm_dll_array();

    void test();
    void set_robotDH_parameters(const QVector<Robot_parameter_s> array_robot_info);
    QVector<Robot_parameter_s> get_robotDH_parameters();
    bool isFileExist(const QString &fullFileName);
private:
    bool lib_load(QLibrary *lib, const QString path, const QString fileName, Algorithm_dll_t *algorithm_dll);
    bool lib_unload(QLibrary *lib);

    Algorithm_obj_t m_Algorithm_obj;
    QVector<QLibrary*> m_libArray;
    QVector<Algorithm_dll_t> m_Algorithm_dll_array;
    QVector<Robot_parameter_s> m_array_robot_info;
};

#endif // ALGORITHM_LOADER_H
