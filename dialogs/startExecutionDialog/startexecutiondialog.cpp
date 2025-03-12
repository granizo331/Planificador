#include "startexecutiondialog.h"
#include "constants.h"
#include "convertions.h"
#include "db/postgres/postgres.h"
#include "custom/currenttask.h"
#include "subtaskexecutions.h"
#include "tasklogs.h"
#include "logger/log.h"

namespace logic
{
StartExecutionDialog::StartExecutionDialog(db::Postgres * const db,
                                           QObject * const parent)
    : QObject(parent),
    m_Db(db)
{

}

StartExecutionDialog::~StartExecutionDialog()
{

}


void StartExecutionDialog::init() noexcept
{


}

void StartExecutionDialog::createTasklog(const QString &name)
{

    const QString userID    = "1";
    const QString startTime = QDateTime::currentDateTime().toString(cus::TIMESTAMP_FORMAT);

    const db::TaskLogs tasklogData("DEFAULT",
                                   name,
                                   cus::currentTask.id(),
                                   userID,
                                   startTime,
                                   "NULL",
                                   cus::toStr(cus::ExecutionStatus::Open),
                                   "");
    db::Result<db::TaskLogs> const result = m_Db->createTasklog(tasklogData);

    if ((db::ErrorCode::NO_ERROR == result.error()) && (false == result.data().isEmpty()))
    {
       cus::currentTask.setTasklog(result.data().at(FIRST));
         const db::SubtaskExecutions subtaskExecution(cus::DEFAULT_STR,
                                                      cus::currentTask.tasklogID(),
                                                      cus::toStr(cus::ExecutionStatus::BeingExecuted),
                                                      cus::toStr(cus::ExecutionMode::Simulation),
                                                      startTime,
                                                      startTime,
                                                      cus::currentTask.id());


             db::Result<db::GenericData> const subtaskExecutionID = m_Db->createTaskExecution(subtaskExecution);

             if ((db::ErrorCode::NO_ERROR == subtaskExecutionID.error()) && (false == subtaskExecutionID.data().isEmpty()))
             {
                QString const subtaskExecId = subtaskExecutionID.data().at(FIRST)[FIRST];
                cus::currentTask.setSubtaskExecID(subtaskExecId);
                cus::currentTask.setTasklogStatus(cus::ExecutionStatus::BeingExecuted);
                logger::log.info(rc::RetCode::SubtaskExecCreatedSuccesfully,cus::currentTask.name());
                emit createdTasklog();
             }
             else
             {
                logger::log.error(rc::RetCode::SubtaskExecCreationError, cus::currentTask.name());
             }
    }
    else
    {
        emit duplicateName();
    }
}

} //namespace logic


