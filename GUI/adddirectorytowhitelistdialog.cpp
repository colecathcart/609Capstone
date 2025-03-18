#include "adddirectorytowhitelistdialog.h"
#include "ui_adddirectorytowhitelistdialog.h"

AddDirectoryToWhitelistDialog::AddDirectoryToWhitelistDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddDirectoryToWhitelistDialog)
{
    ui->setupUi(this);
}

AddDirectoryToWhitelistDialog::~AddDirectoryToWhitelistDialog()
{
    delete ui;
}
