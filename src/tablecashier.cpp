#include "tablecashier.h"

TableCashier::TableCashier(QWidget *parent) : QTableView(parent) {

}

void TableCashier::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Delete: {
        if (currentIndex().row() == this->model()->rowCount() - 1) return;
        emit onDeletePressed(currentIndex());
        break;
    }
    case Qt::Key_Down: {
        if (currentIndex().row() + 1 == this->model()->rowCount() - 1 && state() != QAbstractItemView::EditingState) {
            QModelIndex idx = model()->index(currentIndex().row() + 1, 1);
            edit(idx);
        }
        QTableView::keyPressEvent(event);
        break;
    }
    case Qt::Key_Plus: {
        emit onPlusPressed();
        break;
    }
    case Qt::Key_Escape: {
        emit onEscPressed();
        QTableView::keyPressEvent(event);
        break;
    }
    default:
        if (currentIndex().row()  == this->model()->rowCount() - 1 && state() != QAbstractItemView::EditingState) {
            QModelIndex idx = model()->index(currentIndex().row(), 1);
            edit(idx);
        }
        QTableView::keyPressEvent(event);
    }
}

void TableCashier::onCloseEditor() {
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QTableView::keyPressEvent(event);
}
