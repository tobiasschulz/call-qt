#ifndef MOVIEDELEGATE_H
#define MOVIEDELEGATE_H

#include <QModelIndex>
#include <QStyledItemDelegate>
#include <QAbstractItemView>
#include <QMovie>

class MovieDelegate: public QStyledItemDelegate
{
Q_OBJECT

public:
	MovieDelegate(QAbstractItemView & view, QObject * parent = NULL);

	void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
	virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;

private:
	QMovie * qVariantToPointerToQMovie(const QVariant & variant) const;

private:
	QAbstractItemView & m_view;
};

Q_DECLARE_METATYPE(QMovie *)

#endif // MOVIEDELEGATE_H
