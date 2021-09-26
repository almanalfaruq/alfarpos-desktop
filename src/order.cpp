#include "order.h"

Status Order::getStatus() const {
    return status;
}

void Order::setStatus(const Status &value) {
    status = value;
}

QByteArray Order::parseToJSONOrder(Status status) {
    QJsonArray orderDetailsArr;
    for (auto od : getOrderDetails()) {
        QJsonObject productOrderObj;
        productOrderObj.insert("sell_price", od.getProduct().getSellPrice());
        productOrderObj.insert("product_id", od.getProduct().getID());

        QJsonObject orderDetailObj;
        orderDetailObj.insert("quantity", od.getQty());
        orderDetailObj.insert("sub_total", od.getSubTotal());
        orderDetailObj.insert("use_special_price", od.isUsingSpecialPrice());
        orderDetailObj.insert("product_order", productOrderObj);
        orderDetailsArr.push_back(orderDetailObj);
    }

    QJsonObject orderObj;
    orderObj.insert("id", getId());
    orderObj.insert("amount_paid", getAmountPaid());
    orderObj.insert("customer_id", getCustomerID());
    orderObj.insert("user_id", getUserID());
    orderObj.insert("total", getTotal());
    orderObj.insert("total_change", getTotalChange());
    orderObj.insert("payment_id", getPaymentID());
    orderObj.insert("status", status);
    orderObj.insert("order_details", orderDetailsArr);

    QJsonDocument doc(orderObj);

    return doc.toJson();
}

Order::Order() {

}

Order::Order(qint64 userID, qint64 customerID, Status status, qint64 total, qint64 amountPaid, qint64 totalChange, qint64 paymentID,
             QString invoice, QVector<OrderDetail> orderDetails) :
    userID(userID), customerID(customerID), status(status), total(total), amountPaid(amountPaid), totalChange(totalChange), paymentID(paymentID), invoice(invoice),
    orderDetails(orderDetails) {}

Order::Order(qint64 userID, qint64 customerID, Status status, qint64 total, qint64 amountPaid, qint64 totalChange, qint64 paymentID,
             QVector<OrderDetail> orderDetails) :
    userID(userID), customerID(customerID), status(status), total(total), amountPaid(amountPaid), totalChange(totalChange), paymentID(paymentID),
    orderDetails(orderDetails) {}

Order::Order(qint64 id, qint64 userID, User user, qint64 customerID, Status status, qint64 total, qint64 amountPaid, qint64 totalChange,
             qint64 paymentID, QString invoice, QVector<OrderDetail> orderDetails, QDateTime dateTime) :
    id(id), userID(userID), user(user), customerID(customerID), status(status), total(total), amountPaid(amountPaid), totalChange(totalChange),
    paymentID(paymentID), invoice(invoice), orderDetails(orderDetails), dateTime(dateTime) {}

User Order::getUser() const {
    return user;
}

QDateTime Order::getDateTime() const {
    return dateTime;
}

qint64 Order::getId() const {
    return id;
}

void Order::setId(const qint64 &value) {
    id = value;
}

qint64 Order::getPaymentID() const {
    return paymentID;
}

qint64 Order::getTotalChange() const {
    return totalChange;
}

qint64 Order::getUserID() const {
    return userID;
}

qint64 Order::getCustomerID() const {
    return customerID;
}

qint64 Order::getTotal() const {
    return total;
}

QString Order::getTotalFormatted() const {
    QLocale indonesian(QLocale::Indonesian, QLocale::Indonesia);
    return QString("Rp %1").arg(indonesian.toString(total));
}

qint64 Order::getAmountPaid() const {
    return amountPaid;
}

QString Order::getInvoice() const {
    return invoice;
}

void Order::setInvoice(const QString &value) {
    invoice = value;
}

QVector<OrderDetail> Order::getOrderDetails() const {
    return orderDetails;
}

Order Order::fromJSON(QJsonObject &obj) {
    qint64 id = obj["id"].toInt();
    QString invoice = obj["invoice"].toString();
    Status status = (Status) obj["status"].toInt();
    qint64 total = obj["total"].toInt();
    qint64 totalChange = obj["total_change"].toInt();
    qint64 amountPaid = obj["amount_paid"].toInt();
    QJsonObject userObj = obj["user"].toObject();
    User user = User::fromJSON(userObj);
    QJsonObject custObj = obj["customer"].toObject();
    Customer customer = Customer::fromJSON(custObj);
    QJsonObject paymentObj = obj["payment"].toObject();
    Payment payment = Payment::fromJSON(paymentObj);
    QVector<OrderDetail> orderDetails;
    QJsonArray orderDetailsArray = obj["order_details"].toArray();
    QString dtString = obj["created_at"].toString();
    QDateTime dt = QDateTime::fromString(dtString.left(23), "yyyy-MM-ddTHH:mm:ss.zzz");
    for (auto odArray : orderDetailsArray) {
        QJsonObject odObj = odArray.toObject();
        OrderDetail od = OrderDetail::fromJson(odObj);
        orderDetails.append(od);
    }
    return Order(id, user.getId(), user, customer.getId(), status, total, amountPaid, totalChange, payment.getId(), invoice, orderDetails, dt);
}

OrderDetail::OrderDetail(Product product, qint32 qty, qint64 subTotal, bool useSpecialPrice)
    : product(product), qty(qty), subTotal(subTotal), useSpecialPrice(useSpecialPrice) {}

qint32 OrderDetail::getQty() const {
    return qty;
}

qint64 OrderDetail::getSubTotal() const {
    return subTotal;
}

QString OrderDetail::getSubTotalFormatted() const {
    QLocale indonesian(QLocale::Indonesian, QLocale::Indonesia);
    return indonesian.toString(getSubTotal());
}

Product OrderDetail::getProduct() const {
    return product;
}

bool OrderDetail::isUsingSpecialPrice() const {
    return useSpecialPrice;
}

OrderDetail OrderDetail::fromJson(QJsonObject &obj) {
    QJsonObject productObj = obj["product"].toObject();
    Product product = Product::fromJSON(productObj);
    QJsonObject productOrderObj = obj["product_order"].toObject();
    qint64 sellPrice = productOrderObj["sell_price"].toInt();
    qint64 qty = obj["quantity"].toInt();
    qint64 subTotal = obj["sub_total"].toInt();
    bool useSpecialPrice = obj["use_special_price"].toBool();
    product.setSellPrice(sellPrice);
    product.setQty(qty);
    return OrderDetail(product, qty, subTotal, useSpecialPrice);
}
