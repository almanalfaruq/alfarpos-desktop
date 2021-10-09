#ifndef SHOPPROFILE_H
#define SHOPPROFILE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ShopProfile {
public:
    ShopProfile();
    ShopProfile(QString name, QString address, QString phone, QString thankyouMessage, QString footNote);

    static ShopProfile fromJSON(QJsonObject &obj);

    QString getName() const;

    QString getAddress() const;

    QString getPhone() const;

    QString getThankyouMessage() const;

    QString getFootNote() const;

private:
    QString name, address, phone, thankyouMessage, footNote;
};

#endif // SHOPPROFILE_H
