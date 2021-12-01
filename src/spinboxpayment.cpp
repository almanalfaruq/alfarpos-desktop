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

void SpinBoxPayment::keyPressEvent(QKeyEvent *event) {
    QSpinBox::keyPressEvent(event);
    setupSeparator();
}

void SpinBoxPayment::setupSeparator() {
    int val = valueFromText(text());
    clearFocus();
    setFocus();
    setValue(val);
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier);
    QSpinBox::keyPressEvent(event);
}
