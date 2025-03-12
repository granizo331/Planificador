#pragma once

#include "custom/enums/taskstatus.h"
#include "custom/structures/stepinfo.h"
#include <QObject>
#include <QLibrary>

typedef void (*SetTransformPosition)(float,float,float);
typedef void (*incrementXPosition)();
typedef float (*GetPositionX)();
typedef float (*GetPositionY)();
typedef float (*GetPositionZ)();

namespace cus
{
class TASK_BASIC_INFO;
}

namespace db
{
class Postgres;
}

namespace logic
{

class Builder;

class StepList : public QObject
{
  Q_OBJECT
public:
  friend class StepListUI;
  //Constructor
  StepList(db::Postgres * const db,
           QObject* const parent = nullptr);
  //Destructor
  ~StepList();
  StepList(StepList const&) = delete;
  void operator=(StepList const&) = delete;
  static const QStringList EMPTY_SL_TEXT;

public slots:
  //Leer la lista de pasos de la BBDD
  void getStepList();
  //Añadir nuevo paso
  void addNewStep(const cus::STEP_INFO & step);
  //Borrar el paso actual
  void deleteSelectedStep(const cus::STEP_INFO & step);
  //Crear la ejecución de un paso
  void createStepExecution(cus::STEP_INFO currentStep, bool mode);
  //Modo automático de ejecución
  void autoMode();

signals:
  //Señal para avisar a la parte gráfica de que el modo automático
  // ha acabado
  void finishedAutoMode();
  //Señal para indicar que una ejecución de un paso se ha creado correctamente
  void createdSuccessfully(const QString &stepExecutionID, bool mode);
  //Señal para actualizar el estado de la parte gráfica
  void updateStatusUI();
  //Señal para indicar que un paso ha sido seleccionado
  void stepSelected(const QString &stepID, const QString &stepName,
                    const cus::TaskStatus methodStatus=cus::TaskStatus::None);
  //Señal para enviar la informaicón de un paso determinado
  void stepInfoReceived(const cus::STEP_INFO &stepData, const QStringList &multimedia);
  //Señal para indicar que un paso ha sido modificado
  void stepsModified();
  //Señal para rellenar la lista de pasos
  void fillSteps(const QList<cus::STEP_INFO> & stepList);
private:
  //Puntero a la BBDD
  db::Postgres * m_Db;
  //Puntero a la DLL
  QLibrary * m_Dll;

  //Resolver las funciones de la DLL
  SetTransformPosition m_SetTransformPosition;
  typedef void (*incrementXPosition)(float increment);
  typedef void (*incrementYPosition)(float increment);
  typedef void (*incrementZPosition)(float increment);
  incrementXPosition m_IncrementXPosition;
  incrementYPosition m_IncrementYPosition;
  incrementZPosition m_IncrementZPosition;
  GetPositionX m_GetPositionX;
  GetPositionY m_GetPositionY;
  GetPositionZ m_GetPositionZ;
  typedef void (*saveLog)();
  saveLog m_saveLog;

  //Miembros de clase

  static constexpr int32_t POS_ID = 0;
  const QString SL_COMMANDS_DELIM = "    ";
};

} //namespace logic
