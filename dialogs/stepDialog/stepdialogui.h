#pragma once

#include "stepsLibrary/stepslibrary.h"
#include "structures/stepinfo.h"

#include <QDialog>
#include <QThread>

namespace db
{
class Postgres;
}
namespace Ui
{
class StepDialogUI;
}

namespace logic
{
class Planificator;
class StepDialog;
}


class StepDialogUI : public QDialog
{
    Q_OBJECT
public:
    //Constructor
    explicit StepDialogUI(const QString &stepID,
                          const QString &model,
                          db::Postgres * const db,
                          QWidget * const parent = nullptr);
    //Destructor
    ~StepDialogUI();

    //Inicialización
    void init() noexcept;
public slots:
    //Funcion para añadir multimedias a un paso
    void addMultimedia();
    //Funcion para añadir una accion
    void addAction();
    //Funcion para mostrar la información
    void displayInfoInDialog();
    //Funcion que actualiza las acciones
    void updateActions(const QStringList &actions, const bool init);
    //Funcion para actualizar el area de pasos
    void updateSlCommandsArea();
    //Funcion para guardar el paso
    void saveStep();
    //Funcion que devuelve el texto del paso actual
    QString getCurrentStepText();
    //Funcion llamada al cerrar el diálogo
    void closeDialog();
signals:
    //Señal para guardar cambios
    void saveChanges();

    //Señal para indicar que el diálogo se ha cerrado
    void dialogClosed(const int32_t &row, const QStringList &stepText);
private:
    //Puntero de la parte gráfica
    Ui::StepDialogUI *ui;
    //Puntero de la parte lógica
    logic::StepDialog *m_logic;
    //Puntero a la BBDD
    db::Postgres * const m_Db;
    //Thread de la lógica
    QThread m_logicTh;
    //Objeto de la librería de pasos
    stcomm::StepsLibrary m_library;

    //Miembros de la clase
    cus::STEP_INFO m_stepData;
    const QString SL_COMMANDS_DELIM = "    ";
    QString m_model;
    const QString MODEL_1 = "MODEL1";
};

