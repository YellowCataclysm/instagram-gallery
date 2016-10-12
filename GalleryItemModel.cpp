#include "GalleryItemModel.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QImage>
#include <QLabel>
#include <QApplication>
#include <QMessageBox>
#include <assert.h>

QNetworkAccessManager* GalleryItemModel::manager = nullptr;

GalleryItemModel::GalleryItemModel(QString thumbnail_url, QString standard_image_url, QObject *parent) : QObject(parent)
{
	thumbnail.url = thumbnail_url;
	thumbnail.pixmap = QPixmap();

	standard.url = standard_image_url;
	standard.pixmap = QPixmap();
}

void GalleryItemModel::setNetworkAccessManager(QNetworkAccessManager *manager)
{
	GalleryItemModel::manager = manager;
}

void GalleryItemModel::loadThumbnail()
{
	if( !thumbnail.pixmap.isNull() )
	{
		emit thumbnailLoaded();
		return;
	}

	assert( GalleryItemModel::manager != nullptr );
	QNetworkRequest request;
	request.setUrl( QUrl( thumbnail.url ) );
	QNetworkReply * reply = manager->get( request );
	connect( reply, SIGNAL(finished()), this, SLOT(thumbnailLoadingFinished()) );
	connect( reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(loadingImageError(QNetworkReply::NetworkError)));
}

void GalleryItemModel::loadStandard()
{
	if( !standard.pixmap.isNull() )
	{
		emit standardLoaded();
		return;
	}

	assert( GalleryItemModel::manager != nullptr );
	QNetworkRequest request;
	request.setUrl( QUrl( standard.url ) );
	QNetworkReply * reply = manager->get( request );
	connect( reply, SIGNAL(finished()), this, SLOT(standardLoadingFinished()) );
	connect( reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(loadingImageError(QNetworkReply::NetworkError)));
}

GalleryItemModel::~GalleryItemModel()
{

}

void GalleryItemModel::thumbnailLoadingFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
	if( reply == 0 )
	{
		qDebug() << "GalleryItemModel::thumbnailLoadingFinished() - Critical::Cannot cast sender() to a QNetworkReply*. Check signal sender at slot connect() call.";
		QCoreApplication::exit(EXIT_FAILURE);
	}

	if( reply->error() != QNetworkReply::NoError ) return;

	QByteArray data = reply->readAll();
	if( data.isEmpty() )
	{
		QMessageBox::warning( nullptr, QString("Data not loaded"), QString("Mediadata acquiring error.") + "\nDetails: " + reply->errorString()  );
		return;
	}

	if( thumbnail.pixmap.loadFromData( data ) == false )
	{
		QMessageBox::critical( nullptr, QString("Image conversion failed"), QString("Cannot convert received data to image object."));
		return;
	}
	reply->deleteLater();
	emit thumbnailLoaded();
}

void GalleryItemModel::standardLoadingFinished()
{
	QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
	if( reply == 0 )
	{
		qDebug() << "GalleryItemModel::standardLoadingFinished() - Critical::Cannot cast sender() to a QNetworkReply*. Check signal sender at slot connect() call.";
		QCoreApplication::exit(EXIT_FAILURE);
	}

	if( reply->error() != QNetworkReply::NoError ) return;

	QByteArray data = reply->readAll();
	if( data.isEmpty() )
	{
		QMessageBox::warning( nullptr, QString("Data not loaded"), QString("Mediadata acquiring error.") + "\nDetails: " + reply->errorString()  );
		return;
	}

	if( standard.pixmap.loadFromData( data ) == false )
	{
		QMessageBox::critical( nullptr, QString("Image conversion failed"), QString("Cannot convert received data to image object."));
		return;
	}
	reply->deleteLater();
	emit standardLoaded();
}

void GalleryItemModel::loadingImageError(QNetworkReply::NetworkError error)
{
	if( error == QNetworkReply::NoError ) return;
	QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
	if( reply == 0 )
	{
		qDebug() << "GalleryItemModel::loadingImageError() - Critical::Cannot cast sender() to a QNetworkReply*. Check signal sender at slot connect() call.";
		QCoreApplication::exit(EXIT_FAILURE);
	}
	if( error < QNetworkReply::BackgroundRequestNotAllowedError)
	{
		QMessageBox::critical( nullptr, QString("Connection error"), QString("Cannot connect to target host.\nDetails: ") + reply->errorString() + "\nCheck your internet connection."  );
		return;
	}
	else
	{
		QMessageBox::critical( nullptr, QString("Server error"), QString("Data acquiring error.\nDetails: ") + reply->errorString()  );
		return;
	}
}

