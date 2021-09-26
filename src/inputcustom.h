#ifndef INPUTCUSTOM_H
#define INPUTCUSTOM_H

#include <QLineEdit>
#include <QKeyEvent>

class InputCustom : public QLineEdit {
    Q_OBJECT
public:
    explicit InputCustom(QWidget *parent = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;

signals:
    void onEscPressed();
};

#endif // INPUTCUSTOM_H
