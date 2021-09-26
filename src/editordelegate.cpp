#include "editordelegate.h"

EditorDelegate::EditorDelegate(QWidget *parent) : QStyledItemDelegate(parent) {

}

bool EditorDelegate::eventFilter(QObject *watched, QEvent *event) {
    QWidget* editor = qobject_cast<QWidget*>(watched);
    if (editor && event->type() == QEvent::KeyPress) {
        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
        switch (key_event->key()) {
        case Qt::Key_Up: {
            closeEditor(editor);
            emit onCloseEditor();
            return true;
        }
        case Qt::Key_Home: {
            emit onHomePressed();
            break;
        }
        case Qt::Key_Plus: {
            emit onPlusPressed();
            return true;
        }
        }
    }
    return QStyledItemDelegate::eventFilter(watched, event);
}
