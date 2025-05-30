#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addexetodenylistdialog.h"
#include "adddirectorytowhitelistdialog.h"
#include "systemmonitor.h"
#include <QTimer>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    monitor(new SystemMonitor),
    detector(new DetectorManager),
    listManager(new AllowListAndDenyListManager)
{
    ui->setupUi(this);
    monitor->addObserver(this);

    this->isOn = false;

    ui->denylistExeList->setModel(listManager->getDenyListExtensionModel());
    ui->whitelistDirectoryList->setModel(listManager->getAllowListDirectoryModel());

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
    ui->memProgressBar->setFormat(QString::number(mem, 'f', 2) + "MB");

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

    ui->denylistExeList->setModel(listManager->getDenyListExtensionModel());
    ui->whitelistDirectoryList->setModel(listManager->getAllowListDirectoryModel());

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


void MainWindow::on_newDirectoryButton_clicked()
{

    QString directory = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "/",QFileDialog::ShowDirsOnly);

    if (!directory.isEmpty()) {
        listManager->addAllowListDirectory(directory.append('/'));
    } else {
        qWarning() << "No directory selected.";
    }
    //AddDirectoryToWhitelistDialog whitelistDialog(this, listManager);
    //whitelistDialog.setModal(true);
    //whitelistDialog.exec();
}


void MainWindow::on_newExeButton_released()
{
    addExeToDenylistDialog denylistDialog(this, listManager);
    denylistDialog.setModal(true);
    denylistDialog.exec();
}


void MainWindow::on_removeDirectoryButton_released()
{
    // Get the selected index from the QListView
    QModelIndex selectedIndex = ui->whitelistDirectoryList->selectionModel()->currentIndex();

    if (selectedIndex.isValid()) {
        // Get the selected directory from the model
        QString selectedDirectory = selectedIndex.data().toString();

        // Remove the selected directory using the manager
        listManager->removeAllowListDirectory(selectedDirectory);
    }
}


void MainWindow::on_removeExeButton_released()
{
    // Get the selected index from the QListView
    QModelIndex selectedIndex = ui->denylistExeList->selectionModel()->currentIndex();

    if (selectedIndex.isValid()) {
        // Get the selected directory from the model
        QString selectedExtension = selectedIndex.data().toString();

        // Remove the selected directory using the manager
        listManager->removeDenyListExtension(selectedExtension);
    }

}

