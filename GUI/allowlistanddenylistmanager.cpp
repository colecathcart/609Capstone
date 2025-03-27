#include "allowlistanddenylistmanager.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>


AllowListAndDenyListManager::AllowListAndDenyListManager() {
    loadDenyListExtensions();
    loadAllowListDirectories();
}

// ---------- Load Functions ----------
void AllowListAndDenyListManager::loadDenyListExtensions() {
    QFile file(BLACKLIST_EXTENSIONS_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open .txt for reading.";
        return;
    }
    denyListExtensions.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            denyListExtensions << line;
        }
    }
    file.close();
    qDebug() << "Loaded Deny List Extensions:" << denyListExtensions;
}

void AllowListAndDenyListManager::loadAllowListDirectories() {
    QFile file(WHITELIST_DIRS_PATH);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open .txt for reading.";
        return;
    }
    allowListDirectories.clear();
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            allowListDirectories << line;
        }
    }
    file.close();
    qDebug() << "Loaded Allow List Directories:" << allowListDirectories;
}

// ---------- Add Functions ----------
void AllowListAndDenyListManager::addDenyListExtension(QString newExtension) {
    if (!denyListExtensions.contains(newExtension.trimmed())) {
        denyListExtensions << newExtension.trimmed();
        QFile file(BLACKLIST_EXTENSIONS_PATH);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QTextStream out(&file);
            out << newExtension.trimmed() << "\n";
            file.close();
            qDebug() << "Added to Deny List:" << newExtension;
        } else {
            qWarning() << "Could not open .txt for writing.";
        }
    } else {
        qDebug() << "Extension already in Deny List:" << newExtension;
    }
}

void AllowListAndDenyListManager::addAllowListDirectory(QString newDirectory) {
    if (!allowListDirectories.contains(newDirectory.trimmed())) {
        allowListDirectories << newDirectory.trimmed();
        QFile file(WHITELIST_DIRS_PATH);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
            QTextStream out(&file);
            out << newDirectory.trimmed() << "\n";
            file.close();
            qDebug() << "Added to Allow List:" << newDirectory;
        } else {
            qWarning() << "Could not open .txt for writing.";
        }
    } else {
        qDebug() << "Directory already in Allow List:" << newDirectory;
    }
}

// ---------- Remove Functions ----------
void AllowListAndDenyListManager::removeDenyListExtension(QString removedExtension) {
    if (denyListExtensions.isEmpty()) {
        qWarning() << "No extensions available to remove.";
        return;
    }

    denyListExtensions.removeAll(removedExtension);

    QFile file(BLACKLIST_EXTENSIONS_PATH);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &ext : denyListExtensions) {
            out << ext << "\n";
        }
        file.close();
        qDebug() << "Removed from Deny List:" << removedExtension;
    } else {
        qWarning() << "Could not open" << BLACKLIST_EXTENSIONS_PATH << "for writing.";
    }
}

void AllowListAndDenyListManager::removeAllowListDirectory(QString removedDirectory) {
    if (allowListDirectories.isEmpty()) {
        qWarning() << "No directories available to remove.";
        return;
    }

    allowListDirectories.removeAll(removedDirectory);

    QFile file(WHITELIST_DIRS_PATH);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &dir : allowListDirectories) {
            out << dir << "\n";
        }
        file.close();
        qDebug() << "Removed from Allow List:" << removedDirectory;
    } else {
        qWarning() << "Could not open" << WHITELIST_DIRS_PATH << "for writing.";
    }
}
