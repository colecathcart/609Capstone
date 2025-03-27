#ifndef ADDEXETODENYLISTDIALOG_H
#define ADDEXETODENYLISTDIALOG_H

#include <QDialog>
#include <allowlistanddenylistmanager.h>

namespace Ui {
class addExeToDenylistDialog;
}

class addExeToDenylistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addExeToDenylistDialog(QWidget *parent = nullptr, AllowListAndDenyListManager* manager = nullptr);
    ~addExeToDenylistDialog();

private slots:
    void on_exeDenylistDialogButtons_accepted();

private:
    Ui::addExeToDenylistDialog *ui;
    AllowListAndDenyListManager* listManager;

};

#endif // ADDEXETODENYLISTDIALOG_H
