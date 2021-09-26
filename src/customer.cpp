#include "customer.h"

Customer::Customer() {}

Customer::Customer(qint64 id, QString code, QString name, QString address, QString phone) :
    id(id), code(code), name(name), address(address), phone(phone) {}

qint64 Customer::getId() const {
    return id;
}

QString Customer::getCode() const {
    return code;
}

QString Customer::getPhone() const {
    return phone;
}

QString Customer::getAddress() const {
    return address;
}

QString Customer::getName() const {
    return name;
}

Customer Customer::fromJSON(QJsonObject &obj) {
    qint64 id = obj["id"].toInt();
    QString code = obj["code"].toString();
    QString name = obj["name"].toString();
    QString address = obj["address"].toString();
    QString phone = obj["phone"].toString();
    return Customer(id, code, name, address, phone);
}
