#include "money.h"

Money::Money() {

}

Money::Money(Type type, double amount, QString note) :
    type(type), amount(amount), note(note) {}

Money::Money(Type type, double amount, QString note, QDateTime createTime) : type(type), amount(amount), note(note), createTime(createTime) {

}

Money Money::fromJSON(QJsonObject &obj) {
    Type type = (Type) obj["type"].toInt();
    double amount = obj["amount"].toDouble();
    QString note = obj["note"].toString();
    QString createTime = obj["created_at"].toString();
    QDateTime ctFmt = QDateTime::fromString(createTime.left(23), "yyyy-MM-ddTHH:mm:ss.zzz");
    return Money(type, amount, note, ctFmt);
}

Type Money::getType() const {
    return type;
}

double Money::getAmount() const {
    return amount;
}

QString Money::getNote() const {
    return note;
}

QDateTime Money::getCreateTime() const {
    return createTime;
}

QString Money::getCreateTimeFmt() const {
    return createTime.toString("dd-MM-yyyy hh:mm:ss");
}

QByteArray Money::parseToJSONPayload() {
    QJsonObject moneyObj;
    moneyObj.insert("type", getType());
    moneyObj.insert("amount", getAmount());
    moneyObj.insert("note", getNote());
    QJsonDocument doc(moneyObj);
    return doc.toJson();
}
