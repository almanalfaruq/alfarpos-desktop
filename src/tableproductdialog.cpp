#include "tableproductdialog.h"

TableProductDialog::TableProductDialog(QWidget *parent) : QTableView(parent) {

}

void TableProductDialog::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Enter: {
        }
        case Qt::Key_Return: {
            emit onEnterPressed(currentIndex());
            break;
        }
        case Qt::Key_Escape: {
            emit onEscPressed();
            break;
        }
        default: QTableView::keyPressEvent(event);
    }
}
