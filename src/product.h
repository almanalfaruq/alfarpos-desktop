#ifndef PRODUCT_H
#define PRODUCT_H

#include "QString"
#include "QVector"
#include <QRegularExpression>
#include <QJsonObject>
#include <QJsonArray>

class ProductPrice;

class Product {
private:
    qint64 id, buyPrice, sellPrice;
    qint32 qty;
    QString code, name, unitName;
    float discount;
    QVector<ProductPrice> productPrices;
    bool isSellPriceChanged = false, isOpenPrice = false;

public:
    Product();
    Product(qint64 id, qint64 buyPrice, qint64 sellPrice, qint32 qty, QString code, QString name, QString unitName,
            float discount, bool isOpenPrice);
    qint64 getID() const;
    void setID(qint64 id);

    qint64 getBuyPrice() const;
    void setBuyPrice(qint64 buyPrice);

    qint64 getSellPrice() const;
    qint64 getSellPriceStock() const;
    QString getSellPriceFormatted() const;
    void setSellPrice(qint64 sellPrice);

    qint32 getQty() const;
    void setQty(qint32 qty);

    QString getCode() const;
    void setCode(QString code);

    QString getName() const;
    void setName(QString name);

    QString getUnitName() const;
    void setUnitName(QString unitName);

    float getDiscount() const;
    void setDiscount(float discount);

    qint64 getTotal() const;
    QString getTotalFormatted() const;

    void setProductPrices(QVector<ProductPrice> productPrices);
    QVector<ProductPrice> getProductPrices() const;

    bool isUsingSpecialPrice() const;
    static Product fromJSON(QJsonObject &);
    bool getIsOpenPrice() const;
};

class ProductPrice {
private:
    qint64 pricePerUnit;
    qint64 pricePerPacket;
    qint32 multiplier;

public:
    ProductPrice(qint64 pricePerUnit, qint64 pricePerPacket, qint32 multiplier);

    qint64 getPricePerUnit() const;
    qint64 getPricePerPacket() const;
    qint32 getMultiplier() const;
};

#endif // PRODUCT_H
