#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QString>
#include <QList>
#include <QProcess>
#include <QRegularExpression>
#include "systemobserver.h"
#include "logreader.h"

class SystemMonitor {
public:
    void addObserver(SystemObserver* observer) {
        observers.append(observer);
    }

    void removeObserver(SystemObserver* observer) {
        observers.removeAll(observer);
    }

    void notifyObservers(const bool isOn, const double cpu, const double mem, const QString &log) {
        for (auto observer : observers) {
            observer->update(isOn, cpu, mem, log);
        }
    }

    void fetchAndNotify(const QString &processName) {
        // Find all PIDs with pgrep
        QProcess pidProcess;
        pidProcess.start("/bin/bash", QStringList() << "-c" << "pgrep -f \"" + processName + "\"");
        pidProcess.waitForFinished();
        QString pidOutput = pidProcess.readAllStandardOutput().trimmed();


        if (pidOutput.isEmpty()) {
            notifyObservers(false, 0,0, nullptr);
            return;
        }

        QStringList pids = pidOutput.split("\n");

        // Get CPU and RAM usage using ps
        double totalCpu = 0.0;
        double totalMem = 0.0;

        for (const QString &pid : pids) {
            QProcess psProcess;
            psProcess.start("/bin/bash", QStringList() << "-c" << QString("ps -p %1 -o %cpu,%mem --no-headers").arg(pid));
            psProcess.waitForFinished();
            QString output = psProcess.readAllStandardOutput().trimmed();

            if (!output.isEmpty()) {
                QStringList values = output.split(whitespaceSplitter);
                if (values.size() == 2) {
                    totalCpu += values[0].toDouble();
                    totalMem += values[1].toDouble();
                }
            }
        }

        QString msg = reader.receive_message();

        notifyObservers(true, totalCpu, totalMem, msg);

    }

private:
    QList<SystemObserver*> observers;
    static QRegularExpression whitespaceSplitter;
    LogReader reader;


};

#endif // SYSTEMMONITOR_H
