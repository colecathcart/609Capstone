#ifndef ALLOWLISTANDDENYLISTMANAGER_H
#define ALLOWLISTANDDENYLISTMANAGER_H

#include <QStringList>

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


private:
    QStringList denyListExtensions;
    QStringList allowListDirectories;
};

#endif // ALLOWLISTANDDENYLISTMANAGER_H
