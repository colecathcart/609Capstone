#ifndef SYSTEMOBSERVER_H
#define SYSTEMOBSERVER_H

#include <QString>

class SystemObserver {
public:
    virtual ~SystemObserver() = default;
    virtual void update(const QString &usageData) = 0;
};

#endif // SYSTEMOBSERVER_H
