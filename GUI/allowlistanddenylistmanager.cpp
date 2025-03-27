#include "allowlistanddenylistmanager.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>


AllowListAndDenyListManager::AllowListAndDenyListManager() {
    denyListExtensionsModel = new QStringListModel();
    allowListDirectoriesModel = new QStringListModel();

    loadDenyListExtensions();
    loadAllowListDirectories();
}

// ---------- Load Functions ----------
void AllowListAndDenyListManager::loadDenyListExtensions() {
    QFile file(BLACKLIST_EXTENSIONS_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open" << BLACKLIST_EXTENSIONS_PATH << "for reading.";
        return;
    }

    QStringList extensions;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            extensions.append(line);
        }
    }
    file.close();
    denyListExtensionsModel->setStringList(extensions);
}

void AllowListAndDenyListManager::loadAllowListDirectories() {
    QFile file(WHITELIST_DIRS_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open" << WHITELIST_DIRS_PATH << "for reading.";
        return;
    }

    QStringList dirs;
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            dirs.append(line);
        }
    }
    file.close();
    allowListDirectoriesModel->setStringList(dirs);
}

// ---------- Add Functions ----------
void AllowListAndDenyListManager::addDenyListExtension(QString newExtension) {
    QStringList currentExtensions = denyListExtensionsModel->stringList();
    if (!currentExtensions.contains(newExtension.trimmed())) {
        currentExtensions.append(newExtension.trimmed());
        denyListExtensionsModel->setStringList(currentExtensions);
        saveToFile(BLACKLIST_EXTENSIONS_PATH, denyListExtensionsModel);
    } else {
        qDebug() << "Extension already in list";
    }
}

void AllowListAndDenyListManager::addAllowListDirectory(QString newDirectory) {
    QStringList currentDirs = allowListDirectoriesModel->stringList();
    if (!currentDirs.contains(newDirectory.trimmed())) {
        currentDirs.append(newDirectory.trimmed());
        allowListDirectoriesModel->setStringList(currentDirs);
        saveToFile(WHITELIST_DIRS_PATH, allowListDirectoriesModel);
    } else {
        qDebug() << "Directory already in list";
    }
}

// ---------- Remove Functions ----------
void AllowListAndDenyListManager::removeDenyListExtension(QString removedExtension) {
    QStringList currentExtensions = denyListExtensionsModel->stringList();
    if (!currentExtensions.contains(removedExtension)) {
        qWarning() << "Extension not found.";
        return;
    }
    currentExtensions.removeAll(removedExtension);
    denyListExtensionsModel->setStringList(currentExtensions);
    saveToFile(BLACKLIST_EXTENSIONS_PATH, denyListExtensionsModel);
}

void AllowListAndDenyListManager::removeAllowListDirectory(QString removedDirectory) {
    QStringList currentDirs = allowListDirectoriesModel->stringList();
    if (!currentDirs.contains(removedDirectory)) {
        qWarning() << "Directory not found.";
        return;
    }
    currentDirs.removeAll(removedDirectory);
    allowListDirectoriesModel->setStringList(currentDirs);
    saveToFile(WHITELIST_DIRS_PATH, allowListDirectoriesModel);
}

void AllowListAndDenyListManager::saveToFile(const QString &filePath, QStringListModel *model){
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &item : model->stringList()) {
            out << item << "\n";
        }
        file.close();
    } else {
        qWarning() << "Could not open" << filePath << "for writing.";
    }
}
