#include "mainwindow.h"

#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include "setting.h"

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg) {
    QString txt;
    QDateTime now = QDateTime::currentDateTime();
    switch (type) {
    case QtInfoMsg:
        txt = QString("Info: %1").arg(msg);
        break;
    case QtDebugMsg:
        if (!Setting::getInstance().getDebug()) return;
        txt = QString("%1 - Debug: %2").arg(now.toString(Qt::DateFormat::ISODateWithMs), msg);
        break;
    case QtWarningMsg:
        txt = QString("%1 - Warning: %2").arg(now.toString(Qt::DateFormat::ISODateWithMs), msg);
        break;
    case QtCriticalMsg:
        txt = QString("%1 - Critical: %2").arg(now.toString(Qt::DateFormat::ISODateWithMs), msg);
        break;
    case QtFatalMsg:
        txt = QString("%1 - Fatal: %2").arg(now.toString(Qt::DateFormat::ISODateWithMs), msg);
        break;
    }
    QDate dateNow = QDate::currentDate();
    QDir dir("./logs");
    if (!dir.exists())
        dir.mkpath(".");
    QFile outFile(QString("./logs/log-%1.txt").arg(dateNow.toString(Qt::DateFormat::ISODate)));
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << "\n";
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qInstallMessageHandler(myMessageHandler);
    MainWindow w;
    w.show();
    return a.exec();
}
