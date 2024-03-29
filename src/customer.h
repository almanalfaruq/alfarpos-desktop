#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QString>
#include <QJsonObject>

class Customer {
private:
    qint64 id;
    QString name, address, phone;

public:
    Customer();
    Customer(qint64 id, QString name, QString address, QString phone);

    qint64 getId() const;
    QString getName() const;
    QString getAddress() const;
    QString getPhone() const;

    static Customer fromJSON(QJsonObject &);
};

#endif // CUSTOMER_H
