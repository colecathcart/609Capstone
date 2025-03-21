#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QString>
#include <QList>
#include <QProcess>
#include <QRegularExpression>
#include "systemobserver.h"

class SystemMonitor {
public:
    void addObserver(SystemObserver* observer) {
        observers.append(observer);
    }

    void removeObserver(SystemObserver* observer) {
        observers.removeAll(observer);
    }

    void notifyObservers(const bool isOn, const double cpu, const double mem) {
        for (auto observer : observers) {
            observer->update(isOn, cpu, mem);
        }
    }

    void fetchAndNotify(const QString &processName) {
        // Find all PIDs with pgrep
        QProcess pidProcess;
        pidProcess.start("/bin/bash", QStringList() << "-c" << "pgrep -f \"" + processName + "\"");
        pidProcess.waitForFinished();
        QString pidOutput = pidProcess.readAllStandardOutput().trimmed();


        if (pidOutput.isEmpty()) {
            notifyObservers(false, 0,0);
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

        notifyObservers(true, totalCpu, totalMem);

    }

private:
    QList<SystemObserver*> observers;
    static QRegularExpression whitespaceSplitter;
};

#endif // SYSTEMMONITOR_H
