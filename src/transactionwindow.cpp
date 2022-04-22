#include "transactionwindow.h"
#include "ui_transactionwindow.h"

TransactionWindow::TransactionWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransactionWindow) {
    ui->setupUi(this);
    connect(ui->tvTransaction, SIGNAL(onEnterPressed(QModelIndex)), this, SLOT(onEnterPressed(QModelIndex)), Qt::QueuedConnection);
    connect(ui->tvTransaction, SIGNAL(onEscPressed()), this, SLOT(onEscPressed()), Qt::QueuedConnection);
    QShortcut *closeDialog = new QShortcut(QKeySequence(tr("ESC")), ui->tvTransaction);
    connect(closeDialog, SIGNAL(activated()), this, SLOT(onEscPressed()));

    model = new TransactionTableModel();
    ui->tvTransaction->setModel(model);
    for (int c = 0; c < ui->tvTransaction->horizontalHeader()->count(); ++c) {
        if (c == 0) {
            ui->tvTransaction->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        if (c == 1) {
            ui->tvTransaction->horizontalHeader()->resizeSection(c, 120);
            continue;
        }
        if (c == 3) {
            ui->tvTransaction->horizontalHeader()->resizeSection(c, 130);
            continue;
        }
        ui->tvTransaction->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }

    initTransactionWithFilter();
}

TransactionWindow::~TransactionWindow() {
    delete ui;
}

void TransactionWindow::setTransactionData(const QVector<Money> &monies) {
    model->setTransactionData(monies);
}

void TransactionWindow::onEnterPressed(const QModelIndex &index) {
    Money m = model->getData(index.row());
    int ret = QMessageBox::warning(this, tr("Peringatan"),
                                   tr("Cetak ulang transaksi ini?"),
                                   QMessageBox::Ok | QMessageBox::Cancel,
                                   QMessageBox::Ok);
    if (ret == QMessageBox::Ok) reprintTransaction(m);

    emit onSelectTransactionData(m);
}

void TransactionWindow::onEscPressed() {
    this->close();
}

void TransactionWindow::closeEvent(QCloseEvent *) {
    emit onCloseEvent();
}

void TransactionWindow::initTransactionWithFilter() {
    QDate date = QDate::currentDate();
    ui->dtFilter->setDate(date);
    QString dateFmt = QString("%1-%2-%3").arg(date.year()).arg(date.month()).arg(date.day());
    getTransactionWithFilter(dateFmt);
}

void TransactionWindow::getTransactionWithFilter(QString date) {
    manager = new QNetworkAccessManager();
    QString url  = Setting::getInstance().getApi();
    request.setUrl(QUrl(QString("%1/api/money/filters").arg(url)));
    request.setRawHeader("Authorization", QString("Bearer %1").arg(Setting::getInstance().getAuthToken()).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(5000);
    manager->post(request, parseToJSONPayload(date));
    QObject::connect(manager, &QNetworkAccessManager::finished,
    this, [=](QNetworkReply *reply) {
        manager->deleteLater();
        reply->deleteLater();
        QString answer = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(answer.toUtf8());
        QJsonObject obj = doc.object();
        int code = obj["code"].toInt();
        if (code == 200) {
            QVector<Money> result;
            for (auto v : obj["data"].toArray()) {
                QJsonObject element = v.toObject();
                Money money = Money::fromJSON(element);
                result.append(money);
            }
            model->setTransactionData(result);
            focusLastRow();
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

QByteArray TransactionWindow::parseToJSONPayload(const QString date) {
    QJsonObject payload;
    QJsonArray types;
//    QDate endDate = QDate::fromString(date, "yyyy-MM-dd").addDays(1);
    types.push_back(QJsonValue(-1));
    types.push_back(QJsonValue(1));
    payload.insert("types", types);
    payload.insert("start_date", date);
//    payload.insert("end_date", endDate.toString("yyyy-MM-dd"));
    payload.insert("end_date", date);
//    payload.insert("sort", "desc");

    QJsonDocument doc(payload);

    return doc.toJson();
}

void TransactionWindow::reprintTransaction(Money &money) {
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
            Money mn = money;
            Printer::printMoneyTransaction(mn, shopProfile);
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

void TransactionWindow::on_btnPrint_clicked() {
    QModelIndex index = ui->tvTransaction->currentIndex();
    onEnterPressed(index);
}

void TransactionWindow::on_btnCancel_clicked() {
    this->close();
}

void TransactionWindow::showErrorDialog(const QString &message) {
    QMessageBox errorDialog;
    int ret = errorDialog.critical(this, "Error", message);
    this->setDisabled(false);
    if (ret == QMessageBox::Ok) {
        this->setDisabled(false);
    } else {
        this->setDisabled(false);
    }
}

void TransactionWindow::focusLastRow() {
    QModelIndex index = ui->tvTransaction->model()->index(ui->tvTransaction->model()->rowCount() - 1, 1);
    ui->tvTransaction->setFocus();
    ui->tvTransaction->selectRow(index.row());
    ui->tvTransaction->scrollToBottom();
}

void TransactionWindow::on_btnSearch_clicked() {
    QDate date = this->ui->dtFilter->date();
    QString dateFmt = QString("%1-%2-%3").arg(date.year()).arg(date.month()).arg(date.day());
    getTransactionWithFilter(dateFmt);
}
