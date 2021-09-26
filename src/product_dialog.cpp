#include "product_dialog.h"
#include "ui_product_dialog.h"

ProductDialog::ProductDialog(QWidget *parent) :
    QWidget(parent), ui(new Ui::ProductDialog) {
    ui->setupUi(this);

    connect(ui->tvSelectProduct, SIGNAL(onEnterPressed(QModelIndex)), this, SLOT(onEnterPressed(QModelIndex)), Qt::QueuedConnection);
    connect(ui->tvSelectProduct, SIGNAL(onEscPressed()), this, SLOT(onEscPressed()), Qt::QueuedConnection);

    model = new ProductSelectModel();
    ui->tvSelectProduct->setModel(model);
    for (int c = 0; c < ui->tvSelectProduct->horizontalHeader()->count(); ++c) {
        if (c == 0) {
            ui->tvSelectProduct->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        if (c == 1) {
            ui->tvSelectProduct->horizontalHeader()->resizeSection(c, 300);
            continue;
        }
        if (c == 3) {
            ui->tvSelectProduct->horizontalHeader()->resizeSection(c, 50);
            continue;
        }
        ui->tvSelectProduct->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void ProductDialog::setProducts(const QVector<Product> &products) {
    model->setProductData(products);
}

ProductDialog::~ProductDialog() {
    delete ui;
}

void ProductDialog::onEnterPressed(const QModelIndex &index) {
    Product p = model->getData(index.row());
    p.setQty(1);
    this->close();
    emit onSelectProduct(p);
}

void ProductDialog::onEscPressed() {
    this->close();
}

void ProductDialog::closeEvent(QCloseEvent *) {
    emit onCloseEvent();
}

void ProductDialog::on_btnCancel_clicked() {
    this->close();

}

void ProductDialog::on_btnSelect_clicked() {
    QModelIndex index = ui->tvSelectProduct->currentIndex();
    Product p = model->getData(index.row());
    p.setQty(1);
    this->close();
    emit onSelectProduct(p);

}
