#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QWidget>
#include <QDebug>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include "productselectmodel.h"
#include "setting.h"

namespace Ui {
class ProductDialog;
}

class ProductDialog : public QWidget {
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = nullptr);
    ~ProductDialog();

    void setProducts(const QVector<Product> &, const int &, const QString &query);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ProductDialog *ui;
    ProductSelectModel *model;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    int qty = 1, page = 2;
    QString query = "";

    void showErrorDialog(const QString &message);

private slots:
    void onEnterPressed(const QModelIndex &);
    void onEscPressed();
    void onFetchMore();

    void on_btnCancel_clicked();

    void on_btnSelect_clicked();

signals:
    void onSelectProduct(const Product &);
    void onCloseEvent();
};

#endif // PRODUCT_DIALOG_H
