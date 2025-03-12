
#include <thread>
#include "steplist.h"
#include "constants.h"
#include "custom/enums/convertions.h"
#include "custom/utils.h"

#include "logger/log.h"
#include "custom/structures/taskbasicinfo.h"
#include "custom/commonsignals.h"
#include "currenttask.h"
#include "postgres.h"
#include "stepexecutions.h"

namespace logic
{

const QStringList StepList::EMPTY_SL_TEXT = {cus::EMPTY_SL, cus::EMPTY_SL};

StepList::StepList(db::Postgres * const db,
                   QObject * const parent)
  : QObject{parent},
    m_Db(db)
{
    m_Dll = new QLibrary("DLL");

    if (!m_Dll->load())
    {
        qWarning() << "fallo al cargar la librería";
    }
    else
    {
        qWarning() << "librerías cargadas correctamente";

        m_SetTransformPosition = (SetTransformPosition)m_Dll->resolve("SetTransformPosition");

        m_IncrementXPosition = (incrementXPosition)m_Dll->resolve("incrementXPosition");
        m_IncrementYPosition = (incrementYPosition)m_Dll->resolve("incrementYPosition");
        m_IncrementZPosition = (incrementZPosition)m_Dll->resolve("incrementZPosition");
        m_GetPositionX = (GetPositionX)m_Dll->resolve("GetPositionX");
        m_GetPositionY = (GetPositionY)m_Dll->resolve("GetPositionY");
        m_GetPositionZ = (GetPositionZ)m_Dll->resolve("GetPositionZ");
        m_saveLog = (saveLog)m_Dll->resolve("saveLog");
        if (!m_SetTransformPosition || !m_IncrementXPosition || !m_GetPositionX || !m_GetPositionY || !m_GetPositionZ) {
            qWarning() << "No se pudieron resolver las funciones de la DLL!";

        }
    }
}

StepList::~StepList()
{
}
void StepList::getStepList()
{
  const db::Result<db::Steps> stepsResult = m_Db->getSteps(cus::currentTask.id());

  QList<cus::STEP_INFO> stepList;

  if (db::ErrorCode::NO_ERROR == stepsResult.error())
  {
   for (auto const &step: stepsResult.data())
    {
       cus::STEP_INFO stepRow;
       stepRow.id = step.id();
       stepRow.slText = step.slText();
       stepRow.executionOrder = step.executionOrder();
       stepList.append(stepRow);
    }
    emit fillSteps(stepList);
   }
   else
   {
      logger::log.error(rc::RetCode::StepsReadError,{"POSTGRES", cus::currentTask.id()});
  }

  emit updateStatusUI();
}

void StepList::addNewStep(const cus::STEP_INFO & step)
{
  const db::Error updateStepsError =
       m_Db->updateStepsOrderAfterStepAddition(cus::currentTask.id(), step.executionOrder);

  if (db::ErrorCode::NO_ERROR == updateStepsError)
  {
     const QString ID = cus::DEFAULT_STR;
     const QString comment = cus::EMPTY_STRING;
     const db::Steps stepData(ID, cus::currentTask.id(), step.executionOrder, comment, step.slText);
     const db::Error insertStepError = m_Db->insertStep(stepData);

     if (db::ErrorCode::NO_ERROR == insertStepError)
     {
       if (EMPTY_SL_TEXT.join(SL_COMMANDS_DELIM) == step.slText)
       {
             logger::log.info(rc::RetCode::StepAddedSuccessfully,{cus::currentTask.name(),step.executionOrder});
         getStepList();
         emit stepsModified();
       }
     }
     else
     {
       logger::log.error(rc::RetCode::WriteError);
     }
    }
}

void StepList::deleteSelectedStep(const cus::STEP_INFO & step)
{
  const db::Error deleteStepError = m_Db->deleteStep(cus::currentTask.id(), step.executionOrder);

  if (db::ErrorCode::NO_ERROR == deleteStepError)
  {
    const db::Error updateStepsError =
         m_Db->updateStepsOrderAfterStepDeletion(cus::currentTask.id(), step.executionOrder);

     if (db::ErrorCode::NO_ERROR == updateStepsError)
     {
       if (EMPTY_SL_TEXT.join(SL_COMMANDS_DELIM) == step.slText)
       {
             logger::log.info(rc::RetCode::EmptyStepDeletedSuccessfully,{cus::currentTask.name(),step.executionOrder});
         emit stepsModified();
       }
       else
       {
           logger::log.info(rc::RetCode::StepDeletedSuccessfully,{cus::currentTask.name(),step.executionOrder});
       }
     }
     else
     {
       logger::log.error(rc::RetCode::WriteError);
     }
    }
}
void StepList::createStepExecution(cus::STEP_INFO currentStep, bool mode)
{
    const QString userID = "1";

    const db::StepExecutions dbStepExecutions("DEFAULT",
                                              currentStep.id,
                                              cus::currentTask.subtaskExecID(),
                                              currentStep.executionOrder,
                                              cus::toStr(cus::ExecutionStatus::BeingExecuted),
                                              userID);


    db::Result<db::StepExecutions> const result = m_Db->insertStepExecution(dbStepExecutions);

    if (db::ErrorCode::NO_ERROR == result.error() && (false == result.data().isEmpty()))
    {
        QString stepExecutionID = result.data().at(FIRST).id();
        logger::log.info(rc::RetCode::StepExecutionStatusChange,{stepExecutionID,cus::toStr(cus::ExecutionStatus::BeingExecuted)});
        const QString action = currentStep.slText.split("   ")[1];
        const QString axis = action.split("_")[1];
        const QString increment = action.split("_")[2];
        if (axis == "X")
        {
            m_IncrementXPosition(increment.toFloat());
        }
        else if (axis == "Y")
        {
            m_IncrementYPosition(increment.toFloat());
        }
        else if (axis == "Z")
        {
            m_IncrementZPosition(increment.toFloat());
        }
        else
        {

        }
        emit createdSuccessfully(stepExecutionID, mode);
    }
    else
    {
        logger::log.error(rc::RetCode::StepExecutionCreateError,{currentStep.id});
    }
}

void StepList::autoMode()
{
    const db::Result<db::Steps> stepsResult = m_Db->getSteps(cus::currentTask.id());
    bool autoMode = true;
    QList<cus::STEP_INFO> stepList;

    if (db::ErrorCode::NO_ERROR == stepsResult.error())
    {
        for (auto const &step: stepsResult.data())
        {
            cus::STEP_INFO stepRow;
            stepRow.id = step.id();
            stepRow.slText = step.slText();
            stepRow.executionOrder = step.executionOrder();
            stepList.append(stepRow);
        }
    }
    else
    {
        logger::log.error(rc::RetCode::StepsReadError,{"POSTGRES", cus::currentTask.id()});
    }

    for (int32_t i = 0; i < stepList.length(); i++)
    {
        createStepExecution(stepList[i], true);

        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    emit finishedAutoMode();
}

} //namespace logic
