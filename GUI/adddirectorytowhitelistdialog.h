#ifndef ADDDIRECTORYTOWHITELISTDIALOG_H
#define ADDDIRECTORYTOWHITELISTDIALOG_H

#include <QDialog>

namespace Ui {
class AddDirectoryToWhitelistDialog;
}

class AddDirectoryToWhitelistDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDirectoryToWhitelistDialog(QWidget *parent = nullptr);
    ~AddDirectoryToWhitelistDialog();

private:
    Ui::AddDirectoryToWhitelistDialog *ui;
};

#endif // ADDDIRECTORYTOWHITELISTDIALOG_H
