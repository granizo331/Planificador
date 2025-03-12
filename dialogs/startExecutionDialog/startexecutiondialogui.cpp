#include "startexecutiondialogui.h"
#include "startexecutiondialog.h"
#include "ui_startexecutiondialogui.h"
#include "../../threads.h"
#include "custom/currenttask.h"

StartExecutionDialogUI::StartExecutionDialogUI(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::StartExecutionDialogUI)
{
    ui->setupUi(this);
}

StartExecutionDialogUI::~StartExecutionDialogUI()
{
    delete ui;
    delete m_logic;
    threads::deleteThread(m_logicTh);
}

void StartExecutionDialogUI::init(db::Postgres * const db) noexcept
{

    m_logic = new logic::StartExecutionDialog(db);
    m_logic->moveToThread(&m_logicTh);
    (void)connect(&m_logicTh,&QThread::finished,m_logic, &QObject::deleteLater);

    (void)connect(ui->startButton, &QPushButton::clicked, this, &StartExecutionDialogUI::startButton);
    (void)connect(ui->cancelButton, &QPushButton::clicked, this, &StartExecutionDialogUI::close);

    (void)connect(this, &StartExecutionDialogUI::createTasklog, m_logic, &logic::StartExecutionDialog::createTasklog);
    (void)connect(m_logic, &logic::StartExecutionDialog::createdTasklog, this, &StartExecutionDialogUI::createdTasklog);
    m_logicTh.start();
    m_logic->init();



    ui->cancelButton->setCustomStyle(cus::ButtonStyle::CancelDelete);
    ui->startButton->setCustomStyle(cus::ButtonStyle::Default);

    ui->taskName->setText(cus::currentTask.name());

    setTaskLogName();

    exec();

}

void StartExecutionDialogUI::setTaskLogName()
{
    const QString time = cus::getDate(cus::DATEFORMAT_YYYYMMDD_HYPHEN) + cus::UNDERSCORE_STR +
                         cus::getTime(cus::TIMEFORMAT_HHMMSS_SEP);

    QString type = cus::SUBTASK_LOGNAME_STR;

    const QString logName = time + cus::UNDERSCORE_STR + type + cus::UNDERSCORE_STR + cus::currentTask.name();

    ui->taskLog->setText(logName);
}

void StartExecutionDialogUI::startButton()
{
    emit createTasklog(ui->taskLog->text());
}

void StartExecutionDialogUI::createdTasklog()
{
    setEnabled(true);
    (void)close();
}
