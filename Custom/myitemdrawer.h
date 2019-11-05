#ifndef MYITEMDRAWER_H
#define MYITEMDRAWER_H

#include <QStyledItemDelegate>
#include <QIcon>

class MyItemDrawer : public QStyledItemDelegate
{
    Q_OBJECT
public:
    MyItemDrawer(QObject *parent);
    ~MyItemDrawer();

private:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
private:
    QPixmap m_f,m_d;
};

#endif // MYITEMDRAWER_H
