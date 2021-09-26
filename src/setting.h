#ifndef SETTING_H
#define SETTING_H

#include <QString>
#include <QSettings>

class Setting {
public:
    Setting();
    QString getApi() const;
    void setApi(const QString &value);

    QString getAuthToken();
    void setAuthToken(const QString &);
    void deleteAuthToken();
    QString getPrinter() const;
    QString getPrinterPort() const;
    bool getDebug() const;
};

#endif // SETTING_H
