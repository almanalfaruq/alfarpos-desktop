#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QStackedWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QKeyEvent>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMessageBox>

#include "product.h"
#include "setting.h"
#include "order.h"
#include "printer.h"

namespace Ui {
class PaymentDialog;
}

class PaymentDialog : public QStackedWidget {
    Q_OBJECT

public:
    explicit PaymentDialog(QWidget *parent = nullptr);
    ~PaymentDialog();

    void setProducts(const QVector<Product> &);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::PaymentDialog *ui;

    Setting setting;
    QVector<Product> products;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    int total = 0, amount = 0, totalChange = 0;
    bool isPaymentValid;
    bool shouldPrint = false;
    const QString ActionPrint = "Cetak nota", ActionNotPrint = "Tanpa cetak nota";
    Order order;

    void processOrder();
    void getPaymentType();
    Order createOrder();
    void sendOrderToServer(const QByteArray jsonData);
    void openDialogChange(Order &);
    void cancelPayment();
    void closePayment();

    void showErrorDialog(const QString &message);
signals:
    void canceled();
    void paymentSuccess();
    void closeDialog();

private slots:
    void on_btnOK_clicked();
    void on_btnCancel_clicked();
    void on_btnClose_pressed();
    void checkAmountPaid(int value);
    void triggerMenuAction(QAction *action);
    void setChangeTotal(int index);
};

#endif // PAYMENTDIALOG_H
