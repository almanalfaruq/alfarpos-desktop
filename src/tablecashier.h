#ifndef TABLECASHIER_H
#define TABLECASHIER_H

#include <QTableView>
#include <QKeyEvent>
#include <QDebug>

class TableCashier : public QTableView {
    Q_OBJECT
public:
    explicit TableCashier(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);

public slots:
    void onCloseEditor();

signals:
    void onDeletePressed(const QModelIndex &);
    void onPlusPressed();
    void onEscPressed();
};

#endif // TABLECASHIER_H
