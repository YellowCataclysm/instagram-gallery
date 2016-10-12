#include "PhotoViewerDialog.h"
#include "ui_PhotoViewerDialog.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QDebug>

PhotoViewerDialog::PhotoViewerDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PhotoViewerDialog)
{
	ui->setupUi(this);
	zoomed = false;
	zoomFactor = 2.0f;
	this->grabGesture( Qt::PanGesture );
}

PhotoViewerDialog::~PhotoViewerDialog()
{
	delete ui;
}

void PhotoViewerDialog::setPhoto(QPixmap photo)
{
	ui->photoLabel->setPixmap( photo );
	if( !photo.isNull() )
	{
		originalImageSize = photo.size();
		//ui->scrollAreaWidgetContents->setMinimumSize( originalImageSize );

		QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(this);
		ui->photoLabel->setGraphicsEffect(effect);
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
	zoomed = false;
}

void PhotoViewerDialog::handleGestures(QGestureEvent *event)
{
	if( QGesture * pan = event->gesture( Qt::PanGesture )) handlePan( static_cast<QPanGesture*>(pan) );
}

void PhotoViewerDialog::handlePan(QPanGesture *pan)
{
	qDebug() << "Pan gesture recognized";
	auto delta = pan->delta().toPoint();
	ui->scrollArea->scroll( delta.x(), delta.y() );
}

void PhotoViewerDialog::handlePinch(QPinchGesture *pinch)
{

}


bool PhotoViewerDialog::event(QEvent * event)
{
	if( event->type() == QEvent::Gesture ) handleGestures( static_cast<QGestureEvent*>(event) );
	return QDialog::event(event);
}

void PhotoViewerDialog::mouseDoubleClickEvent(QMouseEvent * event)
{
	QPoint inAreaPos = ui->scrollAreaWidgetContents->mapFromGlobal(event->globalPos());
	if( !ui->scrollAreaWidgetContents->visibleRegion().contains( inAreaPos ) ) return;
	QSize target = zoomed ? originalImageSize : originalImageSize * zoomFactor;
	ui->photoLabel->setPixmap( ui->photoLabel->pixmap()->scaled( target, Qt::AspectRatioMode::KeepAspectRatio ) );
	if( !zoomed )
	{

	}
	zoomed = !zoomed;
}
