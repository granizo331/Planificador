#pragma once

#include "db/genericdata.h"
#include <QObject>

namespace db
{
class Postgres;
}

namespace logic
{
class Planificator;

class StartExecutionDialog : public QObject
{
    Q_OBJECT
public:
    //Constructor
    explicit StartExecutionDialog(db::Postgres * const db,
                        QObject * const parent = nullptr);

    //Destructor
    ~StartExecutionDialog();

    //Inicialización
    void init() noexcept;
signals:
    //Señal para indicar que no se puede crear la ejecución por nombre repetido
    void duplicateName();
    //Señal para informar a la parte gráfica que se ha creado el tasklog correctamente
    void createdTasklog();
public slots:
    //Funcion para crear el tasklog
    void createTasklog(const QString &name);
private:
    //Puntero a la BBDD
    db::Postgres * const m_Db;
};





} //namespace logic
