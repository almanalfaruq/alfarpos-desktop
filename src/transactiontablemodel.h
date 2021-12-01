#ifndef TRANSACTIONTABLEMODEL_H
#define TRANSACTIONTABLEMODEL_H

#include <QAbstractTableModel>
#include "money.h"

class TransactionTableModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit TransactionTableModel(QObject *parent = nullptr);
    void setTransactionData(const QVector<Money> &);
    Money getData(const int index);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QVector<Money> monies;
};

#endif // TRANSACTIONTABLEMODEL_H
