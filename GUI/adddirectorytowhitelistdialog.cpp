#include "adddirectorytowhitelistdialog.h"
#include "ui_adddirectorytowhitelistdialog.h"

AddDirectoryToWhitelistDialog::AddDirectoryToWhitelistDialog(QWidget *parent, AllowListAndDenyListManager *manager)
    : QDialog(parent), listManager(manager)
    , ui(new Ui::AddDirectoryToWhitelistDialog)
{
    ui->setupUi(this);
}

AddDirectoryToWhitelistDialog::~AddDirectoryToWhitelistDialog()
{
    delete ui;
}

void AddDirectoryToWhitelistDialog::on_whitelistDirectoryButtons_accepted()
{
    QString newDir = ui->directoryPath->text().trimmed();

    if (!newDir.isEmpty()) {
        listManager->addAllowListDirectory(newDir);
    }
}

