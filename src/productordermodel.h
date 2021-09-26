#ifndef PRODUCTORDERMODEL_H
#define PRODUCTORDERMODEL_H

#include <QAbstractTableModel>
#include "product.h"
#include <QDebug>

class ProductOrderModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProductOrderModel(QObject *parent = nullptr);
    void setProductData(const QVector<Product> &);
    QVector<Product> getProducts();

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<Product> products;
    void clearModel();
};

#endif // PRODUCTORDERMODEL_H
