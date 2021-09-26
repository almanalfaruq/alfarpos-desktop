#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <QString>
#include <QJsonObject>

class Customer {
private:
    qint64 id;
    QString code, name, address, phone;
public:
    Customer();
    Customer(qint64 id, QString code, QString name, QString address, QString phone);
    qint64 getId() const;
    QString getCode() const;
    QString getPhone() const;
    QString getAddress() const;
    QString getName() const;
    static Customer fromJSON(QJsonObject &);
};

#endif // CUSTOMER_H
