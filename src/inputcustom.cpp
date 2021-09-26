#include "inputcustom.h"

InputCustom::InputCustom(QWidget *parent) : QLineEdit(parent) {

}

void InputCustom::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
    case Qt::Key_Escape: {
        emit onEscPressed();
        QLineEdit::keyPressEvent(event);
        break;
    }
    default:
        QLineEdit::keyPressEvent(event);
    }
}
