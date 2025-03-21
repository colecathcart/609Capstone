#include "detectormanager.h"
#include <QProcess>
#include <QDir>
#include <QCoreApplication>

DetectorManager::DetectorManager() {}

void DetectorManager::startDetector() {
    QString detectorPath = QDir::cleanPath(QCoreApplication::applicationDirPath() + "../../../../app/ransomware_detector");
    QString program = QString("pkexec %1 /").arg(detectorPath);

    QProcess::startDetached("/bin/bash", QStringList() << "-c" << program);
}

void DetectorManager::stopDetector() {
    QProcess process;

    process.start("/bin/bash", QStringList() << "-c" << "pgrep -f ransomware_detector");
    process.waitForFinished();
    QString pidOutput = process.readAllStandardOutput().trimmed();

    if (pidOutput.isEmpty()) {
        return;
    }

    QStringList pids = pidOutput.split("\n");
    for (const QString &pid : pids) {
        process.start("/bin/bash", QStringList() << "-c" << QString("pkexec kill %1").arg(pid));
        process.waitForFinished();
    }

}
