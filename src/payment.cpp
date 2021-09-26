#include "payment.h"

Payment::Payment() {}

Payment::Payment(qint64 id, QString name) :
    id(id), name(name) {}

qint64 Payment::getId() const {
    return id;
}

QString Payment::getName() const {
    return name;
}

Payment Payment::fromJSON(QJsonObject &obj) {
    qint64 id = obj["id"].toInt();
    QString name = obj["name"].toString();
    return Payment(id, name);
}
