#include "PhotoViewerDialog.h"
#include "ui_PhotoViewerDialog.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPainter>
#include <cmath>
#include <QDebug>

template <typename T> int sign(T val) {
	return (T(0) < val) - (val < T(0));
}

PhotoViewerDialog::PhotoViewerDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PhotoViewerDialog)
{
	ui->setupUi(this);
	currentScale = 1.0;
	this->grabGesture( Qt::PanGesture );
	hOffset = vOffset = 0.0;
	zoomed = false;
	maxScale = 3.0;
	mousePressed = false;
}

PhotoViewerDialog::~PhotoViewerDialog()
{
	delete ui;
}

void PhotoViewerDialog::setPhoto(QPixmap photo)
{
	currentPixmap = photo;
	currentScale = 1.0f;
	zoomed = false;
	hOffset = vOffset = 0.0;
	mousePressed = false;
	if( !photo.isNull() )
	{
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
}

void PhotoViewerDialog::handleGestures(QGestureEvent *event)
{
	if( QGesture * pan = event->gesture( Qt::PanGesture )) handlePan( static_cast<QPanGesture*>(pan) );
}

void PhotoViewerDialog::handlePan(QPanGesture *pan)
{
	auto delta = pan->delta();
	hOffset += delta.x();
	vOffset += delta.y();
	clampOffset();
	update();
}

void PhotoViewerDialog::clampOffset()
{
	const qreal pw = currentPixmap.width() * currentScale;
	const qreal ph = currentPixmap.height() * currentScale;
	const qreal maxHOffest = pw / 2;
	const qreal maxVOffest = ph / 2;

	if( fabs(hOffset) > maxHOffest ) hOffset = maxHOffest * sign<qreal>( hOffset );
	if( fabs(vOffset) > maxVOffest ) vOffset = maxVOffest * sign<qreal>( vOffset );
}

bool PhotoViewerDialog::event(QEvent * event)
{
	if( event->type() == QEvent::Gesture ) handleGestures( static_cast<QGestureEvent*>(event) );
	return QDialog::event(event);
}

void PhotoViewerDialog::mouseDoubleClickEvent(QMouseEvent * event)
{
	currentScale = zoomed ? 1.0f : maxScale;
	zoomed = !zoomed;
	if( zoomed )
	{
		const qreal wCenterX = this->width() / 2.0 ;
		const qreal wCenterY = this->height() / 2.0;

		hOffset = (wCenterX - event->pos().x()) * maxScale;
		vOffset = (wCenterY - event->pos().y()) * maxScale;
		clampOffset();
	}
	update();
}


void PhotoViewerDialog::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	const qreal pw = currentPixmap.width();
	const qreal ph = currentPixmap.height();
	const qreal ww = this->width();
	const qreal wh = this->height();

	if( !currentPixmap.isNull() )
	{
		painter.translate( ww/2, wh/2 );
		if( zoomed ) painter.translate(hOffset, vOffset);
		painter.scale(currentScale, currentScale);
		painter.translate(-pw/2, -ph/2);
		painter.drawPixmap(0, 0, currentPixmap);
	}
	else
	{
		QPoint pos = QPoint( ww/2 - 50, wh/2 );
		painter.drawText( pos, "Loading..." );
	}

}


void PhotoViewerDialog::mousePressEvent(QMouseEvent * ev)
{
	if( ev->button() == Qt::LeftButton )
	{
		mousePressed = true;
		mousePrevPos = ev->pos();
	}
}

void PhotoViewerDialog::mouseReleaseEvent(QMouseEvent *ev)
{
	if( ev->button() == Qt::LeftButton ) mousePressed = false;
	else if( ev->button() == Qt::RightButton )
	{
		hOffset = vOffset = 0;
		update();
	}
}

void PhotoViewerDialog::mouseMoveEvent(QMouseEvent * ev)
{
	if( mousePressed )
	{
		auto delta = QPoint(ev->x() - mousePrevPos.x(),ev->y() - mousePrevPos.y());
		hOffset += delta.x();
		vOffset += delta.y();
		mousePrevPos = ev->pos();
		clampOffset();
		update();
	}
}
