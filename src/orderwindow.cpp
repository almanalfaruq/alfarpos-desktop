#include "orderwindow.h"
#include "ui_orderwindow.h"

OrderWindow::OrderWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderWindow) {
    ui->setupUi(this);
}

OrderWindow::~OrderWindow() {
    delete ui;
}

void OrderWindow::setOrderStatus(const Status &status) {
    this->orderStatus = status;
}

void OrderWindow::closeEvent(QCloseEvent *) {
    emit onCloseEvent();
}

void OrderWindow::showEvent(QShowEvent *) {
    initTableOrderList();
    initTableOrderDetail();
    connect(ui->tvOrderList, SIGNAL(onSelectionChange(QModelIndex)), orderModel, SLOT(onSelectionChange(QModelIndex)), Qt::QueuedConnection);
    connect(ui->tvOrderList, SIGNAL(onEnterPressed(QModelIndex)), orderModel, SLOT(onEnterPressed(QModelIndex)), Qt::QueuedConnection);
    connect(orderModel, SIGNAL(onSelectOrder(Order)), this, SLOT(onSelectOrder(Order)), Qt::QueuedConnection);
    connect(orderModel, SIGNAL(onChoosenOrder(Order)), this, SLOT(onChoosenOrder(Order)), Qt::QueuedConnection);
    QShortcut *closeDialog = new QShortcut(QKeySequence(tr("ESC")), ui->tvOrderList);
    connect(closeDialog, SIGNAL(activated()), this, SLOT(closeDialog()));
    initOrderWithFilter();
}

void OrderWindow::initTableOrderList() {
    orderModel = new OrderWindowModel();
    ui->tvOrderList->setModel(orderModel);
    for (int c = 0; c < ui->tvOrderList->horizontalHeader()->count(); ++c) {
        if (c == 0) {
            ui->tvOrderList->horizontalHeader()->resizeSection(c, 130);
            continue;
        }
        if (c == 1) {
            ui->tvOrderList->horizontalHeader()->resizeSection(c, 150);
            continue;
        }
        if (c == 2) {
            ui->tvOrderList->horizontalHeader()->resizeSection(c, 250);
            continue;
        }
        if (c == 3) {
            ui->tvOrderList->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        if (c == 4) {
            ui->tvOrderList->horizontalHeader()->resizeSection(c, 120);
            continue;
        }
        ui->tvOrderList->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void OrderWindow::initTableOrderDetail() {
    productModel = new ProductOrderModel();
    ui->tvOrderDetail->setModel(productModel);
    for (int c = 0; c < ui->tvOrderDetail->horizontalHeader()->count(); ++c) {
        if (c == 0) {
            ui->tvOrderDetail->horizontalHeader()->resizeSection(c, 350);
            continue;
        }
        if (c == 1) {
            ui->tvOrderDetail->horizontalHeader()->resizeSection(c, 50);
            continue;
        }
        if (c == 2) {
            ui->tvOrderDetail->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        if (c == 3) {
            ui->tvOrderDetail->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        if (c == 4) {
            ui->tvOrderDetail->horizontalHeader()->resizeSection(c, 180);
            continue;
        }
        ui->tvOrderDetail->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
}

void OrderWindow::initOrderWithFilter() {
    QDate date = QDate::currentDate();
    this->ui->dtOrderFilter->setDate(date);
    QString dateFmt = QString("%1-%2-%3").arg(date.year()).arg(date.month()).arg(date.day());
    getOrderWithFilter("", dateFmt);
}

void OrderWindow::getOrderWithFilter(const QString invoice, const QString date) {
    manager = new QNetworkAccessManager();
    QString url  = setting.getApi();
    request.setUrl(QUrl(QString("%1/api/orders/filters").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(setting.getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->post(request, parseToJSONPayload(invoice, date));
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QVector<Order> result;
            for (auto v : obj["data"].toArray()) {
                QJsonObject element = v.toObject();
                Order order = Order::fromJSON(element);
                result.append(order);
            }
            orderModel->setOrderData(result);
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout");
                return;
            }
            QString message = obj["message"].toString();
            showErrorDialog(message);
        }
    });
}

QByteArray OrderWindow::parseToJSONPayload(const QString invoice, const QString date) {
    QJsonObject payload;
    QJsonArray statuses;
    statuses.push_back(QJsonValue(orderStatus));
    payload.insert("statuses", statuses);
    payload.insert("invoice", invoice);
    payload.insert("start_date", date);
    payload.insert("end_date", date);

    QJsonDocument doc(payload);

    return doc.toJson();
}


void OrderWindow::on_btnSearch_clicked() {
    QString invoice = this->ui->txtInvoiceFilter->toPlainText();
    QDate date = this->ui->dtOrderFilter->date();
    QString dateFmt = date.toString("yyyy-MM-dd");
    getOrderWithFilter(invoice, dateFmt);
}

void OrderWindow::onSelectOrder(const Order &order) {
    QVector<Product> products;
    for (OrderDetail od : order.getOrderDetails()) {
        Product product = od.getProduct();
        products.append(product);
    }
    productModel->setProductData(products);
}

void OrderWindow::onChoosenOrder(const Order &order) {
    Order od = order;
    if (orderStatus == Status::Pending) {
        joinOrder(od);
    } else if (orderStatus == Status::Finish) {
        showReprintDialog(od);
    }
}

void OrderWindow::joinOrder(Order &order) {
    int ret = QMessageBox::warning(this, tr("Peringatan"),
                                   tr("Gabungkan dengan bill ini? Proses ini akan membatalkan transaksi sebelumnya."),
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
        cancelSelectedOrder(order);
}

void OrderWindow::showReprintDialog(Order &order) {
    int ret = QMessageBox::warning(this, tr("Peringatan"),
                                   QString("Print nota dengan invoice: %1").arg(order.getInvoice()),
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok) {
        reprintOrder(order);
    }
}

void OrderWindow::cancelSelectedOrder(Order &order) {
    manager = new QNetworkAccessManager();
    QString url  = setting.getApi();
    request.setUrl(QUrl(QString("%1/api/orders/status").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(setting.getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->put(request, order.parseToJSONOrder(Status::Canceled));
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            emit onChoosenPendingOrder(order);
            this->close();
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout");
                return;
            }
            QString message = obj["message"].toString();
            showErrorDialog(message);
        }
    });
}

void OrderWindow::showErrorDialog(const QString &message) {
    QMessageBox errorDialog;
    int ret = errorDialog.critical(this, "Error", message);
    this->setDisabled(false);
    if (ret == QMessageBox::Ok) {
        this->setDisabled(false);
    } else {
        this->setDisabled(false);
    }
}

void OrderWindow::reprintOrder(Order &order) {
    manager = new QNetworkAccessManager();
    QString url  = setting.getApi();
    request.setUrl(QUrl(QString("%1/api/profile/shop").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(setting.getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->get(request);
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QJsonObject shopProfileObj = obj["data"].toObject();
            ShopProfile shopProfile = ShopProfile::fromJSON(shopProfileObj);
            Order ord = order;
            Printer::printOrder(ord, shopProfile);
            this->close();
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout");
                return;
            }
            QString message = obj["message"].toString();
            showErrorDialog(message);
        }
    });
}

void OrderWindow::closeDialog() {
    emit onChoosenPendingOrder(Order());
    this->close();
}
