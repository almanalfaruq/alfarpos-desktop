#ifndef ORDER_H
#define ORDER_H

#include <QString>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "product.h"
#include "user.h"
#include "customer.h"
#include "payment.h"

class OrderDetail;

enum Status {
    Canceled = -1,
    Pending  = 1,
    Finish   = 2,
};

class Order {

private:
    qint64 id = 0, userID, customerID, total, amountPaid, totalChange, paymentID;
    User user;
    QDateTime dateTime;
    Status status;
    QString invoice;
    QVector<OrderDetail> orderDetails;

public:
    Order(qint64 userID, qint64 customerID, Status status, qint64 total, qint64 amountPaid, qint64 totalChange, qint64 paymentID,
          QString invoice, QVector<OrderDetail> orderDetails);

    Order(qint64 userID, qint64 customerID, Status status, qint64 total, qint64 amountPaid, qint64 totalChange, qint64 paymentID,
          QVector<OrderDetail> orderDetails);

    Order(qint64 id, qint64 userID, User user, qint64 customerID, Status status, qint64 total, qint64 amountPaid, qint64 totalChange,
          qint64 paymentID, QString invoice, QVector<OrderDetail> orderDetails, QDateTime dateTime);

    Order();
    qint64 getPaymentID() const;
    qint64 getTotalChange() const;
    qint64 getUserID() const;
    qint64 getCustomerID() const;
    qint64 getTotal() const;
    QString getTotalFormatted() const;
    qint64 getAmountPaid() const;
    QString getInvoice() const;
    void setInvoice(const QString &value);
    QVector<OrderDetail> getOrderDetails() const;
    qint64 getId() const;
    Status getStatus() const;
    void setStatus(const Status &value);

    QByteArray parseToJSONOrder(Status status = Finish);
    User getUser() const;
    QDateTime getDateTime() const;
    void setId(const qint64 &value);
    static Order fromJSON(QJsonObject &obj);
};

class OrderDetail {
private:
    Product product;
    qint32 qty;
    qint64 subTotal;
    bool useSpecialPrice;

public:
    OrderDetail(Product product, qint32 qty, qint64 subTotal, bool useSpecialPrice);
    Product getProduct() const;
    qint32 getQty() const;
    qint64 getSubTotal() const;
    QString getSubTotalFormatted() const;
    bool isUsingSpecialPrice() const;

    static OrderDetail fromJson(QJsonObject &);
};

#endif // ORDER_H
