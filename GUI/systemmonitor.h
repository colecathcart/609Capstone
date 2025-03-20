#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QString>
#include <QList>
#include <QProcess>
#include "systemobserver.h"

class SystemMonitor {
public:
    void addObserver(SystemObserver* observer) {
        observers.append(observer);
    }

    void removeObserver(SystemObserver* observer) {
        observers.removeAll(observer);
    }

    void notifyObservers(const QString &usageData) {
        for (auto observer : observers) {
            observer->update(usageData);
        }
    }

    void fetchAndNotify(const QString &processName) {
        // Find all PIDs with pgrep
        QProcess pidProcess;
        pidProcess.start("pgrep " + processName);
        pidProcess.waitForFinished();
        QString pidOutput = pidProcess.readAllStandardOutput().trimmed();

    }

private:
    QList<SystemObserver*> observers;
};

#endif // SYSTEMMONITOR_H
