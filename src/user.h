#ifndef USER_H
#define USER_H

#include <QString>
#include <QJsonObject>

enum Role {
    Admin = 1,
    Manager = 2,
    Cashier = 3,
};

class User {

private:
    QString username, fullname, address, phone;
    qint64 id;
    Role role;

public:
    User();
    User(qint64 id, QString username, QString fullname, QString address, QString phone, Role role);

    QString getUsername() const;
    QString getFullname() const;
    QString getAddress() const;
    QString getPhone() const;
    Role getRole() const;
    qint64 getId() const;

    static User fromJSON(QJsonObject &);
};

#endif // USER_H
