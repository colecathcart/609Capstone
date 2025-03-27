#include "addexetodenylistdialog.h"
#include "ui_addexetodenylistdialog.h"

addExeToDenylistDialog::addExeToDenylistDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addExeToDenylistDialog)
{
    ui->setupUi(this);
}

addExeToDenylistDialog::~addExeToDenylistDialog()
{
    delete ui;
}
