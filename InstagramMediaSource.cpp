#include "InstagramMediaSource.h"
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QApplication>

#include <QDebug>

InstagramMediaSource::InstagramMediaSource(QNetworkAccessManager *manager, QString username) : QObject()
{
	lastID = QString();
	this->manager = manager;
	metaReceivingError = false;
	canFetchMore = false;
	userName = username;

	if( !username.isEmpty() )
	{
		mainUrl = QString( "https://www.instagram.com/" ) + username + QString("/media/");
		canFetchMore = true;
	}
}

void InstagramMediaSource::load()
{
	if( canFetchMore ) loadMeta();
}

bool InstagramMediaSource::canLoad()
{
	return canFetchMore;
}

void InstagramMediaSource::loadMeta()
{
	QNetworkRequest request;
	QString target_url = lastID.isEmpty() ? mainUrl : ( mainUrl + QString("?max_id=") + lastID );
	request.setUrl( QUrl( target_url ) );
	QNetworkReply * reply = manager->get( request );
	connect( reply, SIGNAL(finished()), this, SLOT(metaReceived()) );
	connect( reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(metaRequestError(QNetworkReply::NetworkError)) );
	connect( reply, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(metaSslErrorHandler(QList<QSslError>)) );
}

void InstagramMediaSource::setUsername(QString username)
{
	if( !username.isEmpty() )
	{
		userName = username;
		mainUrl = QString( "https://www.instagram.com/" ) + username + QString("/media/");
		lastID = QString();
		metaReceivingError = false;
		canFetchMore = true;
	}
}

InstagramMediaSource::~InstagramMediaSource()
{

}

QVector<ImageInfo> InstagramMediaSource::parseMeta(QByteArray received_data)
{
	QVector<ImageInfo> result;
	result.reserve(20);

	QJsonParseError err;
	QJsonDocument document = QJsonDocument::fromJson( received_data, &err );
	if( err.error != QJsonParseError::NoError )
	{
		raiseParserInvalidDataError(err.errorString());
		return result;
	}
	if( !document.isObject())
	{
		raiseParserInvalidDataError(QString("Root is not an object"));
		result.clear();
		return result;
	}
	QJsonObject root_object = document.object();
	QJsonValue items_val = root_object["items"];
	if( !items_val.isArray() )
	{
		raiseParserInvalidDataError(QString("no items array found"));
		result.clear();
		return result;
	}
	QJsonArray items = items_val.toArray();
	for( int i = 0 ; i < items.count() ; i++)
	{
		QJsonObject item = items.at(i).toObject();
		if( item.isEmpty() )
		{
			raiseParserInvalidDataError(QString("No items array found"));
			return result;
		}

		QString type = item["type"].toString();
		if( type.isEmpty() )
		{
			raiseParserInvalidDataError( QString("Invalid item type") );
			continue;
		}
		if( type.compare( "image" ) != 0 ) continue;

		QJsonObject images = item["images"].toObject();
		if( images.isEmpty() )
		{
			raiseParserInvalidDataError( QString("Invalid item - no images field") );
			continue;
		}

		QJsonObject thumbnail_obj = images["thumbnail"].toObject();
		if( thumbnail_obj.isEmpty() )
		{
			raiseParserInvalidDataError( QString("Invalid item - no thumbnail") );
			continue;
		}

		QJsonObject standard_obj = images["standard_resolution"].toObject();
		if( standard_obj.isEmpty() )
		{
			raiseParserInvalidDataError( QString("Invalid item - no standard resolution image") );
			continue;
		}

		ImageInfo info;
		info.thumbnail_url = thumbnail_obj["url"].toString();
		info.standard_url = standard_obj["url"].toString();

		if( info.thumbnail_url.isEmpty() || info.standard_url.isEmpty() )
		{
			raiseParserInvalidDataError( QString("Invalid item - thumbnail or standard resolution image url missing") );
			continue;
		}

		result.append(info);

		lastID = item["id"].toString();
		if( lastID.isEmpty() )
		{
			raiseParserInvalidDataError( QString("Invalid item - no item ID passed") );
			continue;
		}
	}
	canFetchMore = root_object["more_available"].toBool(false);
	return result;
}


void InstagramMediaSource::raiseParserInvalidDataError(QString details)
{
	QMessageBox::critical( nullptr, QString("Invalid data"), QString("Invalid data received from host.\nDetails: ") + details);
}

void InstagramMediaSource::metaReceived()
{
	if(metaReceivingError)
	{
		metaReceivingError = false;
		return;
	}
	QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
	if( reply == 0 )
	{
		qDebug() << "InstagramMediaSource::metaRequestError() - Critical::Cannot cast sender() to a QNetworkReply*. Check signal sender at slot connect() call.";
		QCoreApplication::exit(EXIT_FAILURE);
	}
	QByteArray data = reply->readAll();
	if( data.isEmpty() )
	{
		if( reply->error() == QNetworkReply::NoError )
			QMessageBox::warning( nullptr, QString("Data not loaded"), QString("Mediadata acquiring error - Empty resource.") );
		else
			QMessageBox::warning( nullptr, QString("Data not loaded"), QString("Mediadata acquiring error.") + "\nDetails: " + reply->errorString() );
	}
	auto images_data = parseMeta( data );
	reply->deleteLater();

	if( !images_data.empty() ) emit imagesDataLoaded( images_data );
}



void InstagramMediaSource::metaRequestError(QNetworkReply::NetworkError error)
{
	if( error == QNetworkReply::NoError ) return;

	metaReceivingError = true;
	QNetworkReply * reply = qobject_cast<QNetworkReply *>(sender());
	if( reply == 0 )
	{
		qDebug() << "InstagramMediaSource::metaRequestError() - Critical::Cannot cast sender() to a QNetworkReply*. Check signal sender at slot connect() call.";
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

void InstagramMediaSource::metaSslErrorHandler(QList<QSslError> errors)
{
	QString errorText = "Secure connection error.";
	for( int i = 0 ; i < errors.size() ; i++ ) errorText += errors.at(i).errorString() + "\n";
	QMessageBox::critical( nullptr, QString("SSL error"), errorText );
	metaReceivingError = false;
}

