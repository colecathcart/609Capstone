#include "addexetowhitelistdialog.h"
#include "ui_addexetowhitelistdialog.h"

addExeToWhitelistDialog::addExeToWhitelistDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addExeToWhitelistDialog)
{
    ui->setupUi(this);
}

addExeToWhitelistDialog::~addExeToWhitelistDialog()
{
    delete ui;
}
