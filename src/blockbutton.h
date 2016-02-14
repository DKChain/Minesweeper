#ifndef BLOCKBUTTON
#define BLOCKBUTTON
#include <QPushButton>
#include <QMouseEvent>

class BlockButton: public QPushButton
{
    Q_OBJECT
public:
    BlockButton(QWidget *parent = 0);
private:
    void mousePressEvent(QMouseEvent *e);

signals:
    void rightClicked();
};

#endif // BLOCKBUTTON

