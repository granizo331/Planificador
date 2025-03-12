#include "postgres.h"
#include "constants.h"
#include "subtasks.h"
#include "multimedia.h"
#include "steps.h"
#include <QDateTime>

namespace db
{
Postgres::Postgres():Commondb()
{}


Postgres::~Postgres()
{
    disconnect();
}

Result<GenericData> Postgres::createSubtask(const QString &name, const QString &model, const QString &comment)
{
    Result<GenericData> result(ErrorCode::NO_ERROR);
    QDateTime currenttime = QDateTime::currentDateTime();
    QString timeStamp = currenttime.toString("yyyy-MM-dd HH:mm:ss");

    const QString query = "INSERT INTO buildschema.subtasks VALUES(DEFAULT," + quoted(name) + "," + model + "," + quoted(comment) + ","
                          + quoted("ready") + "," + "1" + "," + quoted(timeStamp) + ")";

    qWarning() << query;
    execTransactionWithReturnValue(query,result);

    return result;

}

Result<GenericData> Postgres::readAllTask()
{
    Result<GenericData> result(ErrorCode::NO_ERROR);

    const QString query = "SELECT * FROM buildschema.subtasks";
    execTransactionWithReturnValue(query,result);

    return result;
}

Result<Subtasks> Postgres::getTask(const QString &id)
{

    Result<Subtasks> result(ErrorCode::NO_ERROR);

    const QString query = "SELECT * FROM buildschema.subtasks WHERE id = " + id;

    execQueryAndFillData(query,result);

    return result;
}

Result<GenericData> Postgres::getTaskByName(const QString &name)
{

    Result<GenericData> result(ErrorCode::NO_ERROR);

    const QString query = "SELECT * FROM buildschema.subtasks WHERE name = " + quoted(name);

    execQueryAndFillData(query,result);
    return result;
}

Result<Multimedia> Postgres::getMultimediaFilesInfo()
{
    Result<Multimedia> result(ErrorCode::NO_ERROR);

    const QString query = "SELECT * FROM multimediaschema.multimedia";
    execQueryAndFillData(query,result);

    return result;
}

Error Postgres::insertMultimediaFiles(const QString &values)
{
    Result<GenericData> result(ErrorCode::NO_ERROR);
    const QString query = "INSERT INTO multimediaschema.multimedia VALUES" + values + " RETURNING ID";


    execTransactionWithReturnValue(query,result);

    return result.error();
}

Result<Steps> Postgres::getSteps(const QString &taskID)
{
    Result<Steps> result(ErrorCode::NO_ERROR);

    const QString query = "SELECT * FROM buildschema.steps WHERE subtask_id = " + taskID + " ORDER BY executionorder ASC";

    execQueryAndFillData(query,result);

    return result;
}

Result<SLCommandsLibrary> Postgres::getStepsLibrary()
{
    Result<SLCommandsLibrary> result(ErrorCode::NO_ERROR);

    const QString query = "SELECT * FROM librariesschema.slcommandslibrary";

    execQueryAndFillData(query,result);

    return result;
}

Result<Steps> Postgres::getStep(const QString &id)
{

    Result<Steps> result(ErrorCode::NO_ERROR);

    const QString query = "SELECT * FROM buildschema.steps WHERE id = " + id;


    execQueryAndFillData(query,result);

    return result;

}

Error Postgres::updateStep(const Steps &step)
{

    const QString query = "UPDATE buildschema." + step.tableName() +
                          " SET comment = " + quoted(step.comment()) + ", " +
                          step.getFieldName(Steps::Fields::SlText) + " = " + quoted(step.slText()) +
                          " WHERE id = " + step.id();

    const Error error = execTransaction(query);

    return error;
}


Error Postgres::updateStepsOrderAfterStepAddition(const QString &subtaskID, const QString &order)
{
    Result<GenericData> result(ErrorCode::NO_ERROR);
    const QString query = "UPDATE buildschema." + Steps().tableName() +
                          " SET executionorder = executionorder + 1 " +
                          " WHERE subtask_id = " + subtaskID +
                          " AND executionorder >= " + order +
                          " RETURNING id";

    execTransactionWithReturnValue(query, result);

    return result.error();

}


Error Postgres::insertStep(const Steps &step)
{
    Result<GenericData> result(ErrorCode::NO_ERROR);

    const QString query = "INSERT INTO buildschema." + Steps().tableName() +
                          " VALUES (" +
                          step.id()                + ", " +
                          step.subtaskID()          + ", " +
                          step.executionOrder()    + ", " +
                          quoted(step.comment())   + ", " +
                          quoted(step.slText())    + ")" +
                          " RETURNING id";

    execTransactionWithReturnValue(query,result);

    return result.error();
}

Error Postgres::deleteStep(const QString &subtaskID, const QString &executionOrder)
{
    Result<GenericData> result(ErrorCode::NO_ERROR);

    const QString query = "DELETE FROM buildschema." + Steps().tableName() +
                          " WHERE subtask_id = " + subtaskID +
                          " AND " + Steps().getFieldName(Steps::Fields::ExecutionOrder) + " = " + executionOrder +
                          " RETURNING id";


    execTransactionWithReturnValue(query,result);

    return result.error();
}

Error Postgres::updateStepsOrderAfterStepDeletion(const QString &subtaskID, const QString &order)
{

    Result<GenericData> result(ErrorCode::NO_ERROR);
    const QString query = "UPDATE buildschema." + Steps().tableName() +
                          " SET executionorder = executionorder - 1 " +
                          " WHERE subtask_id = " + subtaskID +
                          " AND executionorder > " + order;

    execTransactionWithReturnValue(query, result);

    return result.error();
}

Result<TaskLogs> Postgres::createTasklog(const TaskLogs &tasklog)
{
    Result<TaskLogs> result(ErrorCode::NO_ERROR);

    const QString query = "INSERT INTO logschema.tasklogs VALUES(DEFAULT, " +
                          quoted(tasklog.name()) + "," + tasklog.taskID() + "," +
                          tasklog.userID() + "," + quoted(tasklog.startTime()) + "," +
                          tasklog.stopTime() + "," + quoted(tasklog.status()) + "," +
                          quoted(tasklog.comment()) + ") ON CONFLICT ON CONSTRAINT tasklogs_name_un DO NOTHING " +
                          "RETURNING *";

    qWarning() << query;

    execTransactionWithReturnValue(query,result);

    return result;


}

Result<GenericData> Postgres::createTaskExecution(const SubtaskExecutions &taskExecution)
{
    Result<GenericData> result(ErrorCode::NO_ERROR);

    const QString query = "INSERT INTO executionschema.subtaskexecutions VALUES(DEFAULT, " +
                          taskExecution.tasklogID() + "," + quoted(taskExecution.status()) + "," +
                          quoted(taskExecution.executionMode()) + "," + quoted(taskExecution.startTime()) + "," +
                          quoted(taskExecution.stopTime()) + "," + taskExecution.subtaskID() + ") RETURNING ID";

    qWarning() << query;

    execTransactionWithReturnValue(query,result);

    return result;
}

Result<StepExecutions> Postgres::insertStepExecution(const StepExecutions &stepExecution)
{
    Result<StepExecutions> result(ErrorCode::NO_ERROR);

    const QString query = "INSERT INTO executionschema.stepexecutions VALUES(DEFAULT, " +
    stepExecution.stepID() + "," + stepExecution.subtaskExecutionID() + "," + stepExecution.executionOrder() + "," +
                          quoted(stepExecution.status()) + "," + stepExecution.userID() + ") RETURNING *";

    qWarning() << query;

    execTransactionWithReturnValue(query,result);
    return result;
}

Result<StepExecutions> Postgres::updateStepExecutionStatus(const QString &stepExecutionID, const QString &status)
{
    Result<StepExecutions> result(ErrorCode::NO_ERROR);

    const QString query = "UPDATE executionschema.stepexecutions SET status = " + quoted(status) + " " +
                          "WHERE id = " + stepExecutionID + " RETURNING *";

    qWarning() << query;

    execTransactionWithReturnValue(query,result);

    return result;
}

} //namespace db

