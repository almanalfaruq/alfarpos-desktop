#ifndef TABLEPRODUCTDIALOG_H
#define TABLEPRODUCTDIALOG_H

#include <QTableView>
#include <QKeyEvent>

class TableProductDialog : public QTableView
{
    Q_OBJECT
public:
    explicit TableProductDialog(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event);

signals:
    void onEnterPressed(const QModelIndex &);
    void onEscPressed();
};

#endif // TABLEPRODUCTDIALOG_H
