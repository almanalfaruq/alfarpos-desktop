#ifndef PRODUCTSELECTMODEL_H
#define PRODUCTSELECTMODEL_H

#include <QAbstractTableModel>
#include "product.h"

class ProductSelectModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProductSelectModel(QObject *parent = nullptr);
    void setProductData(const QVector<Product> &);
    Product getData(const int index);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<Product> products;

signals:

};

#endif // PRODUCTSELECTMODEL_H
