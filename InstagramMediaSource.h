#ifndef INSTAGRAMMEDIASOURCE_H
#define INSTAGRAMMEDIASOURCE_H

#include <QVector>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QStringList>

struct ImageInfo
{
	QString thumbnail_url;
	QString standard_url;
};

class InstagramMediaSource : public QObject
{
	Q_OBJECT

public:
	InstagramMediaSource( QNetworkAccessManager * manager, QString username = QString() );
	void load();
	bool canLoad();
	void setUsername( QString username );
	QString username() { return userName; }
	~InstagramMediaSource();

signals:
	void imagesDataLoaded(QVector<ImageInfo>);
	void loadingFailed();

private:
	QString userName;
	QString mainUrl;
	QString lastID;
	QNetworkAccessManager * manager;
	QVector<ImageInfo> buffered;

	bool metaReceivingError;
	bool canFetchMore;

	void loadMeta();
	QVector<ImageInfo> parseMeta( QByteArray received_data );
	void raiseParserInvalidDataError( QString details );

private slots:
	void metaReceived();
	void metaRequestError(QNetworkReply::NetworkError error);
	void metaSslErrorHandler( QList<QSslError> errors );
};

#endif // INSTAGRAMMEDIASOURCE_H
