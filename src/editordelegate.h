#ifndef EDITORDELEGATE_H
#define EDITORDELEGATE_H

#include <QStyledItemDelegate>
#include <QKeyEvent>

class EditorDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    explicit EditorDelegate(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *editor, QEvent *event);

signals:
    void onCloseEditor();
    void onHomePressed();
    void onPlusPressed();
};

#endif // EDITORDELEGATE_H
