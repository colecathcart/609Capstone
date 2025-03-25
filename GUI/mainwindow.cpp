#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addexetowhitelistdialog.h"
#include "adddirectorytowhitelistdialog.h"
#include "systemmonitor.h"
#include <QTimer>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    monitor(new SystemMonitor),
    detector(new DetectorManager)
{
    ui->setupUi(this);
    monitor->addObserver(this);

    this->isOn = false;

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

void MainWindow::update(const bool isOn, const double cpu, const double mem, const QString &log) {
    // Update the CPU progress bar with 2 decimal places
    ui->cpuProgressBar->setValue(static_cast<int>(cpu));  // Update the actual progress value
    ui->cpuProgressBar->setFormat(QString::number(cpu, 'f', 2) + "%");

    // Update the memory progress bar with 2 decimal places
    ui->memProgressBar->setValue(static_cast<int>(mem));  // Update the actual progress value
    ui->memProgressBar->setFormat(QString::number(mem, 'f', 2) + "%");

    if (log != "") {
        ui->loggingText->append(log);
    }

    if (isOn) {
        this->isOn = true;
        ui->statusText->setMarkdown("Ransomware detector is running.");
    } else {
        this->isOn = false;
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


void MainWindow::on_startButton_released()
{
    if (this->isOn == true) {
        detector->stopDetector();
        detector->startDetector();
    } else {
        detector->startDetector();
    }
}


void MainWindow::on_stopButton_released()
{
    if (this->isOn == true) {
        detector->stopDetector();
    } else {
        return;
    }
}

