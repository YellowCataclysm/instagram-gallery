#ifndef GALLERYITEMMODEL_H
#define GALLERYITEMMODEL_H

#include <QObject>
#include <QNetworkReply>
#include <QPixmap>

class QLabel;
class QPixmap;
class QNetworkAccessManager;

class GalleryItemModel : public QObject
{
	Q_OBJECT
public:
	explicit GalleryItemModel( QString thumbnail_url, QString standard_image_url, QObject *parent = 0);
	static void setNetworkAccessManager(QNetworkAccessManager * manager);

	void loadThumbnail();
	void loadStandard();

	QPixmap getThumbnailPixmap() { return thumbnail.pixmap; }
	QPixmap getStandardPixmap()	{ return standard.pixmap; }

	~GalleryItemModel();

private:
	struct
	{
		QString url;
		QPixmap pixmap;
	} thumbnail, standard;

	static QNetworkAccessManager * manager ;

signals:
	void thumbnailLoaded();
	void standardLoaded();

private slots:
	void thumbnailLoadingFinished();
	void standardLoadingFinished();
	void loadingImageError( QNetworkReply::NetworkError err );
};

#endif // GALLERYITEMMODEL_H
