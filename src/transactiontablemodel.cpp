#include "transactiontablemodel.h"

TransactionTableModel::TransactionTableModel(QObject *) {

}

void TransactionTableModel::setTransactionData(const QVector<Money> &monies) {
    if (rowCount() > 0) {
        beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
        this->monies.clear();
        endRemoveRows();
    }
    for (int row = 0; row < monies.count(); row++) {
        beginInsertRows(QModelIndex(), row, row);
        this->monies.append(monies[row]);
        endInsertRows();
    }
}

Money TransactionTableModel::getData(const int index) {
    return monies[index];
}

int TransactionTableModel::rowCount(const QModelIndex &) const {
    return monies.length();
}

int TransactionTableModel::columnCount(const QModelIndex &) const {
    return 4;
}

QVariant TransactionTableModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();
    Money money = monies.at(row);
    QLocale indonesian(QLocale::Indonesian, QLocale::Indonesia);

    if (role == Qt::DisplayRole) {
        switch (col) {
        case 0:
            if (money.getType() == Type::In) return "Uang masuk";
            return "Uang keluar";
        case 1:
            return QString("Rp %1").arg(indonesian.toString((int) money.getAmount()));
        case 2:
            return money.getNote();
        case 3:
            return money.getCreateTimeFmt();
        }
    }

    return QVariant();
}

QVariant TransactionTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("Tipe Transaksi");
        case 1:
            return QString("Jumlah Uang");
        case 2:
            return QString("Catatan");
        case 3:
            return QString("Tanggal - Jam");
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return QString::number(section + 1);
    return QVariant();
}


