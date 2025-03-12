#pragma once
#include "planificator.h"

#include <QThread>
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class PlanificatorUi;
}
QT_END_NAMESPACE

class PlanificatorUi : public QMainWindow
{
    Q_OBJECT

public:

    //Constructor
    PlanificatorUi(QWidget *parent = nullptr);
    //Destructor
    ~PlanificatorUi();
    //Inicializador
    bool init() noexcept;
    //Setup del Ui
    void setupUi() noexcept;
public slots:
    //Actualizar vista
    void updateView();
signals:
    //Señal para leer una tarea de la BBDD
    void readTaskFromDb();
private slots:
    //Crear tarea
    void createTask();
    //Abrir tarea
    void openTask();
    //Cerrar tarea
    void closeTask();
    //Ejecutar tarea
    void executeTask();
    //Lista de modelos
    void modelList();
    //Editar pasos
    void editStepDialog(const QString &stepID, const bool readOnly);

private:
    //Puntero al Ui
    Ui::PlanificatorUi *ui;
    //Thread de la lógica
    QThread m_logicTh;
    //Puntero a la lógica
    logic::Planificator *m_planificator;
};

