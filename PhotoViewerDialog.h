#ifndef PHOTOVIEWERDIALOG_H
#define PHOTOVIEWERDIALOG_H

#include <QDialog>
#include <QGestureEvent>

namespace Ui {
class PhotoViewerDialog;
}

class PhotoViewerDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PhotoViewerDialog(QWidget *parent = 0);
	~PhotoViewerDialog();

public slots:
	void setPhoto( QPixmap photo );

private:
	Ui::PhotoViewerDialog *ui;
	QPixmap currentPixmap;
	qreal currentScale;
	qreal maxScale;
	qreal hOffset;
	qreal vOffset;
	bool zoomed;
	bool mousePressed;
	QPoint mousePrevPos;

	void handleGestures( QGestureEvent * event );
	void handlePan( QPanGesture * pan );
	void handlePinch( QPinchGesture * pinch );

	// QWidget interface
protected:
	bool event(QEvent *event) override;
	void mouseDoubleClickEvent(QMouseEvent *event);

	// QWidget interface
protected:
	void paintEvent(QPaintEvent *);

	// QWidget interface
protected:
	void mousePressEvent(QMouseEvent *);
	void mouseReleaseEvent(QMouseEvent *);
	void mouseMoveEvent(QMouseEvent *);
};

#endif // PHOTOVIEWERDIALOG_H
