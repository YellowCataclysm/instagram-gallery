#include "Thumbnail.h"
#include <QApplication>
#include <QGraphicsOpacityEffect>

Thumbnail::Thumbnail(QWidget *parent)
	:QLabel(parent)
{
	this->setMinimumSize( 150, 150 );
	this->setMaximumSize( 150, 150 );
	setAlignment( Qt::AlignCenter );
	model = nullptr;
}

Thumbnail::~Thumbnail()
{

}

void Thumbnail::setModel(GalleryItemModel *model)
{
	if( this->model != nullptr ) disconnect(this->model, SIGNAL(thumbnailLoaded()), this, SLOT(thumbnailLoaded()));
	if( model != nullptr )
	{
		setPixmap(QPixmap());
		setText("Loading...");
		connect( model, SIGNAL(thumbnailLoaded(bool)), this, SLOT(thumbnailLoaded(bool)) );
	}
	model->loadThumbnail();
	this->model = model;
}

void Thumbnail::thumbnailLoaded( bool success )
{
	if( success )
	{
		this->setPixmap( model->getThumbnailPixmap() );
		setText(QString());
		// Setup fadeIn animation
		QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);
		this->setGraphicsEffect(effect);
		QPropertyAnimation* a = new QPropertyAnimation( effect, "opacity", this );

		// Disable effect to prevent rendering bugs after animation finished
		QObject::connect( a, &QPropertyAnimation::finished, [effect]() {
			effect->setEnabled(false);
		});
		a->setDuration(500);
		a->setStartValue(0);
		a->setEndValue(1);
		a->start(QAbstractAnimation::DeleteWhenStopped);
	}
	else
	{
		setText("Loading failed");
	}

}

void Thumbnail::mouseDoubleClickEvent(QMouseEvent *)
{
	if( pixmap()->isNull() && model != nullptr )
	{
		model->loadThumbnail();	// Try to reload
		setText("Loading...");
	}
	else emit doubleClicked();
}


void Thumbnail::enterEvent(QEvent *)
{
	QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
}

void Thumbnail::leaveEvent(QEvent *)
{
	QApplication::restoreOverrideCursor();
}
