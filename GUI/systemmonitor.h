#ifndef SYSTEMMONITOR_H
#define SYSTEMMONITOR_H

#include <QString>
#include <QList>
#include <QProcess>
#include <QFile>
#include <QRegularExpression>
#include <unistd.h>
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
        double maxCpu = 0.0;
        double maxMem = 0.0;
        long pageSize = sysconf(_SC_PAGESIZE);

        for (const QString &pid : pids) {
            QProcess psProcess;
            QString command = QString("top -b -n 1 -H -p %1 | awk '/%CPU/ {found=1; next} found {print $9}'").arg(pid);
            //psProcess.start("/bin/bash", QStringList() << "-c" << QString("ps -p %1 -o %cpu,%mem --no-headers").arg(pid));
            psProcess.start("/bin/bash", QStringList() << "-c" << command);
            psProcess.waitForFinished();
            QString output = psProcess.readAllStandardOutput().trimmed();

            QStringList cpuUsages = output.split("\n");
            for (const QString &cpuStr : cpuUsages) {
                bool ok;
                double cpu = cpuStr.toDouble(&ok);
                if (ok && cpu > maxCpu) {
                    maxCpu = cpu;
                }
            }

            // Fetch memory usage using `/proc/<pid>/statm`
            QFile statmFile(QString("/proc/%1/statm").arg(pid));
            if (statmFile.open(QIODevice::ReadOnly)) {
                QString statmData = statmFile.readLine().trimmed();
                statmFile.close();

                QStringList values = statmData.split(QRegularExpression("\\s+"));
                if (!values.isEmpty()) {
                    bool ok;
                    long residentPages = values[1].toLong(&ok); // Resident Set Size (RSS)
                    double totalMem = 0.0;
                    if (ok) totalMem = (residentPages * pageSize) / (1024.0 * 1024.0); // Convert to MB
                    maxMem = totalMem > maxMem ? totalMem : maxMem;
                }
            }
        }

        QString msg = reader.receive_message();

        notifyObservers(true, maxCpu, maxMem, msg);

    }

private:
    QList<SystemObserver*> observers;
    static QRegularExpression whitespaceSplitter;
    LogReader reader;


};

#endif // SYSTEMMONITOR_H
