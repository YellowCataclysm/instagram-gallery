#include "GalleryWindow.h"
#include "ui_GalleryWindow.h"
#include <QNetworkAccessManager>
#include <QGridLayout>
#include <QMovie>
#include <QMessageBox>
#include <assert.h>
#include "PhotoViewerDialog.h"
#include "Thumbnail.h"
#include "GalleryItemModel.h"

GalleryWindow::GalleryWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::GalleryWindow)
{
	ui->setupUi(this);
	ui->usernameLineEdit->setText("gwyn_nightfall");
	networkManager = new QNetworkAccessManager(this);
	connect(networkManager, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)), this, SLOT(networkStateChanged(QNetworkAccessManager::NetworkAccessibility)) );
	source = new InstagramMediaSource( networkManager);
	connect( ui->loadButton, SIGNAL(clicked()), this, SLOT(loadClicked()) );
	connect( source, SIGNAL(imagesDataLoaded(QVector<ImageInfo>)), this, SLOT(mediaDataLoaded(QVector<ImageInfo>)) );
	GalleryItemModel::setNetworkAccessManager( networkManager );
	thumbsLayout = ui->imagesGridLayout;

	nextCellCol = nextCellRow = 0;
	ui->loadMoreButton->hide();

	ui->loadingAnimationLabel->hide();
	QMovie * loader = new QMovie("://loader.gif", QByteArray(), ui->loadingAnimationLabel);
	ui->loadingAnimationLabel->setFrameStyle( QFrame::Panel | QFrame::Plain );
	ui->loadingAnimationLabel->setMovie( loader );

	connect( ui->loadMoreButton, SIGNAL(clicked()), this, SLOT(loadMoreClicked()) );

	photoViewer = new PhotoViewerDialog();
	photoViewer->hide();
	photoViewer->setModal(true);
	grabGesture( Qt::PanGesture );
}

GalleryWindow::~GalleryWindow()
{
	delete ui;
	delete source;
	if( !models.isEmpty() )
	{
		int modelsNum = models.size();
		for( int i = 0 ; i < modelsNum; i++)
		{
			delete models.takeFirst();
		}
	}
	for(int i = 0 ; i < thumbnails.size(); i++) delete thumbnails[i];
}

void GalleryWindow::loadClicked()
{
	QString username = ui->usernameLineEdit->text();
	if( !username.isEmpty() )
	{
		source->setUsername(username);
		for(int i = 0 ; i < thumbnails.size(); i++) thumbnails[i]->hide();
		ui->loadMoreButton->hide();
		nextCellCol = nextCellRow = 0;
		if( source->canLoad() ) source->load();
	}
}

void GalleryWindow::mediaDataLoaded(QVector<ImageInfo> data)
{
	for( int i = 0 ; i < data.size(); i++)
	{
		int currentCellIndex = nextCellRow * columnsNum + nextCellCol;
		bool canReuse = thumbnails.size() >  currentCellIndex;
        Thumbnail * thumb = nullptr;
        if( canReuse ) thumb = thumbnails[currentCellIndex];
		else
		{
			thumb = new Thumbnail();
			thumbsLayout->addWidget( thumb, nextCellRow , nextCellCol );
			connect( thumb, SIGNAL(doubleClicked()), this, SLOT(showStandardResolution()) );
			thumbnails.append(thumb);
		}

		auto item = data[i];
		GalleryItemModel * itemModel = new GalleryItemModel( item.thumbnail_url, item.standard_url);
        //models.append( itemModel );

        auto prevModel = thumb->setModel(itemModel);
        delete prevModel;
		thumb->show();

		nextCellCol++;
		if( nextCellCol >= columnsNum )
		{
			nextCellCol %= columnsNum;
			nextCellRow ++;
		}
	}
	ui->loadMoreButton->show();
	ui->loadMoreButton->setEnabled( source->canLoad() );
	ui->loadingAnimationLabel->movie()->stop();
	ui->loadingAnimationLabel->hide();
}

void GalleryWindow::loadMoreClicked()
{
	if( source->canLoad() )
	{
		ui->loadingAnimationLabel->show();
		ui->loadingAnimationLabel->movie()->start();
		ui->loadMoreButton->hide();
		source->load();
	}
	else
	{
		ui->loadingAnimationLabel->hide();
		ui->loadingAnimationLabel->movie()->stop();
		ui->loadMoreButton->show();
		ui->loadMoreButton->setEnabled(false);
	}

}

void GalleryWindow::showStandardResolution()
{
	Thumbnail * thumb = qobject_cast<Thumbnail*>(sender());
	assert( thumb != 0 );

	GalleryItemModel * model = thumb->getModel();
	assert( model != 0 );

	photoViewer->setPhoto( QPixmap() );
	QObject::connect( model, &GalleryItemModel::standardLoaded, [model, this]() {
		this->photoViewer->setPhoto( model->getStandardPixmap() );
	});
	model->loadStandard();
	photoViewer->show();
}

void GalleryWindow::networkStateChanged(QNetworkAccessManager::NetworkAccessibility accessible)
{
	switch( accessible )
	{
	case QNetworkAccessManager::UnknownAccessibility:
	case QNetworkAccessManager::NotAccessible:
		QMessageBox::warning( this, "Network error", "Network is unaccessible." );
		ui->loadMoreButton->setEnabled( false );
		break;

	case QNetworkAccessManager::Accessible:
		ui->loadMoreButton->setEnabled( true );
	}
}
