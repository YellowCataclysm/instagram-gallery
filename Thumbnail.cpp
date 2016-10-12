#include "Thumbnail.h"
#include <QApplication>
#include <QGraphicsOpacityEffect>

Thumbnail::Thumbnail(QWidget *parent)
	:QLabel(parent)
{
	this->setMinimumSize( 150, 150 );
	this->setMaximumSize( 150, 150 );
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
		connect( model, SIGNAL(thumbnailLoaded()), this, SLOT(thumbnailLoaded()) );
	}
	model->loadThumbnail();
	this->model = model;
}

void Thumbnail::thumbnailLoaded()
{
	this->setPixmap( model->getThumbnailPixmap() );

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

void Thumbnail::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}


void Thumbnail::enterEvent(QEvent *)
{
	QApplication::setOverrideCursor(QCursor(Qt::PointingHandCursor));
}

void Thumbnail::leaveEvent(QEvent *)
{
	QApplication::restoreOverrideCursor();
}
