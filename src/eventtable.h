#ifndef EVENTTABLE_H
#define EVENTTABLE_H

#include <QTableView>
#include <QDebug>
#include <QKeyEvent>

class EventTable : public QTableView {
    Q_OBJECT

public:
    explicit EventTable(QWidget *parent = nullptr);

protected:
    void selectionChanged(const QItemSelection &,
                          const QItemSelection &) override;
    void keyPressEvent(QKeyEvent *event) override;

signals:
    void onSelectionChange(const QModelIndex &);
    void onEnterPressed(const QModelIndex &);
    void onEscPressed();
};

#endif // EVENTTABLE_H
