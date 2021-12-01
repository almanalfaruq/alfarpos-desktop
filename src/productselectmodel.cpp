#include "productselectmodel.h"

ProductSelectModel::ProductSelectModel(QObject *parent) : QAbstractTableModel(parent) {
}

void ProductSelectModel::addProductData(const QVector<Product> &products) {
    int newRow = rowCount();
    for (int row = 0; row < products.count(); row++) {
        beginInsertRows(QModelIndex(), newRow, newRow);
        this->products.append(products[row]);
        endInsertRows();
        newRow++;
    }
}

void ProductSelectModel::setHasNext(const bool &hasNext) {
    this->hasNext = hasNext;
}

Product ProductSelectModel::getData(const int index) {
    return products[index];
}

int ProductSelectModel::rowCount(const QModelIndex & /*parent*/) const {
    return products.count();
}

int ProductSelectModel::columnCount(const QModelIndex & /*parent*/) const {
    return 5;
}

QVariant ProductSelectModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    Product product = products.at(row);

    if (role == Qt::DisplayRole) {
        switch (col) {
        case 0:
            return QString(product.getCode());
        case 1:
            return QString(product.getName());
        case 2:
            return QString::number(product.getSellPriceStock());
        case 3:
            return QString::number(product.getQty());
        case 4:
            return product.getUnitName();
        }
    }

    return QVariant();
}

QVariant ProductSelectModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("Kode");
        case 1:
            return QString("Nama Barang");
        case 2:
            return QString("Harga Jual");
        case 3:
            return QString("Stok");
        case 4:
            return QString("Satuan");
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return QString::number(section + 1);
    return QVariant();
}

void ProductSelectModel::onSelectionChange(const QModelIndex &index) {
    int row = index.row();
    if (rowCount() < 1 || row < 0) {
        return;
    }
    if (row >= rowCount() - 5 && hasNext) emit onFetchMore();
}
