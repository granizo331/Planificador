#pragma once

#include "custom/enums/taskstatus.h"
#include "custom/structures/stepinfo.h"
#include "custom/structures/taskbasicinfo.h"
#include <QWidget>
#include <QThread>

namespace cus
{
class MAP_METHOD;
}

class QTableWidgetItem;

namespace db
{
class Postgres;
}

namespace logic
{
  class Planificator;
  class StepList;
}

namespace Ui
{
  class StepListUI;
}

class StepListUI : public QWidget
{
  Q_OBJECT
public:

  //Constructor
  explicit StepListUI(QWidget* const parent = nullptr);
  //Destructor
  ~StepListUI();
  //Inicialización
  void init(db::Postgres * const db);
  //Iniciar lista de pasos
  void initStepList();
  //Cerrar lista
  void closeAndClear();
  //Comprueba si un paso está vacío o no
  bool isEmpty();

signals:

  //Señal para obtener la lista de pasos
  void getStepList();
  //Señal para crear la ejecución de un paso
  void createStepExecution(cus::STEP_INFO currentStep, bool mode);
  //Señal para el modo automático
  void autoMode();

  //Señal para informar de que un paso se ha seleccionado
  void stepSelected(const QString &stepID, const QString &stepName, cus::TaskStatus methodStatus=cus::TaskStatus::None);
  //Señal para añadir un paso
  void newStepAdded(const cus::STEP_INFO & step);
  //Señal para borrar un paso
  void stepDeleted(const cus::STEP_INFO & step);
  //Señal para editar un paso
  void editStepDialog(const QString & stepID,
                      const bool readOnly);
  //Señal para previsualizar multimedia
  void requestPrevisualizeMultimedia(const QString &multimediaName);
  //Señal para modificar un paso
  void stepsModified();

public slots:

  //Funcion para la ejecución del paso actual
  void setStepExecution(const QString &stepExecutionID, bool mode);
  //Funcion para cambiar la lista de pasos una vez haya finalizado el modo automático
  void finishedTaskAuto();

  //Función para el modo automático
  void autoModeButtonClicked();
  //Funcion para limpiar la lista
  void clearList();
  //Funcion para rellenar la lista con pasos
  void fillSteps(const QList<cus::STEP_INFO> & stepList) noexcept;

  //Funcion para resetear la lista
  void resetList();
  //Funcion que actualiza un paso después de ser editado
  void updateStepAfterEdit(const int32_t &row, const QStringList &slText);
  //Funcion para el modo de vista de ejecución
  void executionView();

private slots:

  //Funcion llamada al deseleccionar un paso
  void deselectStep();

  //Funcion para borrar un paso
  void deleteStep();
  //Funcion para editar un paso
  void editStep();

  //Funcion para comenzar la ejecución de un paso
  void startStepExecution();

  //Funcion insertar un paso
  void insertStep();

  //Funcion para cambiar de fila en la lista
  void rowSelectionChanged(const QTableWidgetItem * const current);
  //Funcion para cambiar de fila en la lista durante la ejecución
  void rowSelectionChangedExecution(const QTableWidgetItem * const current) noexcept;
private:

  //Funcion para cambiar la funcionalidad de la barra de herramientas
  void updateToolbarStatus();
  //Funcion para el modo de vista solo lectura
  void readOnlyView();
  //Funcion para el modo de vista editar
  void editingView();
  //Funcion que rellena una fila con un paso
  void fillRow(const cus::STEP_INFO &stepRow);
  //Funcion que devuelve el texto del paso
  QString getStepText(const int32_t row);
  //Funcion para obtener el paso de la fila
  cus::STEP_INFO getStepFromRow(const int32_t row) noexcept;
  //Funcion para hacer resize del area
  void resizeEvent(QResizeEvent *event) override;
  //Puntero a la parte gráfica
  Ui::StepListUI *ui;
  //Thread de la lógica
  QThread m_logicTh;
  //Puntero a la lógica
  logic::StepList *m_logic;
  //Flag para el modo automático
  bool m_autoMode = false;
  //Puntero a la BBDD
  db::Postgres * m_Db;

  //Miembros de clase
  QString m_currentStepExecution;

  static constexpr int32_t COL_SUBJECT = 0;

  static constexpr int32_t COL_ACTION = 1;

  static constexpr int32_t COL_OBJECT = 2;

  static constexpr int32_t COL_QUALIFIER = 3;

  static constexpr int32_t COL_STEPID = 4;

  static const QString TOOLTIP_EDIT_STEP;

  static const QString TOOLTIP_INSERT_STEP;
  static const QString TOOLTIP_DELETE_STEP;
  const QString SL_COMMANDS_DELIM = "    ";

  bool m_executionMode = false;
};
