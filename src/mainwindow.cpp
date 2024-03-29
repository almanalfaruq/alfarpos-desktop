#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStyle>
#include <QDesktopWidget>
#include <QTime>
#include <QShortcut>
#include <QInputDialog>
#include <QMessageBox>
#include <QProgressDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    qRegisterMetaType<QVector<Product> >("QVector<Product>");
    qRegisterMetaType<Product>("Product");
    qRegisterMetaType<Order>("Order");
    qRegisterMetaType<Type>("Type");
    qRegisterMetaType<Money>("Money");

    ui->stackedWidget->setCurrentIndex(0);

    connect(ui->txtUsername, SIGNAL(onEscPressed()), this, SLOT(openDialogExit()), Qt::QueuedConnection);
    connect(ui->txtPassword, SIGNAL(onEscPressed()), this, SLOT(openDialogExit()), Qt::QueuedConnection);

    timer = new QTimer(this);


    // Will be used to center the window
//    this->setGeometry(
//        QStyle::alignedRect(
//            Qt::LeftToRight,
//            Qt::AlignCenter,
//            this->size(),
//            qApp->desktop()->availableGeometry()
//        )
//    );
}

MainWindow::~MainWindow() {
    Setting::getInstance().deleteAuthToken();
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *) {
    // Will be used to center the widget
//    QRect geom = ui->vLayout->geometry();
//    geom.setWidth(this->width()/2);
//    geom.setHeight(this->height()/2);
//    ui->vLayout->setGeometry(geom);
}

void MainWindow::on_btnLogin_clicked() {
    login();
}

void MainWindow::login() {
    QString username = ui->txtUsername->text();
    if (username == "") {
        showErrorDialog("Username tidak boleh kosong");
        return;
    }
    QString password = ui->txtPassword->text();
    if (password == "") {
        showErrorDialog("Password tidak boleh kosong");
        return;
    }
    QProgressDialog *progress = new QProgressDialog(this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setLabelText("Sedang masuk...");
    progress->setCancelButton(0);
    progress->setRange(0,0);
    progress->setMinimumDuration(0);
    progress->show();
    QJsonObject loginPayload;
    loginPayload.insert("username", username);
    loginPayload.insert("password", password);
    QJsonDocument loginDoc(loginPayload);
    manager = new QNetworkAccessManager();
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/users/login").arg(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->post(request, loginDoc.toJson());
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QJsonObject data = obj["data"].toObject();
            QString token = data["token"].toString();
            Setting::getInstance().setAuthToken(token);
            QJsonObject userObj = data["user"].toObject();
            user = User::fromJSON(userObj);
            ui->txtUsername->clear();
            ui->txtPassword->clear();
            ui->stackedWidget->setCurrentIndex(1);
            initCashier();
        } else {
            if (reply->error() == QNetworkReply::OperationCanceledError || reply->error() == QNetworkReply::TimeoutError) {
                showErrorDialog("Server timeout. Silakan coba lagi.");
            } else {
                QString message = obj["message"].toString();
                showErrorDialog(message);
            }
        }
        progress->cancel();
        reply->close();
    });
}

void MainWindow::initCashier() {
    // clock
    connect(timer, &QTimer::timeout, this, &MainWindow::startTime);
    timer->start(1000);

    // init table
    initTableView();

    QDate now = QDate::currentDate();
    QLocale indonesian(QLocale::Indonesian, QLocale::Indonesia);
    ui->lblTopDesc->setText(QString("%1 [%2]").arg(indonesian.toString(now, QLocale::LongFormat)).arg(user.getFullname()));

    // lcd total
    ui->lcdTotal->display(0);
}

void MainWindow::initTableView() {
    model = new ProductModel();
    connect(model, SIGNAL(editCompleted(bool)), this, SLOT(onEditProduct(bool)), Qt::QueuedConnection);
    connect(model, SIGNAL(productNotFound()), this, SLOT(openDialogNotFound()), Qt::QueuedConnection);
    connect(model, SIGNAL(timeoutError()), this, SLOT(openDialogTimeout()), Qt::QueuedConnection);
    connect(model, SIGNAL(otherError(QString)), this, SLOT(openDialogOtherError(QString)), Qt::QueuedConnection);
    connect(model, SIGNAL(openProductDialog(QVector<Product>, int, QString)), this, SLOT(openProductDialog(QVector<Product>, int, QString)), Qt::QueuedConnection);
    connect(ui->tblItems, SIGNAL(onDeletePressed(QModelIndex)), model, SLOT(onDeleteProduct(QModelIndex)), Qt::QueuedConnection);
    connect(ui->tblItems, SIGNAL(onPlusPressed()), this, SLOT(openDialogPayment()), Qt::QueuedConnection);
    connect(ui->tblItems, SIGNAL(onEscPressed()), this, SLOT(openDialogLogout()), Qt::QueuedConnection);
    ui->tblItems->setModel(model);

    EditorDelegate *delegate = new EditorDelegate();
    connect(delegate, SIGNAL(onCloseEditor()), ui->tblItems, SLOT(onCloseEditor()), Qt::QueuedConnection);
    connect(delegate, SIGNAL(onHomePressed()), this, SLOT(openDialogConfirmClearProduct()), Qt::QueuedConnection);
    connect(delegate, SIGNAL(onPlusPressed()), this, SLOT(openDialogPayment()), Qt::QueuedConnection);
    ui->tblItems->setItemDelegate(delegate);

    for (int c = 0; c < ui->tblItems->horizontalHeader()->count(); ++c) {
        if (c == 0) {
            ui->tblItems->horizontalHeader()->resizeSection(c, 120);
            continue;
        }
        if (c == 2) {
            ui->tblItems->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        if (c == 3) {
            ui->tblItems->horizontalHeader()->resizeSection(c, 50);
            continue;
        }
        if (c == 4) {
            ui->tblItems->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        if (c == 5) {
            ui->tblItems->horizontalHeader()->resizeSection(c, 150);
            continue;
        }
        ui->tblItems->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
    initShortcut();
    focusEditableCell();
}

void MainWindow::initShortcut() {
    // Shortcut to make a payment
    QShortcut *payShortcut = new QShortcut(QKeySequence(tr("PLUS")), ui->tblItems);
    connect(payShortcut, SIGNAL(activated()), this, SLOT(openDialogPayment()));

    // Shortcut to change sell price
    QShortcut *sellPriceShortcut = new QShortcut(QKeySequence(tr("F7")), ui->tblItems);
    connect(sellPriceShortcut, SIGNAL(activated()), this, SLOT(openDialogSellPrice()));

    // Shortcut to change qty
    QShortcut *qtyShortcut = new QShortcut(QKeySequence(tr("F8")), ui->tblItems);
    connect(qtyShortcut, SIGNAL(activated()), this, SLOT(openDialogQty()));

    // Shortcut to change clear/reset transaction
    QShortcut *clearShortcut = new QShortcut(QKeySequence(tr("HOME")), ui->tblItems);
    connect(clearShortcut, SIGNAL(activated()), this, SLOT(openDialogConfirmClearProduct()));

    // Shortcut to change hold order/transaction
    QShortcut *holdOrderShortcut = new QShortcut(QKeySequence(tr("F12")), ui->tblItems);
    connect(holdOrderShortcut, SIGNAL(activated()), this, SLOT(openDialogHoldOrder()));

    // Shortcut to reprint order
    QShortcut *reprintOrderShortcut = new QShortcut(QKeySequence(Qt::Key_PageUp), ui->tblItems);
    connect(reprintOrderShortcut, SIGNAL(activated()), this, SLOT(openDialogReprintBill()));

    // Shortcut to make a money in transaction
    QShortcut *moneyInShortcut = new QShortcut(QKeySequence(tr("CTRL+F7")), ui->tblItems);
    connect(moneyInShortcut, SIGNAL(activated()), this, SLOT(openDialogMoneyIn()));

    // Shortcut to make a money out transaction
    QShortcut *moneyOutShortcut = new QShortcut(QKeySequence(tr("CTRL+F8")), ui->tblItems);
    connect(moneyOutShortcut, SIGNAL(activated()), this, SLOT(openDialogMoneyOut()));

    // Shortcut to check for transaction that being hold
    QShortcut *joinBillShortcut = new QShortcut(QKeySequence(tr("CTRL+F10")), ui->tblItems);
    connect(joinBillShortcut, SIGNAL(activated()), this, SLOT(openDialogJoinBill()));

    // Shortcut to check for seeing the transaction
    QShortcut *openTransactionWindow = new QShortcut(QKeySequence(tr("CTRL+F11")), ui->tblItems);
    connect(openTransactionWindow, SIGNAL(activated()), this, SLOT(openTransactionWindow()));

    // Shortcut to check for open the drawer
    QShortcut *openDrawerShortcut = new QShortcut(QKeySequence(tr("=")), ui->tblItems);
    connect(openDrawerShortcut, SIGNAL(activated()), this, SLOT(openDrawer()));

    // Shortcut to open menu window
    QShortcut *menuShortcut = new QShortcut(QKeySequence(tr("F1")), ui->tblItems);
    connect(menuShortcut, SIGNAL(activated()), this, SLOT(openMenuWindow()));
}

void MainWindow::startTime() {
    QTime t = QTime::currentTime();
    QString text = t.toString("hh:mm:ss");
    ui->lblTime->setText(text);
}

void MainWindow::focusEditableCell() {
    int rows = ui->tblItems->model()->rowCount();
    QModelIndex index = ui->tblItems->model()->index(rows - 1, 1);
    ui->tblItems->setFocus();
    ui->tblItems->edit(index);
    ui->tblItems->selectRow(index.row());
    ui->tblItems->selectColumn(1);
}

void MainWindow::onEditProduct(const bool &isDelete) {
    ui->tblItems->scrollToBottom();
    focusEditableCell();
    QVector<Product> products = model->getProducts();
    bool ok;
    bool isProductCountSame = products.count() - 1 == productCount;
    if (!isProductCountSame) {
        productCount = products.count() - 1;
    }
    int lastRow = products.count() - 2;
    if (lastRow >= 0 && !isDelete && products[lastRow].getIsOpenPrice() && !isProductCountSame) {
        Product product = products[lastRow];
        int newSellPrice = QInputDialog::getInt(this, tr("Ubah Harga Jual"),
                                                tr("Harga Jual"), product.getSellPrice(), 0, 10000000, 1, &ok);

        if (ok && newSellPrice > 0) {
            emit model->changeSellPriceCompleted(lastRow, newSellPrice);
        } else {
            QModelIndex index = ui->tblItems->model()->index(lastRow, 1);
            emit model->onDeleteProduct(index);
            return;
        }
    }
    total = 0;
    for (Product p : products) {
        total += p.getTotal();
    }
    ui->lcdTotal->display(total);
}

void MainWindow::openMenuWindow() {
    menuWindow = new MenuWindow();
    connect(menuWindow, SIGNAL(onSelectMenu(int)), this, SLOT(onSelectMenuIndex(int)), Qt::QueuedConnection);
    connect(menuWindow, SIGNAL(onCloseEvent()), this, SLOT(onCloseDialog()), Qt::QueuedConnection);
    menuWindow->setWindowModality(Qt::ApplicationModal);
    menuWindow->setWindowTitle("Menu");
    menuWindow->show();
}

void MainWindow::onSelectMenuIndex(int index) {
    switch (index) {
    case 0:
        openDialogPayment();
        break;
    case 1:
        openDialogSellPrice();
        break;
    case 2:
        openDialogQty();
        break;
    case 3:
        openDialogConfirmClearProduct();
        break;
    case 4:
        openDialogHoldOrder();
        break;
    case 5:
        openDialogReprintBill();
        break;
    case 6:
        openDialogMoneyIn();
        break;
    case 7:
        openDialogMoneyOut();
        break;
    case 8:
        openDialogJoinBill();
        break;
    case 9:
        openTransactionWindow();
        break;
    case 10:
        openDrawer();
        break;
    }
}

void MainWindow::openDialogHoldOrder() {
    if (total == 0) {
        showErrorDialog("Tidak bisa menunda transaksi kosong");
        return;
    }
    QMessageBox holdOrderDialog;
    int ret = holdOrderDialog.question(this, "Konfirmasi", "Simpan order terlebih dahulu untuk transaksi ini?");
    if (ret == QMessageBox::Yes) {
        Order order = createOrder();
        QByteArray jsonData = order.parseToJSONOrder(order.getStatus());
        manager = new QNetworkAccessManager();
        QString url  = Setting::getInstance().getApi();
        request.setUrl(QUrl(QString("%1/api/orders").arg(url)));
        request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        request.setTransferTimeout(5000);
        manager->post(request, jsonData);
        QObject::connect(manager, &QNetworkAccessManager::finished,
        this, [=](QNetworkReply *reply) {
            manager->deleteLater();
            reply->deleteLater();
            QString answer = reply->readAll();
            QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
            QJsonObject obj = doc.object();
            int code = obj["code"].toInt();
            if (code == 200) {
                QString invoice = obj["data"].toObject()["invoice"].toString();
                emit model->clearModelPressed();
                showHoldInvoice(invoice);
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
}

void MainWindow::showHoldInvoice(const QString invoice) {
    QMessageBox okDialog;
    int ret = okDialog.information(this, "Informasi", QString("Order disimpan dengan nomor invoice: %1").arg(invoice));
    if (ret == QMessageBox::Ok) focusEditableCell();
}

void MainWindow::openDialogQty() {
    QModelIndex index = ui->tblItems->currentIndex();
    int row = index.row();
    if (row == ui->tblItems->model()->rowCount() - 1) {
        row -= 1;
    }
    int qty = model->index(row, 3).data().toInt();
    bool ok;
    int newQty = QInputDialog::getInt(this, tr("Ubah Qty"),
                                      tr("Qty"), qty, 0, 1000, 1, &ok);

    if (ok && newQty > 0)
        emit model->changeQtyCompleted(row, newQty);
}

void MainWindow::openDialogSellPrice() {
    QModelIndex index = ui->tblItems->currentIndex();
    int row = index.row();
    if (row == ui->tblItems->model()->rowCount() - 1) {
        row -= 1;
    }
    int sellPrice = model->index(row, 2).data().toInt();
    bool ok;
    int newSellPrice = QInputDialog::getInt(this, tr("Ubah Harga Jual"),
                                            tr("Harga Jual"), sellPrice, 0, 10000000, 1, &ok);

    if (ok && newSellPrice > 0)
        emit model->changeSellPriceCompleted(row, newSellPrice);
}

void MainWindow::openDialogNotFound() {
    QMessageBox errorDialog;
    int ret = errorDialog.critical(this, "Error", "Produk tidak ditemukan");
    this->setDisabled(false);
    if (ret == QMessageBox::Ok) {
        focusEditableCell();
        this->setDisabled(false);
    } else {
        this->setDisabled(false);
    }
}

void MainWindow::openDialogTimeout() {
    showErrorDialog("Server timeout. Silakan coba lagi.");
}

void MainWindow::openDialogOtherError(const QString &msg) {
    showErrorDialog(msg);
}

void MainWindow::openDialogConfirmClearProduct() {
    int ret = QMessageBox::warning(this, tr("Peringatan"),
                                   tr("Batalkan transaksi ini?"),
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
        emit model->clearModelPressed();
}

void MainWindow::openDialogJoinBill() {
    orderWindow = new OrderWindow();
    connect(orderWindow, SIGNAL(onChoosenPendingOrder(Order)), this, SLOT(onCloseJoinBill(Order)), Qt::QueuedConnection);
    connect(orderWindow, SIGNAL(onCloseEvent()), this, SLOT(onCloseDialog()), Qt::QueuedConnection);
    orderWindow->setWindowModality(Qt::ApplicationModal);
    orderWindow->setWindowTitle("Gabung Bill");
    orderWindow->show();
    this->setDisabled(true);
}

void MainWindow::openTransactionWindow() {
    transactionWindow = new TransactionWindow();
    connect(transactionWindow, SIGNAL(onCloseEvent()), this, SLOT(onCloseDialog()), Qt::QueuedConnection);

    transactionWindow->setWindowModality(Qt::ApplicationModal);
    transactionWindow->setWindowTitle("Informasi Uang Masuk/Keluar");
    transactionWindow->show();
    this->setDisabled(true);
}

void MainWindow::openDrawer() {
    Printer::openDrawer();
}

void MainWindow::openDialogReprintBill() {
    orderWindow = new OrderWindow();
    connect(orderWindow, SIGNAL(onCloseEvent()), this, SLOT(onCloseDialog()), Qt::QueuedConnection);
    orderWindow->setWindowModality(Qt::ApplicationModal);
    orderWindow->setOrderStatus(Status::Finish);
    orderWindow->setWindowTitle("Reprint Nota");
    orderWindow->show();
    this->setDisabled(true);
}

void MainWindow::openProductDialog(const QVector<Product> &products, const int &qty, const QString &query) {
    productDialog = new ProductDialog();
    connect(productDialog, SIGNAL(onSelectProduct(Product)), this, SLOT(onSelectProduct(Product)), Qt::QueuedConnection);
    connect(productDialog, SIGNAL(onCloseEvent()), this, SLOT(onCloseDialog()), Qt::QueuedConnection);

    productDialog->setProducts(products, qty, query);
    productDialog->setWindowModality(Qt::ApplicationModal);
    productDialog->show();
    this->setDisabled(true);
}

void MainWindow::openDialogMoneyIn() {
    openDialogMoneyTransaction(Type::In);
}

void MainWindow::openDialogMoneyOut() {
    openDialogMoneyTransaction(Type::Out);
}

void MainWindow::openDialogMoneyTransaction(const Type &type) {
    QString title = "Catat uang masuk";
    if (type == Type::Out) {
        title = "Catat uang keluar";
    }
    bool ok;
    QMap<QString, QString> map = InputMoneyTransaction::getMapString(this, title, &ok);
    if (ok) {
        Money money = Money(type, map["amount"].toDouble(), map["note"]);
        createMoneyTransaction(money);
    }
}

void MainWindow::createMoneyTransaction(Money &money) {
    QByteArray jsonData = money.parseToJSONPayload();
    manager = new QNetworkAccessManager();
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/money").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->post(request, jsonData);
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QJsonObject element = obj["data"].toObject();
            Money printedMoney = Money::fromJSON(element);
            printMoneyTransaction(printedMoney);
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

void MainWindow::printMoneyTransaction(Money &money) {
    manager = new QNetworkAccessManager();
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/profile/shop").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
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
            Money printedMoney = money;
            Printer::printMoneyTransaction(printedMoney, shopProfile);
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

void MainWindow::showErrorDialog(const QString &message) {
    QMessageBox errorDialog;
    int ret = errorDialog.critical(this, "Error", message);
    this->setDisabled(false);
    if (ret == QMessageBox::Ok) {
        if (ui->stackedWidget->currentIndex() == 1)
            focusEditableCell();
        this->setDisabled(false);
    } else {
        this->setDisabled(false);
    }
}

Order MainWindow::createOrder() {
    QVector<OrderDetail> orderDetails;
    QVector<Product> products = model->getProducts();
    for (auto p : products.mid(0, products.count() - 1)) {
        orderDetails.append(OrderDetail(p, p.getQty(), p.getTotal(), p.isUsingSpecialPrice()));
    }

    // Set paymentID to 1
    return Order(1, 1, Status::Pending, total, 0, 0, 1, orderDetails);
}

void MainWindow::onSelectProduct(const Product &product) {
    this->setDisabled(false);
    emit model->onSelectProduct(product);
}

void MainWindow::openDialogPayment() {
    if (total == 0) return;

    paymentDialog = new PaymentDialog();
    connect(paymentDialog, SIGNAL(canceled()), this, SLOT(onCancelPayment()), Qt::QueuedConnection);
    connect(paymentDialog, SIGNAL(paymentSuccess()), this, SLOT(onSuccessPayment()), Qt::QueuedConnection);
    connect(paymentDialog, SIGNAL(passTotalChange(int)), this, SLOT(onPassTotalChange(int)), Qt::QueuedConnection);
    connect(paymentDialog, SIGNAL(closeDialog()), this, SLOT(onCloseDialog()), Qt::QueuedConnection);

    QVector<Product> products = model->getProducts();
    paymentDialog->setProducts(products.mid(0, products.count() - 1));
    paymentDialog->setWindowModality(Qt::ApplicationModal);
    paymentDialog->show();
    this->setDisabled(true);
}

void MainWindow::openDialogLogout() {
    QString message = "Anda yakin akan keluar?";
    if (total != 0) {
        message = QString("Transaksi belum selesai. %1").arg(message);
    }
    int ret = QMessageBox::warning(this, tr("Peringatan"), message,
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok) {
        Setting::getInstance().deleteAuthToken();
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::onCancelPayment() {
    this->setDisabled(false);
    focusEditableCell();
}

void MainWindow::onSuccessPayment() {
    this->setDisabled(false);
    emit model->clearModelPressed();
    this->ui->lblTotal->setText("TOTAL >>>");
    this->ui->lcdTotal->display(0);
    focusEditableCell();
}

void MainWindow::onPassTotalChange(const int &totalChange) {
    this->ui->lblTotal->setText("KEMBALI >>>");
    this->ui->lcdTotal->display(totalChange);
}

void MainWindow::onCloseJoinBill(const Order &order) {
    if (order.getId() > 1) {
        pendingOrder = order;
        QVector<Product> products;
        for (OrderDetail od : pendingOrder.getOrderDetails()) {
            Product product = od.getProduct();
            products.append(product);
        }
        model->setProductData(products);
    }
    this->setDisabled(false);
    focusEditableCell();
}

void MainWindow::onCloseDialog() {
    this->setDisabled(false);
    if (ui->tblItems->model()->rowCount() < 1) return;
    focusEditableCell();
}

void MainWindow::on_txtPassword_returnPressed() {
    login();
}

void MainWindow::on_txtUsername_returnPressed() {
    login();
}

void MainWindow::on_btnLogin_pressed() {
    login();
}

void MainWindow::on_btnExit_pressed() {
    openDialogExit();
}

void MainWindow::openDialogExit() {
    int ret = QMessageBox::warning(this, tr("Peringatan"), "Anda yakin akan keluar aplikasi?",
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok)
        QApplication::quit();
}
