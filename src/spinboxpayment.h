#ifndef SPINBOXPAYMENT_H
#define SPINBOXPAYMENT_H

#include <QSpinBox>
#include <QChar>
#include <QDebug>

class SpinBoxPayment : public QSpinBox {
    Q_OBJECT
public:
    explicit SpinBoxPayment(QWidget *parent = nullptr);

protected:
    virtual QString textFromValue(int value) const override;
    virtual int valueFromText(const QString &text) const override;
private:
    QString reverse(QString what) const;
    QString addToRd(QString what, QChar add, int kdabir) const;
};

#endif // SPINBOXPAYMENT_H
