#ifndef GALLERYWINDOW_H
#define GALLERYWINDOW_H

#include <QMainWindow>
#include <QList>
#include "InstagramMediaSource.h"

class QNetworkAccessManager;
class QGridLayout;
class Thumbnail;
class GalleryItemModel;
class PhotoViewerDialog;

namespace Ui {
class GalleryWindow;
}

class GalleryWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit GalleryWindow(QWidget *parent = 0);
	~GalleryWindow();

private:
	Ui::GalleryWindow *ui;
	InstagramMediaSource * source;
	QNetworkAccessManager * networkManager;
	QGridLayout * thumbsLayout;
	QVector<Thumbnail*> thumbnails;
	QList<GalleryItemModel*> models;
	PhotoViewerDialog * photoViewer;
	const int columnsNum = 4;
	int nextCellRow;
	int nextCellCol;

private slots:
	void loadClicked();
	void mediaDataLoaded( QVector<ImageInfo> data);
	void loadMoreClicked();
	void thumbnailDoubleClicked();
};

#endif // GALLERYWINDOW_H
