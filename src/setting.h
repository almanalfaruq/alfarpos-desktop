#ifndef SETTING_H
#define SETTING_H

#include <QString>
#include <QSettings>

class Setting final {
public:
    static Setting& getInstance() {
        static Setting instance;
        return instance;
    }

    QString getApi();
    QString getAuthToken();
    void setAuthToken(const QString &);
    void deleteAuthToken();
    QString getPrinter();
    QString getPrinterPort();
    bool getDebug();

private:
    Setting();
    ~Setting() = default;

    // Delete the copy and move constructors
    Setting(const Setting&) = delete;
    Setting& operator=(const Setting&) = delete;
    Setting(Setting&&) = delete;
    Setting& operator=(Setting&&) = delete;

    QString api = "", token = "", printer = "", port = "", debug = "";
};

#endif // SETTING_H
