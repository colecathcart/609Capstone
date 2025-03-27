#ifndef ADDDIRECTORYTOWHITELISTDIALOG_H
#define ADDDIRECTORYTOWHITELISTDIALOG_H

#include <QDialog>
#include <allowlistanddenylistmanager.h>

namespace Ui {
class AddDirectoryToWhitelistDialog;
}

class AddDirectoryToWhitelistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDirectoryToWhitelistDialog(QWidget *parent = nullptr, AllowListAndDenyListManager* manager = nullptr);
    ~AddDirectoryToWhitelistDialog();

private slots:
    void on_whitelistDirectoryButtons_accepted();

private:
    AllowListAndDenyListManager* listManager;
    Ui::AddDirectoryToWhitelistDialog *ui;
};

#endif // ADDDIRECTORYTOWHITELISTDIALOG_H
