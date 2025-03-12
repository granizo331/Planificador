#pragma once

#include <QObject>
#include "db/postgres/postgres.h"

namespace db
{
class Postgres;
}
namespace logic
{
class Planificator : public QObject
{
    Q_OBJECT
public:
    //Constructor de la lógica
    explicit Planificator(QObject *parent = nullptr);
    //Inicialización de la lógica
    void init() noexcept;
    //Inicialización de la BBDD
    db::Postgres * getdb() const {return m_postgresDb;};
signals:

    //Señal para cambiar de vista de la ventana principal
    void updateView();
public slots:
    //Leer tarea de la BBDD
    void readTaskFromDb();
private:
    //Crear conexiones de la BBDD
    void createDbConnections();
    //Puntero al objeto de la BBDD
    db::Postgres * m_postgresDb;
};

} //namespace logic

