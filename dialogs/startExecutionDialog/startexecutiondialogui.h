#pragma once

#include <QDialog>
#include <QThread>

namespace Ui
{
class StartExecutionDialogUI;
}

namespace db
{
class Postgres;
}

namespace logic
{
class Planificator;
class StartExecutionDialog;

}


class StartExecutionDialogUI : public QDialog
{
    Q_OBJECT
public:
    explicit StartExecutionDialogUI(QWidget * parent = nullptr);
    ~StartExecutionDialogUI();
    void init(db::Postgres * const db) noexcept;
public slots:
    void startButton();
    void createdTasklog();
signals:
    void createTasklog(const QString &name);
private:
    void setTaskLogName();
    Ui::StartExecutionDialogUI * ui;
    logic::StartExecutionDialog * m_logic;
    QThread m_logicTh;







};
