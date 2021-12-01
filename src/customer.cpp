#include "customer.h"

Customer::Customer() {

}

Customer::Customer(qint64 id, QString name, QString address, QString phone) : id(id), name(name), address(address), phone(phone) {

}

qint64 Customer::getId() const {
    return id;
}

QString Customer::getName() const {
    return name;
}

QString Customer::getAddress() const {
    return address;
}

QString Customer::getPhone() const {
    return phone;
}

Customer Customer::fromJSON(QJsonObject &obj) {
    qint64 id = obj["id"].toInt();
    QString name = obj["name"].toString();
    QString address = obj["address"].toString();
    QString phone = obj["phone"].toString();
    return Customer(id, name, address, phone);
}
