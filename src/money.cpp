#include "money.h"

Money::Money() {

}

Money::Money(Type type, double amount, QString note) :
    type(type), amount(amount), note(note) {}

Type Money::getType() const {
    return type;
}

double Money::getAmount() const {
    return amount;
}

QString Money::getNote() const {
    return note;
}

QByteArray Money::parseToJSONPayload() {
    QJsonObject moneyObj;
    moneyObj.insert("type", getType());
    moneyObj.insert("amount", getAmount());
    moneyObj.insert("note", getNote());
    QJsonDocument doc(moneyObj);
    return doc.toJson();
}
