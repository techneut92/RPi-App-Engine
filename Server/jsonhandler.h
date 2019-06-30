#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include <QObject>

class jsonHandler : public QObject
{
    Q_OBJECT
public:
    explicit jsonHandler(QObject *parent = nullptr);
    static QVariantMap jsonStringToQMap(QString data);
    static bool isValidJson(QString data);

signals:

public slots:
};

#endif // JSONHANDLER_H
