#ifndef MONEY_H
#define MONEY_H

#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>

enum Type {
    Out = -1,
    In  = 1,
};

class Money {
private:
    Type type;
    double amount;
    QString note;
    QDateTime createTime;

public:
    Money();
    Money(Type type, double amount, QString note);
    Money(Type type, double amount, QString note, QDateTime createTime);
    static Money fromJSON(QJsonObject &);
    Type getType() const;
    double getAmount() const;
    QString getNote() const;
    QDateTime getCreateTime() const;
    QString getCreateTimeFmt() const;

    QByteArray parseToJSONPayload();
};

#endif // MONEY_H
