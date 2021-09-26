#ifndef PRODUCTDIALOG_H
#define PRODUCTDIALOG_H

#include <QWidget>
#include "productselectmodel.h"
#include <QDebug>

namespace Ui {
class ProductDialog;
}

class ProductDialog : public QWidget {
    Q_OBJECT

public:
    explicit ProductDialog(QWidget *parent = nullptr);
    ~ProductDialog();

    void setProducts(const QVector<Product> &);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::ProductDialog *ui;
    ProductSelectModel *model;

private slots:
    void onEnterPressed(const QModelIndex &);
    void onEscPressed();

    void on_btnCancel_clicked();

    void on_btnSelect_clicked();

signals:
    void onSelectProduct(const Product &);
    void onCloseEvent();
};

#endif // PRODUCT_DIALOG_H
