#include "inputmoneytransaction.h"

InputMoneyTransaction::InputMoneyTransaction(QWidget *parent) : QDialog(parent) {
    QFormLayout *lytMain = new QFormLayout(this);

    QLabel *tLabel = new QLabel(QString("Jumlah Uang:"), this);
    txtAmount = new SpinBoxPayment(this);
    txtAmount->setMaximum(20000000);
    txtAmount->setButtonSymbols(QSpinBox::NoButtons);
    lytMain->addRow(tLabel, txtAmount);
    txtAmount->setFocus();

    tLabel = new QLabel(QString("Catatan:"), this);
    txtNote = new QLineEdit(this);
    lytMain->addRow(tLabel, txtNote);

    QDialogButtonBox *buttonBox = new QDialogButtonBox
    ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
      Qt::Horizontal, this );
    lytMain->addWidget(buttonBox);

    bool conn = connect(buttonBox, &QDialogButtonBox::accepted,
                        this, &InputMoneyTransaction::accept);
    Q_ASSERT(conn);
    conn = connect(buttonBox, &QDialogButtonBox::rejected,
                   this, &InputMoneyTransaction::reject);
    Q_ASSERT(conn);

    setLayout(lytMain);
}

QMap<QString, QString> InputMoneyTransaction::getMapString(QWidget *parent, const QString &title, bool *ok) {
    InputMoneyTransaction *dialog = new InputMoneyTransaction(parent);
    dialog->setWindowTitle(title);

    QMap<QString, QString> map;

    const int ret = dialog->exec();
    if (ok)
        *ok = !!ret;

    if (ret) {
        map["amount"] = QString("%1").arg(dialog->txtAmount->value());
        map["note"] = dialog->txtNote->text();
    }

    dialog->deleteLater();

    return map;
}
