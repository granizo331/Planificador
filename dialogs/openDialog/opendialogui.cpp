#include "opendialogui.h"
#include "opendialog.h"
#include "ui_opendialogui.h"
#include "../../threads.h"
#include "configurationdata.h"
#include "custom/currenttask.h"
OpenDialogUI::OpenDialogUI(QWidget *parent)
    : QDialog(parent),
    ui(new Ui::OpenDialogUI)
{
    ui->setupUi(this);
}

OpenDialogUI::~OpenDialogUI()
{
    delete ui;
    delete m_logic;
    threads::deleteThread(m_logicTh);
}

void OpenDialogUI::init(db::Postgres * const db) noexcept
{
    m_logic = new logic::OpenDialog(db);
    m_logic->moveToThread(&m_logicTh);
    (void)connect(&m_logicTh,&QThread::finished,m_logic, &QObject::deleteLater);

    (void)connect(ui->cancelButton, &QPushButton::clicked, this, &OpenDialogUI::close);
    (void)connect(ui->openButton, &QPushButton::clicked,this, &OpenDialogUI::openTask);
    (void)connect(this, &OpenDialogUI::readList, m_logic, &logic::OpenDialog::readTaskList);
    (void)connect(m_logic,&logic::OpenDialog::sendTask,this,&OpenDialogUI::drawTask);
    (void)connect(this, &OpenDialogUI::openTaskDb,m_logic, &logic::OpenDialog::openTaskDb);
    (void)connect(m_logic, &logic::OpenDialog::openTask, this, &OpenDialogUI::openTaskSuccess);
    (void)connect(this, &OpenDialogUI::readRecentTasks, m_logic, &logic::OpenDialog::readRecentTasks);

    (void)connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, &OpenDialogUI::itemSelectionChanged);
    (void)connect(ui->tasksComboBox, &QComboBox::currentTextChanged, this, &OpenDialogUI::selectorChanged);

    const QStringList items = {"Tareas recientes", "Tareas"};
    ui->tasksComboBox->addItems(items);

    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tasksComboBox->setCurrentIndex(cus::VECTOR_FIRST_ELEMENT);

    ui->openButton->setEnabled(false);


    ui->cancelButton->setCustomStyle(cus::ButtonStyle::CancelDelete);
    ui->openButton->setCustomStyle(cus::ButtonStyle::Default);
    m_logicTh.start();
    m_logic->init();

    exec();
}


void OpenDialogUI::readTaskList()
{
    emit readList();
}

void OpenDialogUI::drawTask(const QString &id,
                            const QString &name,
                            const QString &model,
                            const QString &type,
                            const QString &comment,
                            const QString &status,
                            const QString &author,
                            const QString &date)
{
    const int32_t row = ui->tableWidget->rowCount();

    ui->tableWidget->insertRow(row);

    ui->tableWidget->setItem(row,0, new QTableWidgetItem(name));
    ui->tableWidget->setItem(row, 1, new QTableWidgetItem(type));
    ui->tableWidget->setItem(row, 2, new QTableWidgetItem(status));
    ui->tableWidget->setItem(row,3, new QTableWidgetItem(author));
    ui->tableWidget->setItem(row, 4, new QTableWidgetItem(date));
    ui->tableWidget->setItem(row,5, new QTableWidgetItem(id));
}

void OpenDialogUI::openTask()
{
    if (ui->tableWidget->currentItem() != nullptr)
    {
        const int32_t selected = ui->tableWidget->currentRow();
        emit openTaskDb(ui->tableWidget->item(selected,5)->text());
    }
}

void OpenDialogUI::itemSelectionChanged()
{
    const int32_t row = ui->tableWidget->currentRow();

    if (-1 == row)
    {
        ui->openButton->setEnabled(false);
    }
    else
    {
        ui->openButton->setEnabled(true);
    }
}

void OpenDialogUI::selectorChanged(const QString &text)
{
    ui->tableWidget->setRowCount(0);

    if ("Tareas recientes" == text)
    {
        emit readRecentTasks();
    }
    else if ("Tareas" == text)
    {
        emit readList();
    }
    else
    {

    }
}
void OpenDialogUI::openTaskSuccess()
{
    auto confErrors = config::data.addTask2RecentFile(cus::currentTask.name(), cus::currentTask.type());
    setResult(static_cast<int32_t>(QDialog::Accepted));
    (void)close();
}


