#pragma once

#include <QObject>


namespace db
{
class Postgres;
}
namespace logic
{
class Planificator;

class CreateDialog : public QObject
{
    Q_OBJECT
public:
    //Constructor
    explicit CreateDialog(db::Postgres * const db,
                          QObject * const parent = nullptr);
    //Destructor
    ~CreateDialog();

    //Inicialización de la lógica
    void init() noexcept;
public slots:

    //Guardar tarea en BBDD
    void saveTaskDb(const QString &name, const QString &model, const QString &comment);


signals:
private:

    //Puntero a la BBDD
    db::Postgres * const m_Db;
};

}   //namespace logic


