#include "product_dialog.h"
#include "ui_product_dialog.h"

ProductDialog::ProductDialog(QWidget *parent) :
    QWidget(parent), ui(new Ui::ProductDialog) {
    ui->setupUi(this);

    ui->pbWait->setVisible(false);
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

    connect(ui->tvSelectProduct, SIGNAL(onEnterPressed(QModelIndex)), this, SLOT(onEnterPressed(QModelIndex)), Qt::QueuedConnection);
    connect(ui->tvSelectProduct, SIGNAL(onEscPressed()), this, SLOT(onEscPressed()), Qt::QueuedConnection);
    connect(ui->tvSelectProduct, SIGNAL(onSelectionChange(QModelIndex)), model, SLOT(onSelectionChange(QModelIndex)), Qt::QueuedConnection);
    connect(model, SIGNAL(onFetchMore()), this, SLOT(onFetchMore()), Qt::QueuedConnection);
}

void ProductDialog::setProducts(const QVector<Product> &products, const int &qty, const QString &query) {
    model->addProductData(products);
    this->qty = qty;
    this->query = query;
    ui->tvSelectProduct->setFocus();
    ui->tvSelectProduct->selectRow(0);
}

ProductDialog::~ProductDialog() {
    delete ui;
}

void ProductDialog::onEnterPressed(const QModelIndex &index) {
    if (ui->tvSelectProduct->model()->rowCount() < 1) return;
    Product p = model->getData(index.row());
    p.setQty(qty);
    this->close();
    emit onSelectProduct(p);
}

void ProductDialog::onEscPressed() {
    this->close();
}

void ProductDialog::onFetchMore() {
    manager = new QNetworkAccessManager();
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/products?query=%2&page=%3&limit=20").arg(url, query).arg(page)));
    request.setTransferTimeout(5000);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    manager->get(request);
    ui->pbWait->setVisible(true);
    ui->tvSelectProduct->setDisabled(true);
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QVector<Product> result;
            QJsonObject data = obj["data"].toObject();
            bool hasNext = data["has_next"].toBool();
            QJsonArray productsArray = data["products"].toArray();
            for (auto v : productsArray) {
                QJsonObject element = v.toObject();
                Product p = Product::fromJSON(element);
                result.append(p);
            }
            model->setHasNext(hasNext);
            model->addProductData(result);
            page++;
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout. Silakan coba lagi.");
            } else {
                QString message = obj["message"].toString();
                showErrorDialog(message);
            }
        }
        ui->pbWait->setVisible(false);
        ui->tvSelectProduct->setDisabled(false);
        ui->tvSelectProduct->setFocus();
        reply->close();
    });
}

void ProductDialog::closeEvent(QCloseEvent *) {
    emit onCloseEvent();
}

void ProductDialog::on_btnCancel_clicked() {
    this->close();
}

void ProductDialog::on_btnSelect_clicked() {
    if (ui->tvSelectProduct->model()->rowCount() < 1) return;
    QModelIndex index = ui->tvSelectProduct->currentIndex();
    onEnterPressed(index);
}

void ProductDialog::showErrorDialog(const QString &message) {
    QMessageBox errorDialog;
    int ret = errorDialog.critical(this, "Error", message);
    this->setDisabled(false);
    if (ret == QMessageBox::Ok) {
        this->setDisabled(false);
    } else {
        this->setDisabled(false);
    }
}
