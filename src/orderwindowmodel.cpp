#include "orderwindowmodel.h"

OrderWindowModel::OrderWindowModel(QObject *parent) : QAbstractTableModel(parent) {

}

void OrderWindowModel::setOrderData(const QVector<Order> &orders) {
    for (int row = 0; row < orders.count(); row++) {
        beginInsertRows(QModelIndex(), row, row);
        this->orders.append(orders[row]);
        endInsertRows();
    }
}

QVector<Order> OrderWindowModel::getOrders() {
    return orders;
}

int OrderWindowModel::rowCount(const QModelIndex & /*parent*/) const {
    return orders.count();
}

int OrderWindowModel::columnCount(const QModelIndex & /*parent*/) const {
    return 5;
}

QVariant OrderWindowModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    Order order = orders.at(row);

    if (role == Qt::DisplayRole) {
        switch (col) {
        case 0:
            return order.getUser().getFullname();
        case 1:
            return order.getDateTime().toString("HH:mm:ss dd-MM-yyyy");
        case 2:
            return order.getInvoice();
        case 3:
            return order.getCustomerID();
        case 4:
            return order.getTotalFormatted();
        }
    }

    return QVariant();
}

QVariant OrderWindowModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("User");
        case 1:
            return QString("Jam - Tanggal");
        case 2:
            return QString("Invoice");
        case 3:
            return QString("Customer");
        case 4:
            return QString("Total");
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return QString::number(section + 1);
    return QVariant();
}

void OrderWindowModel::onSelectionChange(const QModelIndex &index) {
    emit onSelectOrder(orders.at(index.row()));
}

void OrderWindowModel::onEnterPressed(const QModelIndex &index) {
    emit onChoosenOrder(orders.at(index.row()));
}
