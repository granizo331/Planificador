#pragma once

#include <QDialog>
#include <QThread>
namespace Ui
{
class OpenDialogUI;
}

namespace db
{
class Postgres;
}

namespace logic
{
class Planificator;
class OpenDialog;
}

class OpenDialogUI : public QDialog
{
    Q_OBJECT
public:
    //Constructor
    explicit OpenDialogUI(QWidget *parent = nullptr);

    //Destructor
    ~OpenDialogUI();

    //Inicialización
    void init(db::Postgres * const db) noexcept;

signals:
    //Señal para leer la lista
    void readList();
    //Señal para obtener la información de la tarea de BBDD
    void openTaskDb(const QString &id);
    //Señal para leer lista de tareas recientes
    void readRecentTasks();
private slots:
    //Funcion para abrir una tarea
    void openTask();
    //FUncion para leer la lista de tareas
    void readTaskList();
    //Fucnion para mostrar una tarea en la lista
    void drawTask(const QString &id, const QString &name, const QString &model,
                  const QString &type, const QString &comment, const QString &status, const QString &author, const QString &date);
    //Funcion para abrir uan tarea satisfactoriamente
    void openTaskSuccess();

    //Funciones propias de la tabla de tareas
    void itemSelectionChanged();
    void selectorChanged(const QString &text);
private:

    //Puntero de la parte gráfica
    Ui::OpenDialogUI *ui;
    //Puntero de la lógica
    logic::OpenDialog *m_logic;
    //Thread de la parte lógica
    QThread m_logicTh;
};

