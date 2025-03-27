#ifndef ALLOWLISTANDDENYLISTMANAGER_H
#define ALLOWLISTANDDENYLISTMANAGER_H

#include <QStringList>
#include <QStringListModel>

#define BLACKLIST_EXTENSIONS_PATH  "../../../app/data/blacklist_extensions.txt"
#define WHITELIST_DIRS_PATH  "../../../app/data/whitelist_dirs.txt"


class AllowListAndDenyListManager
{
public:
    AllowListAndDenyListManager();
    void loadDenyListExtensions(); // Load from .txt file and update list
    void loadAllowListDirectories(); // Load from .txt file and update list

    void addDenyListExtension(QString newExtension); // Add new item to list and update .txt file
    void addAllowListDirectory(QString newDirectory); // Add new item to list and update .txt file

    void removeDenyListExtension(QString removedExtension); // Removes the selected item from the list and .txt file
    void removeAllowListDirectory(QString removedDirectory); // Removes the selected item from the list and .txt file

    QStringListModel* getDenyListExtensionModel() const;
    QStringListModel* getAllowListDirectoryModel() const;

private:
    QStringListModel* denyListExtensionsModel;
    QStringListModel* allowListDirectoriesModel;

    void saveToFile(const QString& filePath, QStringListModel* model);

};

#endif // ALLOWLISTANDDENYLISTMANAGER_H
