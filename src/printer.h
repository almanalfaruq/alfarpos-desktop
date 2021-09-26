#ifndef PRINTER_H
#define PRINTER_H

#include <QPrinter>
#include <QTextDocument>
#include "order.h"
#include "setting.h"
#include <QSerialPort>
#include "windows.h"

class Printer {
public:
    Printer();
    static void printOrder(Order &);
    static void openDrawer();
private:
    static QMap<QString, QString> parseOrderToMap(Order &);
    static void open();
    static QString getPrinterName();
};

#endif // PRINTER_H
