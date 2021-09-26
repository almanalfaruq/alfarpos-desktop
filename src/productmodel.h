#ifndef PRODUCTMODEL_H
#define PRODUCTMODEL_H

#include <QAbstractTableModel>
#include "product.h"
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "setting.h"

class ProductModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit ProductModel(QObject *parent = nullptr);
    void setProductData(const QVector<Product> &);
    QVector<Product> getProducts();

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    QVector<Product> products;
    QString result;  //holds text entered into QTableView
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    void addProduct(Product p);
    bool checkAndUpdateSameProduct(const Product &);

public slots:
    void onDeleteProduct(const QModelIndex &);

private slots:
    void changeQty(const qint64 &, const int &);
    void onAddProduct(const Product &);
    void onClearModel();
    void changeSellPrice(const qint64 &, const int &);

signals:
    void editCompleted(const bool &isDelete);
    void changeQtyCompleted(const qint64 &, const int &);
    void openProductDialog(const QVector<Product> &);
    void productNotFound();
    void timeoutError();
    void otherError(const QString &);
    void onSelectProduct(const Product &);
    void clearModelPressed();
    void changeSellPriceCompleted(const qint64 &, const int &);

};

#endif // PRODUCTMODEL_H
