#include "createdialogui.h"
#include "createdialog.h"
#include "ui_createdialogui.h"
#include "../../threads.h"
#include "logger/log.h"


CreateDialogUI::CreateDialogUI(db::Postgres * const db,
                               QWidget *parent)
    : QDialog(parent),
    ui(new Ui::CreateDialogUI),
    m_Db(db)
{
    ui->setupUi(this);
}

CreateDialogUI::~CreateDialogUI()
{
    delete ui;
    delete m_logic;
    threads::deleteThread(m_logicTh);

}

void CreateDialogUI::init() noexcept
{
    m_logic = new logic::CreateDialog(m_Db);
    m_logic->moveToThread(&m_logicTh);
    (void)connect(&m_logicTh,&QThread::finished, m_logic, &QObject::deleteLater);
    (void)connect(ui->cancelButton, &QPushButton::clicked, this, &CreateDialogUI::close);
    (void)connect(ui->saveButton,&QPushButton::clicked,this,&CreateDialogUI::saveTask);
    (void)connect(this, &CreateDialogUI::saveTaskDb, m_logic, &logic::CreateDialog::saveTaskDb);
    m_logicTh.start();
    m_logic->init();
    ui->modelCombo->setEnabled(false);
    (void)exec();
}

//Guardar tarea
void CreateDialogUI::saveTask()
{
    if (ui->nameLineEdit->text().isEmpty())
    {
        logger::log.error(rc::RetCode::EmptyName);
    }
    else
    {
        m_taskName = ui->nameLineEdit->text();
        m_comment = ui->commentText->toPlainText();
        QString model;
        if (ui->modelCombo->currentText() == "Modelo 1")
        {
            model = "1";
        }
        else if (ui->modelCombo->currentText() == "Modelo 2")
        {
            model = "2";
        }
        else if (ui->modelCombo->currentText() == "Modelo 3")
        {
            model = "3";
        }


        emit saveTaskDb(m_taskName,model, m_comment);
    }

    close();
}
