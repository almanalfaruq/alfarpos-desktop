#ifndef PRINTER_H
#define PRINTER_H

#include <QPrinter>
#include <QTextDocument>
#include "order.h"
#include "setting.h"
#include <QSerialPort>
#include "windows.h"
#include "shopprofile.h"
#include "money.h"

class Printer {
public:
    Printer();
    static void printOrder(Order &, ShopProfile &);
    static void printMoneyTransaction(Money &, ShopProfile &);
    static void openDrawer();
private:
    static void setupPrinter(QPrinter *printer, QString docName);
    static QTextDocument getDocumentSetting(QPrinter &);
    static QMap<QString, QString> parseOrderToMap(Order &);
    static void open();
    static QString getPrinterName();
};

#endif // PRINTER_H
