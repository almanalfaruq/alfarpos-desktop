#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QKeyEvent>
#include <QPrinter>
#include <QPainter>
#include <QTextDocument>
#include <QFont>
#include "setting.h"
#include "productmodel.h"
#include "product_dialog.h"
#include "editordelegate.h"
#include "paymentdialog.h"
#include "orderwindow.h"
#include "money.h"
#include "inputmoneytransaction.h"
#include "menuwindow.h"
#include "printer.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    ProductModel *model;
    QTimer *timer;
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    ProductDialog *productDialog;
    PaymentDialog *paymentDialog;
    OrderWindow *orderWindow;
    MenuWindow *menuWindow;
    Setting setting;
    QString productListText;
    Order pendingOrder;
    User user;

    int total = 0, productCount = 0;

    void initCashier();
    void initShortcut();
    void initTableView();
    void startTime();
    void focusEditableCell();
    void showHoldInvoice(const QString invoice);
    Order createOrder();

    void openDialogMoneyTransaction(const Type &type);
    void createMoneyTransaction(Money &money);

    void showErrorDialog(const QString &);
    void login();
protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void onEditProduct(const bool &isDelete);

    void openMenuWindow();
    void openDialogHoldOrder();
    void openDialogQty();
    void openDialogSellPrice();
    void openDialogNotFound();
    void openDialogTimeout();
    void openDialogOtherError(const QString &);
    void openDialogConfirmClearProduct();
    void openDialogJoinBill();
    void openDrawer();
    void openProductDialog(const QVector<Product> &);
    void openDialogMoneyIn();
    void openDialogMoneyOut();
    void openDialogPayment();
    void openDialogLogout();
    void openDialogExit();

    void onSelectProduct(const Product &);
    void onCancelPayment();
    void onSuccessPayment();
    void onCloseJoinBill(const Order &);
    void on_btnLogin_clicked();

    void onSelectMenuIndex(int index);
    void onCloseDialog();

    void openDialogReprintBill();
    void on_txtPassword_returnPressed();

    void on_txtUsername_returnPressed();

    void on_btnLogin_pressed();

    void on_btnExit_pressed();

signals:
    void changeQty(qint64 qty);
};
#endif // MAINWINDOW_H
