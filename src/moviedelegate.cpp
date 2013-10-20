#include "moviedelegate.h"

#include <QVariant>
#include <QAbstractItemView>
#include <QLabel>
#include <QMovie>

MovieDelegate::MovieDelegate(QAbstractItemView & view, QObject * parent)
		: QStyledItemDelegate(parent), m_view(view)
{
}

void MovieDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	const QVariant & data = index.data(Qt::DecorationRole);
	QMovie * movie = qVariantToPointerToQMovie(data);

	if (!movie) {
		QStyledItemDelegate::paint(painter, option, index);
		m_view.setIndexWidget(index, NULL);
	} else {
		QObject * indexWidget = m_view.indexWidget(index);
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

			m_view.setIndexWidget(index, movieLabel);
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
