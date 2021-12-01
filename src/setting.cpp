#include "setting.h"

Setting * p_instance = 0;

Setting::Setting() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    if (settings.value("init").toBool()) return;
    settings.setValue("init", true);
    settings.setValue("api", "http://localhost:8000");
    settings.setValue("printer", "EPSON TM-U220 Receipt");
    settings.setValue("printer-port", "COM10");
}

QString Setting::getApi() {
    if (api != "") return api;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    api = settings.value("api", "http://localhost:8000").toString();
    return api;
}

QString Setting::getAuthToken() {
    return token;
}

void Setting::setAuthToken(const QString &token) {
    this->token = token;
}

void Setting::deleteAuthToken() {
    token = "";
}

QString Setting::getPrinter() {
    if (printer != "") return printer;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    printer = settings.value("printer", "EPSON TM-U220 Receipt").toString();
    return printer;
}

QString Setting::getPrinterPort() {
    if (port != "") return port;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    port = settings.value("printer-port", "COM10").toString();
    return port;
}

bool Setting::getDebug() {
    if (debug != "") return debug.toLower() == "true";
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    debug = settings.value("debug", false).toString();
    return debug.toLower() == "true";
}
