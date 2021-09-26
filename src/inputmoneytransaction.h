#ifndef INPUTMONEYTRANSACTION_H
#define INPUTMONEYTRANSACTION_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include "spinboxpayment.h"

class InputMoneyTransaction : public QDialog {
    Q_OBJECT
public:
    explicit InputMoneyTransaction(QWidget *parent = nullptr);
    static QMap<QString, QString> getMapString(QWidget *parent, const QString &title, bool *ok = nullptr);

private:
    QLineEdit* txtNote;
    QSpinBox* txtAmount;
};

#endif // INPUTMONEYTRANSACTION_H
