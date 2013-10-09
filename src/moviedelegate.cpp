#include "moviedelegate.h"

#include <QVariant>
#include <QAbstractItemView>
#include <QLabel>
#include <QMovie>

MovieDelegate::MovieDelegate(QAbstractItemView & view, QObject * parent)
		: QStyledItemDelegate(parent), view_(view)
{
}

void MovieDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QStyledItemDelegate::paint(painter, option, index);

	const QVariant & data = index.data(Qt::DecorationRole);

	QMovie * movie = qVariantToPointerToQMovie(data);

	if (!movie) {
		view_.setIndexWidget(index, NULL);
	} else {
		QObject * indexWidget = view_.indexWidget(index);
		QLabel * movieLabel = qobject_cast<QLabel *>(indexWidget);

		if (movieLabel) {
			// Reuse existing label

			if (movieLabel->movie() != movie) {
				movieLabel->setMovie(movie);
			}
		} else {
			// Create new label;

			movieLabel = new QLabel;

			movieLabel->setMovie(movie);

			view_.setIndexWidget(index, movieLabel);
		}
	}
}

//---------------------------------------------------------
// Private member functions
//---------------------------------------------------------

QMovie * MovieDelegate::qVariantToPointerToQMovie(const QVariant & variant) const
{
	if (!variant.canConvert<QMovie *>())
		return NULL;

	return variant.value<QMovie *>();
}
