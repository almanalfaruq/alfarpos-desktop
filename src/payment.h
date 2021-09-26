#ifndef PAYMENT_H
#define PAYMENT_H

#include <QString>
#include <QJsonObject>

class Payment {
private:
    qint64 id;
    QString name;
public:
    Payment();
    Payment(qint64 id, QString name);

    qint64 getId() const;
    QString getName() const;
    static Payment fromJSON(QJsonObject &);
};

#endif // PAYMENT_H
