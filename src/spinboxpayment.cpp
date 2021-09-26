#include "spinboxpayment.h"

SpinBoxPayment::SpinBoxPayment(QWidget *parent)
    : QSpinBox(parent) {
    setPrefix(QString("Rp "));
}

QString SpinBoxPayment::textFromValue(int value) const {
    QLocale indonesian(QLocale::Indonesian, QLocale::Indonesia);
    QString str = indonesian.toString(value);
    return str;
}

int SpinBoxPayment::valueFromText(const QString &text) const {
    QString str = text;
    return str.remove(0, 3).remove('.').toInt();
}
