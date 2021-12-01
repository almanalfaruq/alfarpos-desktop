#ifndef SPINBOXPAYMENT_H
#define SPINBOXPAYMENT_H

#include <QSpinBox>
#include <QChar>
#include <QDebug>
#include <QKeyEvent>

class SpinBoxPayment : public QSpinBox {
    Q_OBJECT
public:
    explicit SpinBoxPayment(QWidget *parent = nullptr);

protected:
    virtual QString textFromValue(int value) const override;
    virtual int valueFromText(const QString &text) const override;
    virtual void keyPressEvent(QKeyEvent *event) override;
private:
    QString reverse(QString what) const;
    QString addToRd(QString what, QChar add, int kdabir) const;
    void setupSeparator();
};

#endif // SPINBOXPAYMENT_H
