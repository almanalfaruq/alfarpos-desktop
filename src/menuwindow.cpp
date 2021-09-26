#include "menuwindow.h"
#include "ui_menuwindow.h"

MenuWindow::MenuWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MenuWindow) {
    ui->setupUi(this);
    model = new MenuModel();
    ui->tvMenu->setModel(model);
    ui->tvMenu->setSelectionBehavior(QHeaderView::SelectRows);
    for (int c = 0; c < ui->tvMenu->horizontalHeader()->count(); ++c) {
        if (c == 0) {
            ui->tvMenu->horizontalHeader()->resizeSection(c, 400);
            continue;
        }
        if (c == 1) {
            ui->tvMenu->horizontalHeader()->resizeSection(c, 100);
            continue;
        }
        ui->tvMenu->horizontalHeader()->setSectionResizeMode(
            c, QHeaderView::Stretch);
    }
    connect(ui->tvMenu, SIGNAL(onEnterPressed(QModelIndex)), model, SLOT(onEnterPressed(QModelIndex)), Qt::QueuedConnection);
    connect(ui->tvMenu, SIGNAL(onEscPressed()), this, SLOT(onEscPressed()), Qt::QueuedConnection);
    connect(model, SIGNAL(onSelectIndex(int)), this, SLOT(selectMenu(int)), Qt::QueuedConnection);
}

MenuWindow::~MenuWindow() {
    delete ui;
}

void MenuWindow::closeEvent(QCloseEvent *) {
    emit onCloseEvent();
}

void MenuWindow::selectMenu(int index) {
    emit onSelectMenu(index);
    this->close();
}

void MenuWindow::onEscPressed() {
    this->close();
}
