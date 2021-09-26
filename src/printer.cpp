#include "printer.h"

#include <QMap>
#include <tchar.h>
#include <QPainter>
#include <QPrinterInfo>
#include <QTextDocumentWriter>

#pragma comment(lib, "winspool.lib")

Printer::Printer() {
}

void Printer::printOrder(Order &order) {
    QMap<QString, QString> orderMap = parseOrderToMap(order);
    QPrinter printer(QPrinter::HighResolution);
    printer.setPrinterName(getPrinterName());
    printer.setPageSizeMM(QSizeF(70, 279));
    printer.setResolution(160);
    printer.setDocName("Invoice");
    if (!printer.isValid()) {
        qDebug() << __FILE__ << __LINE__ << QString("Printer %1 isn't valid").arg(printer.printerName());
        return;
    }
    QTextDocument *doc = new QTextDocument();
    // personal reason
    // the EPSON TM U220 will print slightly bold even when using light font
    // but for the other printer, in this case IWARE IW-200U, should use bold style
    if (printer.printerName().toLower().contains("epson")) {
        doc->setDefaultStyleSheet("*{font-family:'Victor Mono Light';font-size:18px;width:100%;}tr{width:100%;}td{padding:3px0px;width:100%;}.qty{width:40%;text-align:right;}.price{width:30%;text-align:right;}.subtotal{width:35%;text-align:right;}");
    } else {
        doc->setDefaultStyleSheet("*{font-family:'Victor Mono';font-weight:bold;font-size:18px;width:100%;}tr{width:100%;}td{padding:3px0px;width:100%;}.qty{width:40%;text-align:right;}.price{width:30%;text-align:right;}.subtotal{width:35%;text-align:right;}");
    }
    doc->setHtml("<p align='center' width='100%' style='margin:2px 0px;'>TOKO ALFAR</p>"
                 "<p align='center' width='100%' style='margin:2px 0px;'>DEPAN PASAR KETAON, BANYUDONO, BOYOLALI</p>"
                 "<p align='center' width='100%' style='margin:2px 0px;'>TELP (0276) 3283720</p>"
                 "<table width='100%' style='border-collapse:collapse;margin-top:25px;'>"
                 "<tr>"
                 + QString("<td colspan='3' style='border-top: 1px double black'>Ksr: %1</td>").arg(orderMap["name"]) +
                 "</tr>"
                 "<tr>"
                 + QString("<td colspan='3'>Tgl, Jam: %1</td>").arg(orderMap["date_time"]) +
                 "</tr>"
                 "<tr>"
                 + QString("<td colspan='3' style='border-bottom: 1px solid black'>Invoice: %1</td>").arg(orderMap["invoice"]) +
                 "</tr>"
                 + orderMap["product_list"] +
                 "<tr>"
                 "<td width='30%'></td>"
                 "<td width='35%' align='right' style='border-top:1px dashed black;border-bottom:1px dashed black;'>Subtotal :</td>"
                 + QString("<td width='35%' align='right' style='border-top:1px dashed black;border-bottom:1px dashed black;'>%1</td>").arg(orderMap["total_formatted"]) +
                 "</tr>"
                 "<tr>"
                 + QString("<td width='30%' align='right'>%1 Item</td>").arg(orderMap["total_item"]) +
                 "<td width='35%' align='right'>Dibayar :</td>"
                 + QString("<td width='35%' align='right'>%1</td>").arg(orderMap["amount_paid"]) +
                 "</tr>"
                 "<tr>"
                 + QString("<td width='30%' align='right'>%1 Pcs</td>").arg(orderMap["total_pcs"]) +
                 "<td width='35%' align='right'>Kembali :</td>"
                 + QString("<td width='35%' align='right'>%1</td>").arg(orderMap["total_change"]) +
                 "</tr>"
                 "<tr>"
                 "<td colspan='3' align='center'></td>"
                 "</tr>"
                 "<tr>"
                 "<td colspan='3' align='center'>TERIMA KASIH ATAS KUNJUNGAN ANDA</td>"
                 "</tr>"
                 "<tr>"
                 "<td colspan='3' align='center'>BARANG YANG SUDAH DIBELI TIDAK DAPAT DIKEMBALIKAN</td>"
                 "</tr>"
                 "</table>");
    doc->setPageSize(printer.pageRect().size());
    doc->print(&printer);
    doc->deleteLater();
}

QMap<QString, QString> Printer::parseOrderToMap(Order &order) {
    QMap<QString, QString> map;
    qint64 totalItem = 0, totalPcs = 0;
    QString productListText;
    for (OrderDetail od : order.getOrderDetails()) {
        Product product = od.getProduct();
        totalItem++;
        totalPcs += od.getQty();
        productListText += QString(
                               "<tr>"
                               "<td colspan='3'>%1</td>"
                               "</tr>"
                               "<tr>"
                               "<td width='30%' align='right'>%2 x </td>"
                               "<td width='35%' align='right'>%3 = </td>"
                               "<td width='35%' align='right'>%4</td>"
                               "</tr>"
                           ).
                           arg(product.getName()).
                           arg(od.getQty()).
                           arg(product.getSellPriceFormatted()).
                           arg(od.getSubTotalFormatted());
    }
    QLocale indonesian(QLocale::Indonesian, QLocale::Indonesia);
    map["total_formatted"] = indonesian.toString(order.getTotal());
    map["product_list"] = productListText;
    map["amount_paid"] = indonesian.toString(order.getAmountPaid());
    map["total_change"] = indonesian.toString(order.getTotalChange());
    map["total_item"] = QString("%1").arg(totalItem);
    map["total_pcs"] = QString("%1").arg(totalPcs);
    map["invoice"] = order.getInvoice();
    map["date_time"] = order.getDateTime().toString(QString("dd-MM-yyyy hh:mm:ss"));
    map["name"] = order.getUser().getFullname();
    return map;
}

QString Printer::getPrinterName() {
    QPrinterInfo info;
    Setting setting = Setting();
    if (setting.getPrinter() != "") return setting.getPrinter();
    return info.defaultPrinterName();
}

void Printer::openDrawer() {
    Setting setting = Setting();
    // We cannot send command to USB00X using QSerialPort,
    // hence we're using the winspool (win32api) lib
    if (setting.getPrinterPort().contains("USB")) {
        open();
        return;
    }
    QSerialPort serialPort;
    serialPort.setPortName(setting.getPrinterPort());
    serialPort.setBaudRate(QSerialPort::Baud9600);
    serialPort.setParity(QSerialPort::NoParity);
    serialPort.setStopBits(QSerialPort::OneStop);
    serialPort.setFlowControl(QSerialPort::NoFlowControl);
    if (serialPort.open(QIODevice::WriteOnly)) {
        qDebug() << __FILE__ << __LINE__ << QString("Success connecting to port %1").arg(setting.getPrinterPort());
        QByteArray data;
        data.push_back((unsigned char) 0x1B);
        data.push_back((unsigned char) 0x70);
        data.push_back((char) 0x00);
        data.push_back((unsigned char) 0x19);
        data.push_back((unsigned char) 0xFA);
        const qint64 bytesWritten = serialPort.write(data);

        if (bytesWritten == -1) {
            qDebug() << __FILE__ << __LINE__ << QObject::tr("Failed to write the data to port %1, error: %2")
                     .arg(setting.getPrinterPort()).arg(serialPort.errorString())
                     << "\n";
        } else if (bytesWritten != data.size()) {
            qDebug() << __FILE__ << __LINE__ << QObject::tr("Failed to write all the data to port %1, error: %2")
                     .arg(setting.getPrinterPort()).arg(serialPort.errorString())
                     << "\n";
        } else if (!serialPort.waitForBytesWritten(5000)) {
            qDebug() << __FILE__ << __LINE__ << QObject::tr("Operation timed out or an error "
                     "occurred for port %1, error: %2")
                     .arg(setting.getPrinterPort()).arg(serialPort.errorString())
                     << "\n";
        }
        return;
    }

    qDebug() << __FILE__ << __LINE__ << QString("Cannot connect to port %1").arg(setting.getPrinterPort());
}

void Printer::open() {
    qDebug() << __FILE__ << __LINE__ << "Opening drawer on printer";
    QString printer_name = getPrinterName();

    QByteArray data;
    data.push_back((unsigned char) 0x1B);
    data.push_back((unsigned char) 0x70);
    data.push_back((char) 0x00);
    data.push_back((unsigned char) 0x19);
    data.push_back((unsigned char) 0xFA);

    HANDLE p_hPrinter;
    DOC_INFO_1 DocInfo;
    DWORD   dwJob = 0L;
    DWORD   dwBytesWritten = 0L;
    BOOL    bStatus = FALSE;

    //code to convert QString to wchar_t
    wchar_t szPrinterName[255];
    int length = printer_name.toWCharArray(szPrinterName);
    szPrinterName[length]=0;

    if (OpenPrinter(szPrinterName,&p_hPrinter,NULL)) {
        qDebug() << "Printer opening success " << QString::fromWCharArray(szPrinterName);
        DocInfo.pDocName = (LPTSTR)_T("Loyalty Receipt");
        DocInfo.pOutputFile = NULL;
        DocInfo.pDatatype = (LPTSTR)_T("RAW");
        dwJob = StartDocPrinter( p_hPrinter, 1, (LPBYTE)&DocInfo );
        if (dwJob > 0) {
            qDebug() << "Job is set.";
            bStatus = StartPagePrinter(p_hPrinter);
            if (bStatus) {
                qDebug() << "Writing text to printer" << data ;
                bStatus = WritePrinter(p_hPrinter, data.data(), data.length(),&dwBytesWritten);
                if(bStatus > 0) {
                    qDebug() << "printer write success" << bStatus;
                }
                EndPagePrinter(p_hPrinter);
            } else {
                qDebug() << "could not start printer";
            }
            EndDocPrinter(p_hPrinter);
            qDebug() << "closing doc";
        } else {
            qDebug() << "Couldn't create job";
        }
        ClosePrinter(p_hPrinter);
        qDebug() << "closing printer";
    } else {
        qDebug() << "Printer opening Failed";
    }
}
