#include "addexetodenylistdialog.h"
#include "ui_addexetodenylistdialog.h"

addExeToDenylistDialog::addExeToDenylistDialog(QWidget *parent, AllowListAndDenyListManager *manager)
    : QDialog(parent), listManager(manager)
    , ui(new Ui::addExeToDenylistDialog)
{
    ui->setupUi(this);
}

addExeToDenylistDialog::~addExeToDenylistDialog()
{
    delete ui;
}

void addExeToDenylistDialog::on_exeDenylistDialogButtons_accepted()
{

}

