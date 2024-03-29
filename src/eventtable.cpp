#include "eventtable.h"

EventTable::EventTable(QWidget *parent) : QTableView(parent) {

}

void EventTable::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) {
    emit onSelectionChange(currentIndex());
    QTableView::selectionChanged(selected, deselected);
}

void EventTable::keyPressEvent(QKeyEvent *event) {
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
    default:
        QTableView::keyPressEvent(event);
    }
}
