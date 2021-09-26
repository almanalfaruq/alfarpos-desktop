#ifndef ORDERWINDOWMODEL_H
#define ORDERWINDOWMODEL_H

#include <QAbstractTableModel>
#include "order.h"

class OrderWindowModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit OrderWindowModel(QObject *parent = nullptr);
    void setOrderData(const QVector<Order> &);
    QVector<Order> getOrders();

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

public slots:
    void onSelectionChange(const QModelIndex &);
    void onEnterPressed(const QModelIndex &);

signals:
    void onSelectOrder(const Order &);
    void onChoosenOrder(const Order &);

private:
    QVector<Order> orders;
};

#endif // ORDERWINDOWMODEL_H
