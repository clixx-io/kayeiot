#ifndef NEWEVENTSIGNALDIALOG_H
#define NEWEVENTSIGNALDIALOG_H

#include <QDialog>

namespace Ui {
class NewEventSignalDialog;
}

class NewEventSignalDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewEventSignalDialog(QWidget *parent = 0);
    ~NewEventSignalDialog();

private:
    Ui::NewEventSignalDialog *ui;
};

#endif // NEWEVENTSIGNALDIALOG_H
