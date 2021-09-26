#ifndef MONEY_H
#define MONEY_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>

enum Type {
    Out = -1,
    In  = 1,
};

class Money {
private:
    Type type;
    double amount;
    QString note;

public:
    Money();
    Money(Type type, double amount, QString note);
    Type getType() const;
    double getAmount() const;
    QString getNote() const;

    QByteArray parseToJSONPayload();
};

#endif // MONEY_H
