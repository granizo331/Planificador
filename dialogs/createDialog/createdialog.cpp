#include "createdialog.h"
#include <QDebug>
#include "db/postgres/postgres.h"
#include "logger/log.h"


namespace logic
{
CreateDialog::CreateDialog(db::Postgres * const db,
                           QObject *const parent)
    : QObject{parent},
    m_Db(db)
{}

CreateDialog::~CreateDialog()
{

}

void CreateDialog::init() noexcept
{

}

//Crear tarea y guardarla en BBDD
void CreateDialog::saveTaskDb(const QString &name, const QString &model,const QString &comment)
{

    const db::Result<db::GenericData> result = m_Db->createSubtask(name,model,comment);

    if (db::ErrorCode::NO_ERROR == result.error())
    {
        logger::log.info(rc::RetCode::CreateTask);
    }
    else
    {
        logger::log.error(rc::RetCode::CreateTaskError,{"POSTGRES"});
    }
}

} //namespace logic
