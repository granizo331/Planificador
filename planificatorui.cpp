#include "planificatorui.h"
#include "./ui_planificatorui.h"
#include "startexecutiondialogui.h"
#include "threads.h"
#include "dialogs/openDialog/opendialogui.h"
#include "dialogs/createDialog/createdialogui.h"
#include "dialogs/modelDialog/modeldialogui.h"
#include "dialogs/stepDialog/stepdialogui.h"
#include "custom/currenttask.h"
#include "QDebug"
PlanificatorUi::PlanificatorUi(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PlanificatorUi),
    m_planificator(nullptr)
{
    ui->setupUi(this);
}

PlanificatorUi::~PlanificatorUi()
{
    delete ui;
    delete m_planificator;
    threads::deleteThread(m_logicTh);

}

//Funcion para inicializar el UI
bool PlanificatorUi::init() noexcept
{
    m_planificator = new logic::Planificator();
    m_planificator->moveToThread(&m_logicTh);

    (void)connect(&m_logicTh, &QThread::finished, m_planificator, &QObject::deleteLater);
    m_logicTh.start();
    m_planificator->init();

    (void)connect(this, &PlanificatorUi::readTaskFromDb, m_planificator, &logic::Planificator::readTaskFromDb);

    (void)connect(m_planificator,&logic::Planificator::updateView, this, &PlanificatorUi::updateView);

    ui->stepList->init(m_planificator->getdb());

    return true;
}

//Funcion para hacer el setup del UI
void PlanificatorUi::setupUi() noexcept
{
    QHBoxLayout *layout = new QHBoxLayout();

    layout->addWidget(ui->stepArea);

    layout->addWidget(ui->stepList);

    ui->centralwidget->setLayout(layout);

    ui->messageDisplayDock->setAllowedAreas(Qt::RightDockWidgetArea);
    ui->messageDisplayDock->setWidget(ui->messageDisplay_3);

    addDockWidget(Qt::RightDockWidgetArea,ui->messageDisplayDock);
    (void)connect(ui->actioncreate, &QAction::triggered, this, &PlanificatorUi::createTask);
    (void)connect(ui->actionOpentask, &QAction::triggered, this, &PlanificatorUi::openTask);
    (void)connect(ui->actionClosetask, &QAction::triggered, this , &PlanificatorUi::closeTask);
    (void)connect(ui->actionExecutetask, &QAction::triggered, this, &PlanificatorUi::executeTask);
    (void)connect(ui->actionList, &QAction::triggered, this, &PlanificatorUi::modelList);

    (void)connect(ui->stepList, &StepListUI::editStepDialog, this, &PlanificatorUi::editStepDialog);
    updateView();
    show();
}

//Funcion que actualiza la vista de la ventana principal
void PlanificatorUi::updateView()
{
    if (cus::currentTask.isEmpty())
    {
        ui->actioncreate->setEnabled(true);
        ui->actionOpentask->setEnabled(true);
        ui->actionClosetask->setEnabled(false);
        ui->actionExecutetask->setEnabled(false);
        ui->centralwidget->setEnabled(false);
    }
    else
    {
        ui->actioncreate->setEnabled(false);
        ui->actionOpentask->setEnabled(false);
        ui->actionClosetask->setEnabled(true);
        ui->actionExecutetask->setEnabled(true);
        ui->centralwidget->setEnabled(true);
    }
}

//Funcion para crear una tarea
void PlanificatorUi::createTask()
{
    CreateDialogUI * const createTaskDialog = new CreateDialogUI(m_planificator->getdb(),this);
    createTaskDialog->init();
}

//Funcion para abrir una tarea
void PlanificatorUi::openTask()
{
    OpenDialogUI * openTaskDialog = new OpenDialogUI(this);
    openTaskDialog->init(m_planificator->getdb());

    if (!openTaskDialog->result())
    {
        if (cus::DEFAULT_STR != cus::currentTask.id())
        {
            emit readTaskFromDb();
            ui->stepList->initStepList();
        }
    }

    delete openTaskDialog;
}

//Funcion para lista de modelos
void PlanificatorUi::modelList()
{
    ModelDialogUI * modelDialog = new ModelDialogUI(this);
    modelDialog->init(m_planificator->getdb());
}

//Funcion para editar una tarea
void PlanificatorUi::editStepDialog(const QString &stepID, const bool readOnly)
{

    const QString model = cus::currentTask.model();
    StepDialogUI * const editStep = new StepDialogUI(stepID,model,m_planificator->getdb(), this);

    (void)connect(editStep, &StepDialogUI::dialogClosed, ui->stepList, &StepListUI::updateStepAfterEdit);
    editStep->init();

    delete editStep;

}

//Funcion para cerrar una tarea
void PlanificatorUi::closeTask()
{
    cus::currentTask.clear();
    ui->stepList->closeAndClear();
    updateView();
    if (QFile::exists("/home/daniel/Escritorio/transform_log.txt"))
    {
        if (QFile::remove("/home/daniel/Escritorio/transform_log.txt"))
        {
            qWarning() << "log borrado satisfactoriamente";
        }
        else
        {
            qWarning() << "fallo al borrar el log";
        }
    }


}

//Funcion para ejecutar una tarea
void PlanificatorUi::executeTask()
{
    if (ui->stepList->isEmpty())
    {
        logger::log.error(rc::RetCode::NoSteps);
    }
    else
    {
        ui->actionExecutetask->setEnabled(false);
        StartExecutionDialogUI * startExecution = new StartExecutionDialogUI(this);
        startExecution->init(m_planificator->getdb());

        if (!cus::currentTask.tasklogID().isEmpty())
        {
            ui->stepList->executionView();
        }
    }
}


