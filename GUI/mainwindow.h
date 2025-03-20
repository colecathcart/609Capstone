#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <systemmonitor.h>
#include <systemobserver.h>

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
    void update(const bool isOn, const double cpu, const double mem) override;


private slots:
    void on_newExeButton_clicked();

    void on_newDirectoryButton_clicked();

    void fetchData();

private:
    Ui::MainWindow *ui;
    SystemMonitor *monitor;
};
#endif // MAINWINDOW_H
