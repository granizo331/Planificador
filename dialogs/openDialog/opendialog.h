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

class OpenDialog : public QObject
{
    Q_OBJECT
public:

    //Constructor
    explicit OpenDialog(db::Postgres * const db,
                        QObject * const parent = nullptr);

    //Destructor
    ~OpenDialog();

    //Inicialización
    void init() noexcept;

signals:
    //Señal para enviar una tarea a la lógica
    void sendTask(const QString &id, const QString &name, const QString &model,
                  const QString &type,const QString &comment, const QString &status,
                    const QString &author, const QString &date);

    //Señal para abrir una tarea
    void openTask();
public slots:
    //Leer tareas
    void readTaskList();
    //Enviar información de la tarea
    void sendDataTask(const db::GenericData &data);
    //Abrir tarea
    void openTaskDb(const QString &id);
    //Leer tareas recientes
    void readRecentTasks();
private:
    //Puntero a la BBDD
    db::Postgres * const m_Db;
};

} //namespace logic
