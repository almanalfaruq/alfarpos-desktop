#include "paymentdialog.h"
#include "ui_paymentdialog.h"

#include <QMenu>

PaymentDialog::PaymentDialog(QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::PaymentDialog) {
    ui->setupUi(this);
    this->setCurrentIndex(0);

    setting = Setting();

    connect(ui->txtAmount, SIGNAL(valueChanged(int)), this, SLOT(checkAmountPaid(int)));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(setChangeTotal(int)));
    ui->txtAmount->setFocus();
    ui->txtAmount->installEventFilter(this);
    getPaymentType();
}

PaymentDialog::~PaymentDialog() {
    delete ui;
}

void PaymentDialog::setProducts(const QVector<Product> &products) {
    this->products = products;
    total = 0;
    for (auto p : products) {
        total += p.getTotal();
    }
    ui->txtAmount->setValue(total);
    ui->txtAmount->selectAll();
}

void PaymentDialog::getPaymentType() {
    QString url  = setting.getApi();
    manager = new QNetworkAccessManager();
    request.setUrl(QUrl(QString("%1/api/payments").arg(url)));
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
            for (auto v : obj["data"].toArray()) {
                QJsonObject element = v.toObject();
                qint64 id = element["id"].toInt();
                QString name = element["name"].toString();
                ui->cbPaymentType->addItem(name, QVariant(id));
            }
            ui->cbPaymentType->setCurrentIndex(0);
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

void PaymentDialog::showErrorDialog(const QString &message) {
    QMessageBox errorDialog;
    int ret = errorDialog.critical(this, "Error", message);
    this->setDisabled(false);
    if (ret == QMessageBox::Ok) {
        this->setDisabled(false);
    } else {
        this->setDisabled(false);
    }
}

void PaymentDialog::checkAmountPaid(int value) {
    amount = value;
    isPaymentValid = false;
    if (amount < total) {
        ui->lblErrorState->setText("Uang yang diterima belum cukup");
    } else {
        ui->lblErrorState->setText("");
        isPaymentValid = true;
    }
}

void PaymentDialog::triggerMenuAction(QAction *action) {
    QString actionText = action->text();
    if (actionText == "") {
        return;
    }
    if (actionText == ActionPrint) {
        shouldPrint = true;
    }
    order = createOrder();
    QByteArray jsonData = order.parseToJSONOrder(order.getStatus());
    sendOrderToServer(jsonData);
}

void PaymentDialog::setChangeTotal(int index) {
    if (index != 1) return;
    QLocale indonesian(QLocale::Indonesian, QLocale::Indonesia);
    ui->lblChange->setText(QString("Kembali\nRp %1").arg(indonesian.toString(totalChange)));
    ui->btnClose->setAutoDefault(true);
    QTimer::singleShot(1, ui->btnClose, SLOT(setFocus()));
}

void PaymentDialog::processOrder() {
    if (!isPaymentValid) return;
    QMenu menu(this);
    menu.addAction(ActionNotPrint);
    menu.addAction(ActionPrint);
    connect(&menu, SIGNAL(triggered(QAction*)), this, SLOT(triggerMenuAction(QAction*)));
    menu.exec(ui->lblChange->mapToGlobal(QPoint(150,0)));
}

bool PaymentDialog::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->txtAmount) {
        if (event->type() == QKeyEvent::KeyPress) {
            QKeyEvent * ke = static_cast<QKeyEvent*>(event);
            if (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter) {
                processOrder();
                return false;
            } else if (ke->key() == Qt::Key_Escape) {
                cancelPayment();
                return true;
            }
        }
        return false;
    }

    return QStackedWidget::eventFilter(watched, event);
}

void PaymentDialog::closeEvent(QCloseEvent *) {
    emit closeDialog();
}

void PaymentDialog::cancelPayment() {
    this->close();
    emit canceled();
}

Order PaymentDialog::createOrder() {
    int paymentID = ui->cbPaymentType->itemData(ui->cbPaymentType->currentIndex()).toInt();
    QVector<OrderDetail> orderDetails;
    for (auto p : products) {
        orderDetails.append(OrderDetail(p, p.getQty(), p.getTotal(), p.isUsingSpecialPrice()));
    }

    totalChange = amount - total;
    return Order(1, 1, Status::Finish, total, amount, totalChange, paymentID, orderDetails);
}

void PaymentDialog::sendOrderToServer(const QByteArray jsonData) {
    manager = new QNetworkAccessManager();
    QString url  = setting.getApi();
    request.setUrl(QUrl(QString("%1/api/orders").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(setting.getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->post(request, jsonData);
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QJsonObject orderObj = obj["data"].toObject();
            order = Order::fromJSON(orderObj);
            openDialogChange(order);
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

void PaymentDialog::openDialogChange(Order &order) {
    if (shouldPrint) {
        Printer::printOrder(order);
    }
    Printer::openDrawer();
    this->setCurrentIndex(1);
}

void PaymentDialog::closePayment() {
    this->close();
    emit paymentSuccess();
}

void PaymentDialog::on_btnOK_clicked() {
    processOrder();
}

void PaymentDialog::on_btnCancel_clicked() {
    cancelPayment();
}

void PaymentDialog::on_btnClose_pressed() {
    closePayment();
}