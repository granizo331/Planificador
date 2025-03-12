#include "opendialog.h"
#include "db/postgres/postgres.h"
#include "custom/enums/convertions.h"
#include "subtasks.h"
#include "tasktype.h"
#include "custom/currenttask.h"
#include "configurationdata.h"
#include "logger/log.h"
#include <QDebug>

namespace logic
{
OpenDialog::OpenDialog(db::Postgres * const db,
                       QObject *const parent)
    : QObject{parent},
    m_Db(db)
{}


OpenDialog::~OpenDialog()
{

}

void OpenDialog::init() noexcept
{

}

void OpenDialog::readTaskList()
{
    db::Result<db::GenericData> result = m_Db->readAllTask();

    if (db::ErrorCode::NO_ERROR == result.error())
    {
        if (result.data().length() > 0)
        {
            for (auto & subtask: result.data())
            {
                subtask.setContent(cus::toStr(cus::TaskType::SubTask));
                sendDataTask(subtask);
            }
        }
    }
    else
    {
        logger::log.error(rc::RetCode::ReadTaskListFail, {"POSTGRES"});

    }

}

void OpenDialog::sendDataTask(const db::GenericData &data)
{
    const QString id = data[0];
    const QString name = data[1];
    const QString model = data[2];
    const QString type = data.content();
    const QString comment = data[3];
    const QString status = data[4];
    const QString author = "Daniel";
    const QString date = data[6];


    emit sendTask(id, name,model,type,comment,status,author,date);
}

void OpenDialog::openTaskDb(const QString &id)
{
    db::Result<db::Subtasks> subtask = m_Db->getTask(id);

    if (db::ErrorCode::NO_ERROR == subtask.error())
    {
        cus::currentTask.setSubtask(subtask.data().at(0));
        cus::currentTask.setStatus(cus::TaskStatus::BeingEdited);
        emit openTask();
    }
    else
    {
        logger::log.error(rc::RetCode::OpenTaskFail, {"POSTGRES"});
    }
}

void OpenDialog::readRecentTasks()
{
    (void)config::data.readRecentTasks();
    auto recentTasks = config::data.getRecentTasks();

    for (auto const &task : recentTasks)
    {
        const db::Result<db::GenericData> taskDb = m_Db->getTaskByName(task.name);
        if (db::ErrorCode::NO_ERROR == taskDb.error())
        {
            if (taskDb.data().length() > 0)
            {
                db::GenericData taskFinal = taskDb.data().at(FIRST);
                taskFinal.setContent(cus::toStr(cus::TaskType::SubTask));
                sendDataTask(taskFinal);
            }
        }
        else
        {
            logger::log.error(rc::RetCode::ReadTaskFail, {"POSTGRES"});
        }
    }
}


} //namespace logic

