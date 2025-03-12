#include "planificator.h"
#include "QDebug"
#include "currenttask.h"
#include "logger/log.h"
#include "structures/stepinfo.h"
#include "postgres.h"


namespace logic
{
Planificator::Planificator(QObject *parent)
    : QObject{parent}
{}

//Inicialización
void Planificator::init() noexcept
{
    createDbConnections();
}
//Crear conexiones a la BBDD
void Planificator::createDbConnections()
{
    m_postgresDb = new db::Postgres();

    const db::Error resPostgres = m_postgresDb->connect("127.0.0.1" ,"10000","oms","admin_oms","root");

    if (db::ErrorCode::NO_ERROR == resPostgres)
    {
        logger::log.info(rc::RetCode::ConnectionOk, {"Postgres"});
    }
    else
    {
        logger::log.error(rc::RetCode::ConnectionFailed, {"Postgres"});
        logger::log.debug(rc::RetCode::ConnectionFailed,{"Postgres"});
    }
}
//Leer tarea

void Planificator::readTaskFromDb()
{
    emit updateView();
}
} //namespace logic
