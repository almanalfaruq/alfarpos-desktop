#include "orderwindow.h"
#include "ui_orderwindow.h"

OrderWindow::OrderWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OrderWindow) {
    ui->setupUi(this);
    ui->pbWait->setVisible(false);
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

    connect(ui->tvOrderList, SIGNAL(onSelectionChange(QModelIndex)), orderModel, SLOT(onSelectionChange(QModelIndex)), Qt::QueuedConnection);
    connect(ui->tvOrderList, SIGNAL(onEnterPressed(QModelIndex)), orderModel, SLOT(onEnterPressed(QModelIndex)), Qt::QueuedConnection);
    connect(orderModel, SIGNAL(onSelectOrder(Order)), this, SLOT(onSelectOrder(Order)), Qt::QueuedConnection);
    connect(orderModel, SIGNAL(onChoosenOrder(Order)), this, SLOT(onChoosenOrder(Order)), Qt::QueuedConnection);
    connect(orderModel, SIGNAL(onFetchMore()), this, SLOT(onFetchMore()), Qt::QueuedConnection);
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
    dateFmt = QString("%1-%2-%3").arg(date.year()).arg(date.month()).arg(date.day());
    getOrderWithFilter();
}

void OrderWindow::getOrderWithFilter() {
    page = 1;
    manager = new QNetworkAccessManager();
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/orders/filters").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->post(request, parseToJSONPayload());
    ui->pbWait->setVisible(true);
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QVector<Order> result;
            QJsonObject data = obj["data"].toObject();
            for (auto v : data["orders"].toArray()) {
                QJsonObject element = v.toObject();
                Order order = Order::fromJSON(element);
                result.append(order);
            }
            orderModel->setHasNext(data["has_next"].toBool());
            orderModel->setOrderData(result);
            page++;
            focusFirstRow();
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout. Silakan coba lagi.");
            } else {
                QString message = obj["message"].toString();
                showErrorDialog(message);
            }
        }
        ui->pbWait->setVisible(false);
        reply->close();
    });
}

QByteArray OrderWindow::parseToJSONPayload() {
    QJsonObject payload;
    QJsonArray statuses;
    statuses.push_back(QJsonValue(orderStatus));
    payload.insert("statuses", statuses);
    payload.insert("invoice", invoice);
    payload.insert("start_date", dateFmt);
    payload.insert("end_date", dateFmt);
    payload.insert("page", page);
    payload.insert("limit", 10);
    payload.insert("sort", "desc");

    QJsonDocument doc(payload);

    return doc.toJson();
}

void OrderWindow::focusFirstRow() {
    QModelIndex index = ui->tvOrderList->model()->index(0, 1);
    ui->tvOrderList->setFocus();
    ui->tvOrderList->selectRow(index.row());
    ui->tvOrderList->scrollToTop();
}

void OrderWindow::on_btnSearch_clicked() {
    invoice = this->ui->txtInvoiceFilter->toPlainText();
    QDate date = this->ui->dtOrderFilter->date();
    dateFmt = date.toString("yyyy-MM-dd");
    getOrderWithFilter();
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
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/orders/status").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->put(request, order.parseToJSONOrder(Status::Canceled));
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            emit onChoosenPendingOrder(order);
            this->close();
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout. Silakan coba lagi.");
            } else {
                QString message = obj["message"].toString();
                showErrorDialog(message);
            }
        }
        reply->close();
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
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/profile/shop").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    manager->get(request);
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
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
                showErrorDialog("Server timeout. Silakan coba lagi.");
            } else {
                QString message = obj["message"].toString();
                showErrorDialog(message);
            }
        }
        reply->close();
    });
}

void OrderWindow::onFetchMore() {
    manager = new QNetworkAccessManager();
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/orders/filters").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->post(request, parseToJSONPayload());
    ui->tvOrderList->setDisabled(true);
    ui->pbWait->setVisible(true);
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QVector<Order> result;
            QJsonObject data = obj["data"].toObject();
            for (auto v : data["orders"].toArray()) {
                QJsonObject element = v.toObject();
                Order order = Order::fromJSON(element);
                result.append(order);
            }
            orderModel->setHasNext(data["has_next"].toBool());
            orderModel->addOrderData(result);
            page++;
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout. Silakan coba lagi.");
            } else {
                QString message = obj["message"].toString();
                showErrorDialog(message);
            }
        }
        ui->tvOrderList->setDisabled(false);
        ui->tvOrderList->setFocus();
        ui->pbWait->setVisible(false);
        reply->close();
    });
}

void OrderWindow::closeDialog() {
    emit onChoosenPendingOrder(Order());
    this->close();
}
