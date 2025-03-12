#pragma once

#include <QObject>
#include "stepsLibrary/stepslibrary.h"
#include "custom/structures/stepinfo.h"
namespace db
{
class Postgres;
}
namespace logic
{
class StepDialog : public QObject
{
    Q_OBJECT
public:
    //Constructor
    explicit StepDialog(cus::STEP_INFO & stepData,
                        const QString &model,
                        db::Postgres * const db,
                        stcomm::StepsLibrary * const library,
                        QObject *parent = nullptr);
    //Destructor
    ~StepDialog();
    //Inicialización
    void init() noexcept;

signals:
    //Señal para mostrar la información
    void displayInfoInDialog();
    //Señal para enviar los sujetos
    void sendSubjects(const QStringList &subjects, const bool init);
    //Señal para enviar las acciones
    void sendActions(const QStringList &actions, const bool init);
    //Señal para mostrar los cambios guardados
    void changesSaved();
public slots:
    //Función para obtener la información de un paso
    void getStepInfo() noexcept;
    //Funcion para obtener la librería de pasos
    void getStepsLibrary();
    //Funcion para obtener los archivos multimedia de un paso
    void getStepMultimedias();
    //Funcion para obtener las acciones
    void getActions(const QString &subject, const bool &init);
    //Funcion para guardar los camios
    void saveChanges();
private:
    //Puntero a la BBDD
    db::Postgres * const m_Db;
    //Puntero a la librería de apsos
    stcomm::StepsLibrary *m_library;
    //Puntero a la información del paso actual
    cus::STEP_INFO * const m_stepData;
    //Miembro de clase
    const QString m_model;
};

} //namespace db
