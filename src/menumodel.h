#ifndef MENUMODEL_H
#define MENUMODEL_H

#include <QAbstractTableModel>

class MenuModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit MenuModel(QObject *parent = nullptr);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
    QString menus[11][2] = {{"BAYAR", "+"}, {"RUBAH HARGA", "F7"}, {"RUBAH QTY", "F8"}, {"BATAL SEMUA TRANSAKSI", "HOME"},
        {"HOLD/MENUNDA TRANSAKSI", "F12"}, {"REPRINT NOTA", "PAGE UP"}, {"CATAT UANG MASUK", "CTRL+F7"},
        {"CATAT UANG KELUAR", "CTRL+F8"},{"GABUNG BILL", "CTRL+F10"}, {"INFORMASI TRANSAKSI", "CTRL+F11"}, {"OPEN DRAWER", "="},
    };

public slots:
    void onEnterPressed(const QModelIndex &);

signals:
    void onSelectIndex(int index);
};

#endif // MENUMODEL_H
