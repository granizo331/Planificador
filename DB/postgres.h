#pragma once

#include <QString>
#include "db/dbglobal.h"
#include "db/errors.h"
#include "db/commondb.h"
#include "db/genericdata.h"
#include "db/postgres/subtasks.h"
#include "db/postgres/multimedia.h"
#include "db/postgres/steps.h"
#include "db/postgres/slcommandslibrary.h"
#include "db/postgres/tasklogs.h"
#include "db/postgres/subtaskexecutions.h"
#include "db/postgres/stepexecutions.h"


#ifndef NO_PQSL_LINKED
#include <pqxx/pqxx>
#endif

namespace db
{
class OMSDB_EXPORT Postgres : public Commondb
{
public:
    //Constructor
    explicit Postgres();
    //Destructor
    ~Postgres() override;
public slots:
    //Funcion para crear una tarea en BBDD
    Result<GenericData> createSubtask(const QString &name, const QString &model, const QString &comment);
    //Funcion para leer todas las tareas de la BBDD
    Result<GenericData> readAllTask();
    //Funcion para obtener una tarea concreta de la BBDD
    Result<Subtasks> getTask(const QString &id);
    //Funcion para obtener una tarea por el nombre en la BBDD
    Result<GenericData> getTaskByName(const QString &name);
    //Funcion para obtener información de los archivos multimedia
    Result<Multimedia> getMultimediaFilesInfo();
    //Funcion para añadir multimedia
    Error insertMultimediaFiles(const QString &values);
    //Funcion que obtiene los pasos de una tarea
    Result<Steps> getSteps(const QString &task);
    //Funcion para obtener la librería de pasos
    Result<SLCommandsLibrary> getStepsLibrary();
    //Funcion para obtener un paso
    Result<Steps> getStep(const QString &id);
    //Funcion que actualiza un paso
    Error updateStep(const Steps &step);
    //Funcion para actualizar el orden de un paso
    Error updateStepsOrderAfterStepAddition(const QString &subtaskID, const QString &order);
    //Funcion para insertar un paso
    Error insertStep(const Steps &step);
    //Funcion para borrar un paso
    Error deleteStep(const QString &subtaskID, const QString &executionOrder);
    //Funcion para actualizar el orden de un paso después de borrar
    Error updateStepsOrderAfterStepDeletion(const QString &subtaskID, const QString &order);
    //Funcion para crear un task log
    Result<TaskLogs> createTasklog(const TaskLogs &tasklog);
    //Funcion para crear la ejecución de una tarea
    Result<GenericData> createTaskExecution(const SubtaskExecutions &taskExecution);
    //Funcion para crear la ejecución de un paso
    Result<StepExecutions> insertStepExecution(const StepExecutions &stepExectuon);
    //Función para actualizar el estado de una ejecución de paso
    Result<StepExecutions> updateStepExecutionStatus(const QString &stepExecutionID, const QString &status);
};



} //namespace db


