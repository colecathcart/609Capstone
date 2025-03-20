#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addexetowhitelistdialog.h"
#include "adddirectorytowhitelistdialog.h"
#include "systemmonitor.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    monitor(new SystemMonitor)
{
    ui->setupUi(this);
    monitor->addObserver(this);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::fetchData);
    timer->start(500);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete monitor;
}

void MainWindow::fetchData() {
    monitor->fetchAndNotify("ransomware_detector");
}

void MainWindow::update(const bool isOn, const double cpu, const double mem) {
    // Update the CPU progress bar with 2 decimal places
    ui->cpuProgressBar->setValue(static_cast<int>(cpu));  // Update the actual progress value
    ui->cpuProgressBar->setFormat(QString::number(cpu, 'f', 2) + "%");

    // Update the memory progress bar with 2 decimal places
    ui->memProgressBar->setValue(static_cast<int>(mem));  // Update the actual progress value
    ui->memProgressBar->setFormat(QString::number(mem, 'f', 2) + "%");
    if (isOn) {
        ui->statusText->setMarkdown("Ransomware detector is running.");
    } else {
        ui->statusText->setMarkdown("Ransomware detector is not running.");
    }

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

