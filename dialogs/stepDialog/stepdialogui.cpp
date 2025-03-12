#include "stepdialogui.h"
#include "stepdialog.h"
#include "ui_stepdialogui.h"
#include "../../threads.h"
#include "custom/widgets/multimediaBrowser/multimediabrowserui.h"
#include "utils.h"

StepDialogUI::StepDialogUI(const QString &stepID,
                           const QString &model,
                           db::Postgres * const db,
                           QWidget *parent)
    : QDialog{parent},
    ui(new Ui::StepDialogUI),
    m_Db(db),
    m_library({})
{
    if (model == "1")
    {
        m_model = "MODEL1";
    }
    else if (model == "2")
    {
        m_model = "MODEL2";
    }
    else if (model == "3")
    {
        m_model = "MODEL3";
    }
    m_stepData.id = stepID;
    ui->setupUi(this);
}

StepDialogUI::~StepDialogUI()
{
    delete ui;
    delete m_logic;
    threads::deleteThread(m_logicTh);
}

void StepDialogUI::init() noexcept
{

    m_logic = new logic::StepDialog(m_stepData,m_model,m_Db,&m_library);
    m_logic->moveToThread(&m_logicTh);

    (void)connect(&m_logicTh, &QThread::finished, m_logic, &QObject::deleteLater);
    ui->saveButton->            setCustomStyle(cus::ButtonStyle::Default);
    ui->cancelButton->          setCustomStyle(cus::ButtonStyle::CancelDelete);
    ui->addMultimediaButton->   setCustomStyle(cus::ButtonStyle::IconOnlyDefault);
    ui->deleteMultimediaButton->setCustomStyle(cus::ButtonStyle::IconOnlyDelete);
    ui->addSubjectButton->      setCustomStyle(cus::ButtonStyle::IconOnlyDefault);
    ui->addActionButton->       setCustomStyle(cus::ButtonStyle::IconOnlyDefault);

    cus::Style::setIcon(ui->addMultimediaButton,    cus::Icons::Add);
    cus::Style::setIcon(ui->addSubjectButton,       cus::Icons::Add);
    cus::Style::setIcon(ui->addActionButton,        cus::Icons::Add);
    cus::Style::setIcon(ui->deleteMultimediaButton, cus::Icons::Delete);

    ui->subjectLine->setText(m_model);
    ui->subjectLine->setEnabled(false);

    (void)connect(ui->addMultimediaButton, &QPushButton::clicked, this, &StepDialogUI::addMultimedia);
    (void)connect(ui->multimediasList,        &QListWidget::itemDoubleClicked,
                   this, [this](const QListWidgetItem * const item)
                   {cus::previsualizeMultimedia(this, item->text(), ui->statusLabel);});

    (void)connect(ui->addActionButton, &QPushButton::clicked, this, &StepDialogUI::addAction);

    (void)connect(m_logic, &logic::StepDialog::displayInfoInDialog, this, &StepDialogUI::displayInfoInDialog);

    (void)connect(m_logic, &logic::StepDialog::sendActions, this, &StepDialogUI::updateActions);

    (void)connect(ui->cancelButton, &QPushButton::clicked, this, &StepDialogUI::close);

    (void)connect(ui->saveButton, &QPushButton::clicked, this, &StepDialogUI::saveStep);
    (void)connect(this, &StepDialogUI::saveChanges, m_logic, &logic::StepDialog::saveChanges);

    (void)connect(m_logic, &logic::StepDialog::changesSaved, this, &StepDialogUI::closeDialog);
    m_logicTh.start();
    m_logic->init();


}

void StepDialogUI::addMultimedia()
{
    QStringList selectedFiles;

    MultimediaBrowserUI multimediaBrowser(selectedFiles,this);

    multimediaBrowser.init(m_Db);

    for (auto const &mm : selectedFiles)
    {
        ui->multimediasList->addItem(mm);
    }
}

void StepDialogUI::addAction()
{
    const QString action = ui->addActionLineEdit->text();

    if (false == action.trimmed().isEmpty())
    {
        const QString text = action.toUpper();

        const int32_t index = ui->actionCombo->findText(text);

        if (-1 == index)
        {
            ui->actionCombo->addItem(text);
            ui->actionCombo->setCurrentIndex(ui->actionCombo->findText(text));

            ui->addActionLineEdit->clear();

            m_library.createNewAction(ui->subjectLine->text(),text);

            ui->addActionButton->setEnabled(false);
        }
    }
    else
    {

    }
}

void StepDialogUI::updateActions(const QStringList &actions, const bool init)
{
    const QStringList splittedSLCommand = m_stepData.slText.split(SL_COMMANDS_DELIM, Qt::SkipEmptyParts);
    (void)ui->actionCombo->blockSignals(true);

    ui->actionCombo->addItems(actions);

    const int32_t emptySlIndex = ui->actionCombo->findText(cus::EMPTY_SL);

    if((ui->actionCombo->count() > MIN_LIST_SIZE) && (INVALID_ID == emptySlIndex))
    {
        ui->actionCombo->addItem(cus::EMPTY_SL);
    }

    if (true == init)
    {

        const int64_t editedLength = splittedSLCommand.length();
        const int32_t index = ui->actionCombo->findText(splittedSLCommand[1]);

        if ((editedLength >= 2) && (INVALID_ID == index))
        {
            ui->actionCombo->addItem(splittedSLCommand[1]);
        }

        ui->actionCombo->setCurrentIndex
            (ui->actionCombo->findText(splittedSLCommand[1]));
    }
    else
    {
        if(ui->actionCombo->count() > MIN_LIST_SIZE)
        {
            ui->actionCombo->setCurrentIndex(ui->actionCombo->findText(cus::EMPTY_SL));
        }
    }

    (void)ui->actionCombo->blockSignals(false);
    updateSlCommandsArea();
}

void StepDialogUI::updateSlCommandsArea()
{

}


void StepDialogUI::saveStep()
{
    m_stepData.comment = ui->commentText->toPlainText();
    m_stepData.executionOrder = m_stepData.executionOrder;
    m_stepData.slText = getCurrentStepText();

    emit saveChanges();

}
QString StepDialogUI::getCurrentStepText()
{
    QString sl;
    sl = ui->subjectLine->text() + SL_COMMANDS_DELIM + ui->actionCombo->currentText();

    return sl;
}
void StepDialogUI::displayInfoInDialog()
{

    const QStringList splittedSLCommand = m_stepData.slText.split(SL_COMMANDS_DELIM, Qt::SkipEmptyParts);
    QStringList actions;

    if (1 < splittedSLCommand.length())
    {
        actions.append(splittedSLCommand[1]);
        updateActions(actions, true);
    }

    ui->executionOrderNumber->setText(m_stepData.executionOrder);
    ui->commentText->setText(m_stepData.comment);

    (void)exec();
}

void StepDialogUI::closeDialog()
{
    const QStringList splittedSLCommand = m_stepData.slText.split(SL_COMMANDS_DELIM, Qt::SkipEmptyParts);

    emit dialogClosed(ui->executionOrderNumber->text().toInt() - ONE, splittedSLCommand);

    (void)close();
}


