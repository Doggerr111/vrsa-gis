#ifndef TEMPORARYGRAPHICSITEM_H
#define TEMPORARYGRAPHICSITEM_H

#include <QObject>

class TemporaryGraphicsItem : public QObject
{
    Q_OBJECT
public:
    explicit TemporaryGraphicsItem(QObject *parent = nullptr);

signals:

};

#endif // TEMPORARYGRAPHICSITEM_H
