#pragma once

#include <QDialog>
#include <QThread>

namespace Ui
{
class CreateDialogUI;
}

namespace db
{
class Postgres;
}

namespace logic
{
class Planificator;
class CreateDialog;
}

class CreateDialogUI : public QDialog
{
    Q_OBJECT
public:
    //Constructor
    explicit CreateDialogUI(db::Postgres * const db,
                            QWidget *parent = nullptr);
    //Destructor
    ~CreateDialogUI();

    //Inicialización
    void init() noexcept;
signals:
    //Señal para guaradar la tarea en BBDD
    void saveTaskDb(const QString &name,const QString &model, const QString &comment);
private slots:
    //Crear tarea
    void saveTask();
private:

    //Objeto de la parte gráfica
    Ui::CreateDialogUI *ui;
    //Objeto de la lógica
    logic::CreateDialog *m_logic;
    //Thread de la lógica
    QThread m_logicTh;
    //Miembros de la clase
    QString m_taskName;
    QString m_comment;
    //Puntero a la BBDD
    db::Postgres * const m_Db;
};

