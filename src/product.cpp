#include "product.h"

Product::Product():
    id(0), buyPrice(0), sellPrice(0), qty(0), code(""), name(""), unitName(""), discount(0) {}

Product::Product(qint64 id, qint64 buyPrice, qint64 sellPrice, qint32 qty, QString code, QString name, QString imageURL,
                 float discount, bool isOpenPrice):
    id(id), buyPrice(buyPrice), sellPrice(sellPrice), qty(qty), code(code), name(name), unitName(imageURL),
    discount(discount), isOpenPrice(isOpenPrice) {}

qint64 Product::getID() const {
    return id;
}

void Product::setID(qint64 id) {
    this->id = id;
}

qint64 Product::getBuyPrice() const {
    return buyPrice;
}

void Product::setBuyPrice(qint64 buyPrice) {
    this->buyPrice = buyPrice;
}

qint64 Product::getSellPrice() const {
    return sellPrice;
}

qint64 Product::getSellPriceStock() const {
    return sellPrice;
}

QString Product::getSellPriceFormatted() const {
    QString str = QString::number(getSellPrice());
    str.replace(QRegularExpression("\\B(?=(\\d{3})+(?!\\d))"), ".");
    return str;
}

void Product::setSellPrice(qint64 sellPrice) {
    this->sellPrice = sellPrice;
    this->isSellPriceChanged = true;
}

qint32 Product::getQty() const {
    return qty;
}

void Product::setQty(qint32 qty) {
    this->qty = qty;
}

QString Product::getCode() const {
    return code;
}

void Product::setCode(QString code) {
    this->code = code;
}

QString Product::getName() const {
    return name;
}

void Product::setName(QString name) {
    this->name = name;
}

QString Product::getUnitName() const {
    return unitName;
}

void Product::setUnitName(QString unitName) {
    this->unitName = unitName;
}

float Product::getDiscount() const {
    return discount;
}

void Product::setDiscount(float discount) {
    this->discount = discount;
}

qint64 Product::getTotal() const {
    qint64 total = 0;
    if (isSellPriceChanged || productPrices.count() == 0) {
        total = getSellPrice() * getQty();
    } else {
        for (auto pp : getProductPrices()) {
            if (pp.getMultiplier() > getQty()) {
                continue;
            } else {
                // Code for multiple price
                if (pp.getPricePerUnit() > 0) {
                    total = getQty() * pp.getPricePerUnit();
                } else {
                    int modulo = getQty() % pp.getMultiplier();
                    qint64 specialTotalPrice = (getQty() - modulo) * pp.getPricePerPacket();
                    total = specialTotalPrice + (modulo * getSellPrice());
                }
                break;
            }
        }
    }

    if (total == 0) {
        total = getSellPrice() * getQty();
    }

    if (getDiscount() > 0)
        return total - (getDiscount() * total);

    return total;
}

QString Product::getTotalFormatted() const {
    QString str = QString::number(getTotal());
    str.replace(QRegularExpression("\\B(?=(\\d{3})+(?!\\d))"), ".");
    return str;
}

void Product::setProductPrices(QVector<ProductPrice> productPrices) {
    this->productPrices = productPrices;
}

QVector<ProductPrice> Product::getProductPrices() const {
    return productPrices;
}

bool Product::isUsingSpecialPrice() const {
    qint64 totalFromQty = getSellPrice() * getQty();
    return totalFromQty != getTotal();
}

bool Product::getIsOpenPrice() const {
    return isOpenPrice;
}

Product Product::fromJSON(QJsonObject &obj) {
    qint64 id = obj["id"].toInt();
    qint64 buyPrice = obj["buy_price"].toObject()["Int64"].toInt();
    qint64 sellPrice = obj["sell_price"].toObject()["Int64"].toInt();
    qint64 stock = obj["quantity"].toObject()["Int64"].toInt();
    QString code = obj["code"].toObject()["String"].toString();
    QString name = obj["name"].toString();
    QString unitName = obj["unit"].toObject()["name"].toString();
    float discount = obj["discount"].toDouble();
    bool isOpenPrice = obj["is_open_price"].toBool();
    Product product = Product(id, buyPrice, sellPrice, stock, code, name,
                              unitName, discount, isOpenPrice);

    // special price
    QJsonArray productPricesJson = obj["product_prices"].toArray();
    QVector<ProductPrice> prices;
    for (auto x : productPricesJson) {
        QJsonObject element = x.toObject();
        qint32 multiplier = element["quantity_multiplier"].toInt();
        QJsonObject objPricePerUnit = element["price_per_unit"].toObject();
        qint64 pricePerUnit = objPricePerUnit["Int64"].toInt();
        QJsonObject objPricePerPacket = element["price_per_packet"].toObject();
        qint64 pricePerPacket = objPricePerPacket["Int64"].toInt();
        prices.append(ProductPrice(pricePerUnit, pricePerPacket, multiplier));
    }
    if (prices.count() != 0) {
        product.setProductPrices(prices);
    }
    return product;
}

ProductPrice::ProductPrice(qint64 pricePerUnit, qint64 pricePerPacket, qint32 multiplier):
    pricePerUnit(pricePerUnit), pricePerPacket(pricePerPacket), multiplier(multiplier) {}

qint64 ProductPrice::getPricePerUnit() const {
    return pricePerUnit;
}

qint64 ProductPrice::getPricePerPacket() const {
    return pricePerPacket;
}

qint32 ProductPrice::getMultiplier() const {
    return multiplier;
}
