#ifndef PRODUCTSELECTMODEL_H
#define PRODUCTSELECTMODEL_H

#include <QAbstractTableModel>
#include "product.h"

class ProductSelectModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProductSelectModel(QObject *parent = nullptr);
    void addProductData(const QVector<Product> &);
    void setHasNext(const bool &hasNext);
    Product getData(const int index);

public slots:
    void onSelectionChange(const QModelIndex &index);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<Product> products;
    bool hasNext;

signals:
    void onFetchMore();
};

#endif // PRODUCTSELECTMODEL_H
