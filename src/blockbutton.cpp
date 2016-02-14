#include "blockbutton.h"

BlockButton::BlockButton(QWidget *parent):
    QPushButton(parent)
{

}


void BlockButton::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
        emit clicked();
    if(e->button() == Qt::RightButton)
        emit rightClicked();
}
