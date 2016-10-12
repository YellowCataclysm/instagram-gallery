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
	bool networkUnaccessible;

private slots:
	void loadClicked();
	void mediaDataLoaded( QVector<ImageInfo> data);
	void loadMoreClicked();
	void showStandardResolution();
	void networkStateChanged(QNetworkAccessManager::NetworkAccessibility accessible);
};

#endif // GALLERYWINDOW_H
