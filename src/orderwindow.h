#ifndef ORDERWINDOW_H
#define ORDERWINDOW_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QShortcut>
#include <QMessageBox>
#include "setting.h"
#include "order.h"
#include "orderwindowmodel.h"
#include "productordermodel.h"
#include "printer.h"

namespace Ui {
class OrderWindow;
}

class OrderWindow : public QWidget {
    Q_OBJECT

public:
    explicit OrderWindow(QWidget *parent = nullptr);
    ~OrderWindow();
    void setOrderStatus(const Status &);

protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;

private slots:
    void on_btnSearch_clicked();
    void onSelectOrder(const Order &);
    void onChoosenOrder(const Order &);
    void closeDialog();

private:
    Ui::OrderWindow *ui;
    Setting setting;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    OrderWindowModel *orderModel;
    ProductOrderModel *productModel;

    Status orderStatus = Status::Pending;

    void initOrderWithFilter();
    void getOrderWithFilter(const QString invoice, const QString date);
    QByteArray parseToJSONPayload(const QString invoice, const QString date);
    void initTableOrderList();
    void initTableOrderDetail();

    void cancelSelectedOrder(Order &);
    void joinOrder(Order &);
    void showReprintDialog(Order &);
    void reprintOrder(Order &);
    void showErrorDialog(const QString &message);
signals:
    void onChoosenPendingOrder(const Order &);
    void onCloseEvent();
};

#endif // ORDERWINDOW_H
