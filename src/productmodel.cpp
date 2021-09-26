#include "productmodel.h"

ProductModel::ProductModel(QObject *parent) : QAbstractTableModel(parent) {
    products.append(Product());
    connect(this, SIGNAL(changeQtyCompleted(qint64, int)), this, SLOT(changeQty(qint64, int)));
    connect(this, SIGNAL(onSelectProduct(Product)), this, SLOT(onAddProduct(Product)));
    connect(this, SIGNAL(clearModelPressed()), this, SLOT(onClearModel()));
    connect(this, SIGNAL(changeSellPriceCompleted(qint64, int)), this, SLOT(changeSellPrice(qint64, int)));
}

void ProductModel::setProductData(const QVector<Product> &products) {
    beginRemoveRows(QModelIndex(), rowCount() - 1, rowCount() - 1);
    this->products.removeAt(rowCount() - 1);
    endRemoveRows();
    for (int row = 0; row < products.count(); row++) {
        if (!checkAndUpdateSameProduct(products[row])) {
            beginInsertRows(QModelIndex(), row, row);
            this->products.append(products[row]);
            endInsertRows();
        }
    }
    addProduct(Product());
}

QVector<Product> ProductModel::getProducts() {
    return products;
}

void ProductModel::addProduct(Product p) {
    int newRow = products.count();
    beginInsertRows(QModelIndex(), newRow, newRow);
    products.append(p);
    endInsertRows();
    emit editCompleted(false);
}

void ProductModel::changeQty(const qint64 &index, const int &newQty) {
    Product product = products[index];
    product.setQty(newQty);
    products[index] = product;
    emit editCompleted(false);
}

void ProductModel::onAddProduct(const Product &product) {
    if (checkAndUpdateSameProduct(product)) return;
    products[products.count() - 1] = product;
    addProduct(Product());
}

void ProductModel::onDeleteProduct(const QModelIndex &index) {
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    products.removeAt(index.row());
    endRemoveRows();
    emit editCompleted(true);
}

void ProductModel::onClearModel() {
    beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
    products.clear();
    endRemoveRows();
    addProduct(Product());
}

void ProductModel::changeSellPrice(const qint64 &index, const int &sellPrice) {
    Product product = products[index];
    product.setSellPrice(sellPrice);
    products[index] = product;
    emit editCompleted(false);
}

int ProductModel::rowCount(const QModelIndex & /*parent*/) const {
    return products.count();
}

int ProductModel::columnCount(const QModelIndex & /*parent*/) const {
    return 6;
}

QVariant ProductModel::data(const QModelIndex &index, int role) const {
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
            if (row == products.count() - 1)
                return QString();
            return QString::number(product.getSellPrice());
        case 3:
            if (row == products.count() - 1)
                return QString();
            return QString::number(product.getQty());
        case 4:
            if (row == products.count() - 1)
                return QString();
            return QString::number(product.getDiscount());
        case 5:
            if (row == products.count() - 1)
                return QString();
            return QString::number(product.getTotal());
        }
    }

    return QVariant();
}

QVariant ProductModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("Kode");
        case 1:
            return QString("Nama");
        case 2:
            return QString("Harga");
        case 3:
            return QString("QTY");
        case 4:
            return QString("Discount");
        case 5:
            return QString("Total");
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return QString::number(section + 1);
    return QVariant();
}

bool ProductModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (role == Qt::EditRole) {
        if (!checkIndex(index))
            return false;
        if (index.row() != products.count() - 1 || index.column() != 1)
            return false;
        result = value.toString();
        if (result == "")
            return false;
        QStringList qtyCode = result.split("*");
        int qty = 1;
        QString code = result;
        if (qtyCode.length() > 1) {
            qty = qtyCode.at(0).toInt();
            code = qtyCode.at(1);
        }

        manager = new QNetworkAccessManager();
        Setting settings = Setting();
        QString url  = settings.getApi();
        request.setUrl(QUrl(QString("%1/api/products?query=%2&page=1&limit=100").arg(url, code)));
        request.setTransferTimeout(5000);
        manager->get(request);
        QObject::connect(manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            manager->deleteLater();
            QString answer = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
            QJsonObject obj = doc.object();
            int code = obj["code"].toInt();
            if (code == 404) {
                emit productNotFound();
            } else if (code == 200) {
                QVector<Product> result;
                QJsonArray productsArray = obj["data"].toObject()["products"].toArray();
                if (productsArray.count() < 1) {
                    emit productNotFound();
                    return;
                }
                for (auto v : productsArray) {
                    QJsonObject element = v.toObject();
                    Product p = Product::fromJSON(element);

                    result.append(p);
                }

                if (result.count() == 1) {
                    if (checkAndUpdateSameProduct(result[0])) return;
                    result[0].setQty(qty);
                    products[products.count() - 1] = result[0];
                    addProduct(Product());
                } else if (result.count() > 1) {
                    emit openProductDialog(result);
                }
            } else {
                if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                    emit timeoutError();
                    return;
                }
                QString message = obj["message"].toString();
                emit otherError(message);
            }
        });

        return true;
    }
    return false;
}

Qt::ItemFlags ProductModel::flags(const QModelIndex &index) const {
    if (index.row() == products.count() -1 && index.column() == 1)
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

bool ProductModel::checkAndUpdateSameProduct(const Product &product) {
    for (int i = 0; i < products.count(); i++) {
        if (products[i].getID() == product.getID()) {
            Product tempProduct = products[i];
            products[i] = product;
            products[i].setQty(tempProduct.getQty() + 1);
            emit editCompleted(false);
            return true;
        }
    }
    return false;
}
