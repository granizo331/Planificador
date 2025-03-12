#include <QMenu>

#include "steplistui.h"
#include "steplist.h"
#include "ui_steplistui.h"
#include "constants.h"
#include "../threads.h"
#include "retcodes/messagebox.h"
#include "custom/commonsignals.h"
#include "custom/currenttask.h"


const QString StepListUI::TOOLTIP_EDIT_STEP = "Edit Step";
const QString StepListUI::TOOLTIP_INSERT_STEP = "Insert New Step";
const QString StepListUI::TOOLTIP_DELETE_STEP = "Borrar paso";

StepListUI::StepListUI(QWidget * const parent) : QWidget{parent},
    ui(new Ui::StepListUI),
    m_logic(nullptr),
    m_Db(nullptr)
{
  ui->setupUi(this);
}

StepListUI::~StepListUI()
{
  delete ui;
  delete m_logic;
  threads::deleteThread(m_logicTh);
}

void StepListUI::init(db::Postgres * const db)
{
  // Thread

  m_Db = db;
  m_logic = new logic::StepList(db);
  m_logic->moveToThread(&m_logicTh);


  (void)connect(&m_logicTh, &QThread::finished, m_logic, &QObject::deleteLater);

  m_logicTh.start();
  ui->addButton->setCustomStyle(cus::ButtonStyle::IconOnlyDefault);
  ui->deleteButton->setCustomStyle(cus::ButtonStyle::IconOnlyDelete);
  ui->editButton->setCustomStyle(cus::ButtonStyle::IconOnlyDefault);
  ui->startButton->setCustomStyle(cus::ButtonStyle::IconOnlyDefault);
  ui->autoModeButton->setCustomStyle(cus::ButtonStyle::IconOnlyDefault);

  ui->addButton->setToolTip(TOOLTIP_INSERT_STEP);
  ui->deleteButton->setToolTip(TOOLTIP_DELETE_STEP);
  ui->editButton->setToolTip(TOOLTIP_EDIT_STEP);


  ui->stepTable->setColumnHidden(COL_STEPID, true);

  cus::Style::setIcon(ui->addButton, cus::Icons::Add);
  cus::Style::setIcon(ui->deleteButton, cus::Icons::Delete);
  cus::Style::setIcon(ui->editButton, cus::Icons::Edit);
  cus::Style::setIcon(ui->startButton, cus::Icons::Play);
  cus::Style::setIcon(ui->autoModeButton, cus::Icons::Skip);

  (void)connect(ui->addButton,            &QPushButton::clicked, this, &StepListUI::insertStep);
  (void)connect(ui->deleteButton,         &QPushButton::clicked, this, &StepListUI::deleteStep);
  (void)connect(ui->editButton,           &QPushButton::clicked, this, &StepListUI::editStep);
  (void)connect(ui->startButton,          &QPushButton::clicked, this, &StepListUI::startStepExecution);
  (void)connect(ui->autoModeButton,            &QPushButton::clicked, this, &StepListUI::autoMode);

  (void)connect(ui->stepTable, &QTableWidget::itemDoubleClicked,     this, &StepListUI::editStep);
  (void)connect(ui->stepTable, &QTableWidget::currentItemChanged,    this, &StepListUI::rowSelectionChanged);
  (void)connect(ui->stepTable, &cus::SelectionTable::deselectItem,   this, &StepListUI::deselectStep);

  (void)connect(this, &StepListUI::getStepList,      m_logic, &logic::StepList::getStepList);
  (void)connect(this, &StepListUI::createStepExecution, m_logic, &logic::StepList::createStepExecution);

  (void)connect(this, &StepListUI::newStepAdded,     m_logic, &logic::StepList::addNewStep);
  (void)connect(this, &StepListUI::stepDeleted,      m_logic, &logic::StepList::deleteSelectedStep);
  (void)connect(this, &StepListUI::autoMode, m_logic, &logic::StepList::autoMode);

  (void)connect(m_logic, &logic::StepList::fillSteps,          this, &StepListUI::fillSteps);
  (void)connect(m_logic, &logic::StepList::updateStatusUI,     this, &StepListUI::updateToolbarStatus);
  (void)connect(m_logic, &logic::StepList::stepSelected,       this, &StepListUI::stepSelected);
  (void)connect(m_logic, &logic::StepList::stepsModified,      this, &StepListUI::stepsModified);
  (void)connect(m_logic, &logic::StepList::createdSuccessfully, this, &StepListUI::setStepExecution);
  (void)connect(m_logic, &logic::StepList::finishedAutoMode, this, &StepListUI::finishedTaskAuto);

  ui->startButton->setEnabled(false);
  ui->autoModeButton->setEnabled(false);
  resetList();
}

void StepListUI::initStepList()
{
  m_executionMode = false;
  ui->startButton->setEnabled(false);
  ui->stepTable->clearSelection();
  ui->stepTable->setCurrentItem(nullptr);

  emit getStepList();
}

void StepListUI::closeAndClear()
{

  resetList();
}


void StepListUI::rowSelectionChanged(const QTableWidgetItem * const current)
{
    if (!m_executionMode)
    {
        if (nullptr != current)
        {
            updateToolbarStatus();
        }
        else
        {
            ui->deleteButton->setEnabled(false);
            ui->editButton->setEnabled(false);
        }
    }
    else
    {
        rowSelectionChangedExecution(current);
    }
}

void StepListUI::updateToolbarStatus()
{
  if (!cus::currentTask.isEmpty())
  {
    editingView();
  }
  else
  {
    readOnlyView();
  }
}

void StepListUI::readOnlyView()
{
  ui->addButton->setEnabled(false);
  ui->deleteButton->setEnabled(false);

  if (ui->stepTable->currentItem() != nullptr)
  {
    ui->editButton->setEnabled(true);
  }
  else
  {
    ui->editButton->setEnabled(false);
  }

  cus::Style::setIcon(ui->editButton, cus::Icons::View);

  emit cus::commonsignals.editStepListEnabled(false);
}

void StepListUI::executionView()
{
    m_executionMode = true;
    ui->addButton->setEnabled(false);
    ui->editButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
    if (ui->stepTable->currentItem() != nullptr)
    {
        ui->startButton->setEnabled(true);
        ui->autoModeButton->setEnabled(true);
    }


}

void StepListUI::editingView()
{
  if (ui->stepTable->currentItem() != nullptr)
  {
    ui->addButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
    ui->editButton->setEnabled(true);
  }
  else
  {
    ui->addButton->setEnabled(true);
    ui->deleteButton->setEnabled(false);
    ui->editButton->setEnabled(false);
  }

  cus::Style::setIcon(ui->editButton, cus::Icons::Edit);
  ui->editButton->setToolTip(TOOLTIP_EDIT_STEP);

}

void StepListUI::resetList()
{
  clearList();

  readOnlyView();
}

void StepListUI::clearList()
{
  ui->stepTable->setRowCount(ZERO);
}

void StepListUI::fillSteps(const QList<cus::STEP_INFO> & stepList) noexcept
{
  const int32_t currentRow = ui->stepTable->currentRow();

  clearList();

  for (const cus::STEP_INFO & step : stepList)
  {
    ui->stepTable->insertRow(ui->stepTable->rowCount());
    fillRow(step);
  }
  if ((ZERO <= currentRow) &&
      (ui->stepTable->rowCount() > currentRow))
  {
    ui->stepTable->selectRow(currentRow);
  }
}

void StepListUI::fillRow(const cus::STEP_INFO &stepRow)
{
  bool errorRow;
  const QStringList stepSplitted = stepRow.slText.split(cus::SL_REGEXP);
  const int32_t rowPos = stepRow.executionOrder.toInt()-ONE;

  if (INVALID_ID != stepSplitted.indexOf(cus::EMPTY_SL))
  {
    errorRow = true;
  }
  else
  {
    errorRow = false;
  }

  for (int32_t i = COL_SUBJECT; i <= COL_QUALIFIER; ++i)
  {
    if (i < (stepSplitted.size()))
    {
      ui->stepTable->setItem(rowPos, i, new QTableWidgetItem(stepSplitted[i]));
    }
    else
    {
      ui->stepTable->setItem(rowPos, i, new QTableWidgetItem(cus::EMPTY_STRING));
    }

    ui->stepTable->item(rowPos, i)->setToolTip(stepRow.slText);
  }
  ui->stepTable->setItem(rowPos, COL_STEPID,    new QTableWidgetItem(stepRow.id));

  ui->stepTable->setRowIncomplete(stepRow.executionOrder.toInt()-ONE, errorRow);
}

QString StepListUI::getStepText(const int32_t row)
{
  QString slText;

  if (nullptr != ui->stepTable->item(row, COL_SUBJECT))
  {
    (void)slText.append(ui->stepTable->item(row, COL_SUBJECT)->text());
  }

  for (int32_t i = COL_ACTION; i <= COL_QUALIFIER; ++i)
  {
    if (cus::EMPTY_STRING != ui->stepTable->item(row, i)->text())
    {
      (void)slText.append(SL_COMMANDS_DELIM + ui->stepTable->item(row, i)->text());
    }
  }

  return slText;
}

void StepListUI::insertStep()
{
  int32_t newItemRow;
  cus::STEP_INFO newStep;
  newStep.slText = logic::StepList::EMPTY_SL_TEXT.join(SL_COMMANDS_DELIM);

  if (nullptr == ui->stepTable->currentItem())
  {
    newItemRow = ui->stepTable->rowCount();
    ui->stepTable->insertRow(newItemRow);
    newStep.executionOrder = QString::number(newItemRow + ONE);

    fillRow(newStep);
  }
  else
  {
    newItemRow = ui->stepTable->currentRow() + ONE;
    ui->stepTable->insertRow(newItemRow);
    newStep.executionOrder = QString::number(newItemRow + ONE);

    fillRow(newStep);
    rowSelectionChanged(ui->stepTable->currentItem());
  }

  ui->stepTable->selectRow(newItemRow);
  ++newItemRow;

  emit newStepAdded(newStep);
}

void StepListUI::deleteStep()
{
  if (nullptr != ui->stepTable->currentItem())
  {
    const int32_t deletedRow = ui->stepTable->currentRow();
    const QString name = getStepText(deletedRow);

    const bool returnButton = MessageBox::question(this, TOOLTIP_DELETE_STEP, rc::RetCode::ConfirmStepDeletion, {name});

    if (returnButton)
    {
      if (ui->stepTable->rowCount() != ZERO)
      {
        if (deletedRow == ZERO)
        {
          ui->stepTable->selectRow(deletedRow);
        }
        else
        {
          ui->stepTable->selectRow(deletedRow - ONE);
        }
      }

      const cus::STEP_INFO stepToDelete = getStepFromRow(deletedRow);
      ui->stepTable->removeRow(deletedRow);
      rowSelectionChanged(ui->stepTable->currentItem());
      if (ui->stepTable->rowCount() == ZERO)
      {
        ui->addButton->setEnabled(true);
      }
      emit stepDeleted(stepToDelete);
    }
  }
}

void StepListUI::updateStepAfterEdit(const int32_t &row, const QStringList &slText)
{
  cus::STEP_INFO step;
  step.executionOrder = QString::number(row + ONE);
  step.slText = slText.join(SL_COMMANDS_DELIM);
  step.id = ui->stepTable->item(row, COL_STEPID)->text();

  fillRow(step);
  rowSelectionChanged(ui->stepTable->currentItem());
}

void StepListUI::deselectStep()
{
  ui->stepTable->clearSelection();
  ui->stepTable->setCurrentItem(nullptr);

  emit stepSelected(cus::EMPTY_STRING, cus::EMPTY_STRING);
}

void StepListUI::editStep()
{
  if (nullptr != ui->stepTable->currentItem())
  {
    const bool readOnly =  false;
    const int32_t stepRow = ui->stepTable->currentItem()->row();

    const QString stepID = ui->stepTable->item(stepRow, COL_STEPID)->text();


    emit editStepDialog(stepID, readOnly);
  }
}

cus::STEP_INFO StepListUI::getStepFromRow(const int32_t row) noexcept
{
  cus::STEP_INFO step;
  step.slText = getStepText(row);
  step.executionOrder = QString::number(row + ONE);
  step.id = ui->stepTable->item(row, COL_STEPID)->text();
  return step;
}

void StepListUI::startStepExecution()
{
    ui->autoModeButton->setEnabled(false);
    cus::STEP_INFO currentStep = getStepFromRow(ui->stepTable->currentRow());
    emit createStepExecution(currentStep,false);
}

void StepListUI::setStepExecution(const QString &stepExecutionID, bool mode)
{
    m_currentStepExecution = stepExecutionID;
    ui->startButton->setEnabled(false);
    m_autoMode = mode;

    int currentRow = ui->stepTable->currentRow();

    if (currentRow >= 0)
    {
        for (int col = 0; col < ui->stepTable->columnCount(); ++col)
        {
            QTableWidgetItem *item = ui->stepTable->item(currentRow, col);
            if (item)
            {
                item->setBackground(Qt::green);
            }
        }

        int nextRow = currentRow +1;
        if (nextRow < ui->stepTable->rowCount())
        {
            ui->stepTable->setCurrentCell(nextRow,0);
        }
    }
}

void StepListUI::finishedTaskAuto()
{
    ui->startButton->setEnabled(false);
    ui->autoModeButton->setEnabled(false);
    m_autoMode = false;
}

void StepListUI::autoModeButtonClicked()
{
    emit autoMode();
}

bool StepListUI::isEmpty()
{
    return ui->stepTable->rowCount()==0;

}

void StepListUI::resizeEvent(QResizeEvent *event)
{
  const int32_t width20Percentage =
      static_cast<int32_t>(static_cast<real32_t>(ui->stepTable->width())*cus::PERCENTAGE_20);

  const int32_t width25Percentage =
      static_cast<int32_t>(static_cast<real32_t>(ui->stepTable->width())*cus::PERCENTAGE_25);

  ui->stepTable->setColumnWidth(COL_SUBJECT, width20Percentage);
  ui->stepTable->setColumnWidth(COL_ACTION, width25Percentage);
  ui->stepTable->setColumnWidth(COL_OBJECT, width25Percentage);
  ui->stepTable->horizontalHeader()->setStretchLastSection(true);
}

void StepListUI::rowSelectionChangedExecution(const QTableWidgetItem * const current) noexcept
{
    ui->startButton->setEnabled(true);
    ui->autoModeButton->setEnabled(true);

    if (current)
    {
        int currentRow = current->row();

        if (currentRow != 0)
        {
            ui->autoModeButton->setEnabled(false);
        }

        for (int col = 0; col < ui->stepTable->columnCount(); ++col)
        {
            QTableWidgetItem *item = ui->stepTable->item(currentRow, col);
            if (item && item->background().color() == Qt::green)
            {
                ui->startButton->setEnabled(false);
                ui->autoModeButton->setEnabled(false);
            }
        }
    }
    else
    {
        ui->startButton->setEnabled(false);
        ui->autoModeButton->setEnabled(false);
    }

    if (m_autoMode == true)
    {
        ui->startButton->setEnabled(false);
    }
}
