#include "stepdialog.h"
#include "postgres.h"
#include "slcommandslibrary.h"
#include "logger/log.h"


namespace logic
{
StepDialog::StepDialog(cus::STEP_INFO & stepData,
                       const QString &model,
                       db::Postgres * const db,
                       stcomm::StepsLibrary * const library,
                       QObject *parent)
    : QObject{parent},
    m_Db(db),
    m_library(library),
    m_stepData(&stepData),
    m_model(model)
{}

StepDialog::~StepDialog()
{

}

void StepDialog::init() noexcept
{

    getStepInfo();
    getStepsLibrary();

    getStepMultimedias();
    getActions(m_model,false);

    emit displayInfoInDialog();

}


void StepDialog::getStepInfo() noexcept
{
    const db::Result<db::Steps> step = m_Db->getStep(m_stepData->id);

    if (db::ErrorCode::NO_ERROR == step.error())
    {
        for (auto const &st : step.data())
        {
            m_stepData->comment = st.comment();
            m_stepData->slText = st.slText();
            m_stepData->executionOrder = st.executionOrder();
            m_stepData->subtaskID = st.subtaskID();
        }
    }
    else
    {
        logger::log.error(rc::RetCode::StepReadError,{"POSTGRES",m_stepData->id});
    }

}


void StepDialog::getStepsLibrary()
{

    const db::Result<db::SLCommandsLibrary> result = m_Db->getStepsLibrary();

    if (db::ErrorCode::NO_ERROR == result.error())
    {
        *m_library = stcomm::StepsLibrary(result.data());
    }
}

void StepDialog::getActions(const QString &subject, const bool &init)
{
    QStringList actionsList = m_library->getActionsStringList(subject);

    //TODO: aqui se comprueba si es nuevo o no el subject
    if(false == m_library->isSubjectPredefined(subject))
    {
        actionsList.insert(ZERO, cus::EMPTY_SL);
    }
    else if(false == actionsList.isEmpty())
    {
        actionsList.insert(ZERO, cus::EMPTY_SL);
    }
    else{}

    const QSet auxSet = QSet(actionsList.begin(), actionsList.end());

    QStringList actionsValues = auxSet.values();
    actionsValues.sort();
    emit sendActions(actionsValues, init);
}

void StepDialog::saveChanges()
{

    const db::Steps stepDbData(m_stepData->id,
                               m_stepData->subtaskID,
                               m_stepData->executionOrder,
                               m_stepData->comment,
                               m_stepData->slText);

    const db::Error updateStepError = m_Db->updateStep(stepDbData);
    if(db::ErrorCode::NO_ERROR != updateStepError)
    {
        logger::log.error(rc::RetCode::StepEditedFailed, {"POSTGRES",m_stepData->slText});
    }

    emit changesSaved();
}
void StepDialog::getStepMultimedias()
{
    qWarning() << "get step mutlimedias luego se hace";
}

} // namespace logic
