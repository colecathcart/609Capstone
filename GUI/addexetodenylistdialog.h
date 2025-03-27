#ifndef ADDEXETODENYLISTDIALOG_H
#define ADDEXETODENYLISTDIALOG_H

#include <QDialog>

namespace Ui {
class addExeToDenylistDialog;
}

class addExeToDenylistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addExeToDenylistDialog(QWidget *parent = nullptr);
    ~addExeToDenylistDialog();

private:
    Ui::addExeToDenylistDialog *ui;
};

#endif // ADDEXETODENYLISTDIALOG_H
