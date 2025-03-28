#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <systemmonitor.h>
#include <systemobserver.h>
#include <detectormanager.h>
#include <allowlistanddenylistmanager.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow, public SystemObserver
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void update(const bool isOn, const double cpu, const double mem, const QString &log) override;


private slots:
    void on_newDirectoryButton_clicked();

    void fetchData();

    void on_startButton_released();

    void on_stopButton_released();

    void on_newExeButton_released();

    void on_removeDirectoryButton_released();

    void on_removeExeButton_released();

private:
    Ui::MainWindow *ui;
    SystemMonitor *monitor;
    DetectorManager *detector;
    AllowListAndDenyListManager *listManager;
    bool isOn;
};
#endif // MAINWINDOW_H
