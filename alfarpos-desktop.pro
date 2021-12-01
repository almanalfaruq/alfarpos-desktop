QT       += core gui network printsupport serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/customer.cpp \
    src/editordelegate.cpp \
    src/eventtable.cpp \
    src/inputcustom.cpp \
    src/inputmoneytransaction.cpp \
    src/main.cpp \
    src/mainwindow.cpp \
    src/menumodel.cpp \
    src/menuwindow.cpp \
    src/money.cpp \
    src/order.cpp \
    src/orderwindow.cpp \
    src/orderwindowmodel.cpp \
    src/payment.cpp \
    src/paymentdialog.cpp \
    src/printer.cpp \
    src/product.cpp \
    src/product_dialog.cpp \
    src/productmodel.cpp \
    src/productordermodel.cpp \
    src/productselectmodel.cpp \
    src/setting.cpp \
    src/shopprofile.cpp \
    src/spinboxpayment.cpp \
    src/tablecashier.cpp \
    src/transactiontablemodel.cpp \
    src/transactionwindow.cpp \
    src/user.cpp

HEADERS += \
    src/customer.h \
    src/editordelegate.h \
    src/eventtable.h \
    src/inputcustom.h \
    src/inputmoneytransaction.h \
    src/mainwindow.h \
    src/menumodel.h \
    src/menuwindow.h \
    src/money.h \
    src/order.h \
    src/orderwindow.h \
    src/orderwindowmodel.h \
    src/payment.h \
    src/paymentdialog.h \
    src/printer.h \
    src/product.h \
    src/product_dialog.h \
    src/productmodel.h \
    src/productordermodel.h \
    src/productselectmodel.h \
    src/setting.h \
    src/shopprofile.h \
    src/spinboxpayment.h \
    src/tablecashier.h \
    src/transactiontablemodel.h \
    src/transactionwindow.h \
    src/user.h

FORMS += \
    src/mainwindow.ui \
    src/menuwindow.ui \
    src/orderwindow.ui \
    src/paymentdialog.ui \
    src/product_dialog.ui \
    src/transactionwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
