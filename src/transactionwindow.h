#ifndef TRANSACTIONWINDOW_H
#define TRANSACTIONWINDOW_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>
#include <QShortcut>
#include "transactiontablemodel.h"
#include "setting.h"
#include "printer.h"

namespace Ui {
class TransactionWindow;
}

class TransactionWindow : public QWidget {
    Q_OBJECT

public:
    explicit TransactionWindow(QWidget *parent = nullptr);
    ~TransactionWindow();

    void setTransactionData(const QVector<Money> &);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::TransactionWindow *ui;
    TransactionTableModel *model;
    QNetworkAccessManager *manager;
    QNetworkRequest request;

    void initTransactionWithFilter();
    void getTransactionWithFilter(QString date);
    QByteArray parseToJSONPayload(const QString date);
    void reprintTransaction(Money &);
    void showErrorDialog(const QString &message);
    void focusLastRow();

private slots:
    void onEnterPressed(const QModelIndex &);
    void onEscPressed();

    void on_btnPrint_clicked();

    void on_btnCancel_clicked();

    void on_btnSearch_clicked();

signals:
    void onSelectTransactionData(const Money &);
    void onCloseEvent();
};

#endif // TRANSACTIONWINDOW_H
