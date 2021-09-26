#include "menumodel.h"

MenuModel::MenuModel(QObject *parent) : QAbstractTableModel(parent) {
}

int MenuModel::rowCount(const QModelIndex & /*parent*/) const {
    return 10;
}

int MenuModel::columnCount(const QModelIndex & /*parent*/) const {
    return 2;
}

QVariant MenuModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    int col = index.column();

    if (role == Qt::DisplayRole) {
        return menus[row][col];
    }

    return QVariant();
}

QVariant MenuModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return QString("Proses");
        case 1:
            return QString("Tombol");
        }
    } else if (orientation == Qt::Vertical && role == Qt::DisplayRole)
        return QString::number(section + 1);
    return QVariant();
}

void MenuModel::onEnterPressed(const QModelIndex &index) {
    int idx = index.row();
    emit onSelectIndex(idx);
}
