#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addexetowhitelistdialog.h"
#include "adddirectorytowhitelistdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_newExeButton_clicked()
{
    addExeToWhitelistDialog whitelistDialog;
    whitelistDialog.setModal(true);
    whitelistDialog.exec();


}


void MainWindow::on_newDirectoryButton_clicked()
{
    AddDirectoryToWhitelistDialog whitelistDialog;
    whitelistDialog.setModal(true);
    whitelistDialog.exec();
}

