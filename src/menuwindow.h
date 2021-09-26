#ifndef MENUWINDOW_H
#define MENUWINDOW_H

#include <QWidget>

#include "menumodel.h"

namespace Ui {
class MenuWindow;
}

class MenuWindow : public QWidget {
    Q_OBJECT

public:
    explicit MenuWindow(QWidget *parent = nullptr);
    ~MenuWindow();

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::MenuWindow *ui;

    MenuModel *model;

private slots:
    void selectMenu(int index);
    void onEscPressed();

signals:
    void onSelectMenu(int index);
    void onCloseEvent();
};

#endif // MENUWINDOW_H
