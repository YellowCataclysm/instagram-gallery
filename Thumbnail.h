#ifndef THUMBNAIL_H
#define THUMBNAIL_H

#include <QLabel>
#include "GalleryItemModel.h"
#include <QPropertyAnimation>

class Thumbnail : public QLabel
{
	Q_OBJECT

public:
	Thumbnail( QWidget * parent = 0 );
	~Thumbnail();

	void setModel( GalleryItemModel * model );
	GalleryItemModel * getModel() { return model; }

private:
	GalleryItemModel * model;
	QPropertyAnimation * fadeInAnimation;

signals:
	void doubleClicked();

private slots:
	void thumbnailLoaded(bool success);

	// QWidget interface
protected:
	virtual void mouseDoubleClickEvent(QMouseEvent *);

	// QWidget interface
protected:
	void enterEvent(QEvent *);
	void leaveEvent(QEvent *);
};

#endif // THUMBNAIL_H
