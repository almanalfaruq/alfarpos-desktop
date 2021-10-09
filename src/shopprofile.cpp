#include "shopprofile.h"

ShopProfile::ShopProfile() {

}

ShopProfile::ShopProfile(QString name, QString address, QString phone, QString thankyouMessage, QString footNote) :
    name(name), address(address), phone(phone), thankyouMessage(thankyouMessage), footNote(footNote) {}

ShopProfile ShopProfile::fromJSON(QJsonObject &obj) {
    QString name = obj["name"].toString();
    QString address = obj["address"].toString();
    QString phone = obj["phone"].toString();
    QString thankyouMessage = obj["thankyouMessage"].toString();
    QString footNote = obj["footNote"].toString();
    return ShopProfile(name, address, phone, thankyouMessage, footNote);
}

QString ShopProfile::getName() const {
    return name;
}

QString ShopProfile::getAddress() const {
    return address;
}

QString ShopProfile::getPhone() const {
    return phone;
}

QString ShopProfile::getThankyouMessage() const {
    return thankyouMessage;
}

QString ShopProfile::getFootNote() const {
    return footNote;
}
