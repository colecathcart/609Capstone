#ifndef ADDEXETOWHITELISTDIALOG_H
#define ADDEXETOWHITELISTDIALOG_H

#include <QDialog>

namespace Ui {
class addExeToWhitelistDialog;
}

class addExeToWhitelistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addExeToWhitelistDialog(QWidget *parent = nullptr);
    ~addExeToWhitelistDialog();

private:
    Ui::addExeToWhitelistDialog *ui;
};

#endif // ADDEXETOWHITELISTDIALOG_H
