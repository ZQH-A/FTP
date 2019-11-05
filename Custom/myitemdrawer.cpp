#include "myitemdrawer.h"
#include <QPainter>

MyItemDrawer::MyItemDrawer(QObject *parent) :QStyledItemDelegate(parent),m_f("/run/media/root/343c117f-ca55-4783-9631-a7a6c5c319cf/lost+found/zengqianghui/QT/listdelegete/icon/document.ico"),m_d("/run/media/root/343c117f-ca55-4783-9631-a7a6c5c319cf/lost+found/zengqianghui/QT/listdelegete/icon/folder.ico")
{

}

MyItemDrawer::~MyItemDrawer()
{

}

void MyItemDrawer::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QRect rect=option.rect;
    rect.adjust(2,2,-2,-2);

    QString name=index.data(Qt::DisplayRole).toString();
    QString size =index.data(Qt::UserRole+1).toString();
    QString kind=index.data(Qt::UserRole+2).toString();
    QString time=index.data(Qt::UserRole+3).toString();

    if(option.state & QStyle::State_Selected)
    {
        painter->setBrush(QColor(0xCC,0xAA,0xAA));
        painter->drawRoundedRect(rect,2,2);
        painter->setBrush(Qt::NoBrush);
    }

    if(1)//图片
    {
        QRect dst=rect;
        dst.setRight(rect.left()+30);
        QRect area(0,0,24,24);
        area.moveCenter(dst.center());
        if (kind=="目录")
            painter->drawPixmap(area,m_d);
        else
            painter->drawPixmap(area,m_f);
    }

    if(1)//文件名
    {
        QRect dst=rect;
        dst.setLeft(rect.left()+30);
        dst.setRight(rect.left()+150);
        painter->drawText(dst,Qt::AlignLeft|Qt::AlignVCenter,name);
    }

    if(1)//文件大小
    {
        QRect dst=rect;
        dst.setLeft(rect.left()+120);
        dst.setRight(rect.left()+160);
        painter->drawText(dst,Qt::AlignRight|Qt::AlignVCenter,size);
    }

    if(1)//文件类型
    {
        QRect dst=rect;
        dst.setLeft(rect.left()+220);
        dst.setRight(rect.left()+300);
        painter->drawText(dst,Qt::AlignCenter,kind);
    }
    if(1)//文件最后修改时间
    {
        QRect dst=rect;
        dst.setLeft(rect.left()+320);
        dst.setRight(rect.left()+500);
        painter->drawText(dst,Qt::AlignLeft|Qt::AlignVCenter,time);
    }
}

QSize MyItemDrawer::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size=QStyledItemDelegate::sizeHint(option,index);
    size.setHeight(40);
    return size;
}
