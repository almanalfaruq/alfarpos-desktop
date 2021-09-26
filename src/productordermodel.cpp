#include "productordermodel.h"

ProductOrderModel::ProductOrderModel(QObject *parent) : QAbstractTableModel(parent) {

}

void ProductOrderModel::setProductData(const QVector<Product> &products) {
    if (rowCount() > 0 ) {
        clearModel();
    }
    for (int row = 0; row < products.count(); row++) {
        beginInsertRows(QModelIndex(), row, row);
        this->products.append(products[row]);
        endInsertRows();
    }
}

void ProductOrderModel::clearModel() {
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    products.clear();
    endRemoveRows();
}

QVector<Product> ProductOrderModel::getProducts() {
    return products;
}

int ProductOrderModel::rowCount(const QModelIndex & /*parent*/) const {
    return products.count();
}

int ProductOrderModel::columnCount(const QModelIndex & /*parent*/) const {
    return 5;
}

QVariant ProductOrderModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    Product product = products.at(row);

    if (role == Qt::DisplayRole) {
        switch (col) {
        case 0:
            return product.getName();
        case 1:
            return product.getQty();
        case 2:
            return product.getUnitName();
        case 3:
            return product.getDiscount();
        case 4:
            return product.getTotal();
        }
    }

    return QVariant();
}

QVariant ProductOrderModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("Barang");
        case 1:
            return QString("QTY");
        case 2:
            return QString("Satuan");
        case 3:
            return QString("%Disc");
        case 4:
            return QString("Jumlah");
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return QString::number(section + 1);
    return QVariant();
}
