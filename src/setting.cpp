#include "setting.h"

Setting::Setting() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    if (settings.value("init").toBool()) return;
    settings.setValue("init", true);
    settings.setValue("api", "http://localhost:8000");
    settings.setValue("printer", "EPSON TM-U220 Receipt");
    settings.setValue("printer-port", "COM10");
}

QString Setting::getApi() const {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    return settings.value("api", "http://localhost:8000").toString();
}

void Setting::setApi(const QString &value) {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    settings.setValue("api", value);
}

QString Setting::getAuthToken() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    return settings.value("token", "").toString();
}

void Setting::setAuthToken(const QString &token) {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    settings.setValue("token", token);
}

void Setting::deleteAuthToken() {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    settings.setValue("token", "");
}

QString Setting::getPrinter() const {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    return settings.value("printer", "EPSON TM-U220 Receipt").toString();
}

QString Setting::getPrinterPort() const {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    return settings.value("printer-port", "COM10").toString();
}

bool Setting::getDebug() const {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Alfar-Dev", "AlfarPOS");
    return settings.value("debug", false).toBool();
}
