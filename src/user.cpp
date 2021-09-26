#include "user.h"

User::User() {

}

User:: User(qint64 id, QString username, QString fullname, QString address, QString phone, Role role):
    id(id), username(username), fullname(fullname), address(address), phone(phone), role(role) {}

QString User::getUsername() const {
    return username;
}

QString User::getFullname() const {
    return fullname;
}

QString User::getAddress() const {
    return address;
}

QString User::getPhone() const {
    return phone;
}

Role User::getRole() const {
    return role;
}

qint64 User::getId() const {
    return id;
}

User User::fromJSON(QJsonObject &obj) {
    qint64 id = obj["id"].toInt();
    QString username = obj["username"].toString();
    QString fullname = obj["full_name"].toString();
    QString address = obj["address"].toString();
    QString phone = obj["phone"].toString();
    Role role = (Role) obj["role_id"].toInt();
    return User(id, username, fullname, address, phone, role);
}
