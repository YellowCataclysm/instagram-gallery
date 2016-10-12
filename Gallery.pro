#-------------------------------------------------
#
# Project created by QtCreator 2016-10-10T12:54:44
#
#-------------------------------------------------

QT       += core gui
QT += network
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gallery
TEMPLATE = app

QMAKE_RPATHDIR += ./libs


SOURCES += main.cpp\
        GalleryWindow.cpp \
    InstagramMediaSource.cpp \
    Thumbnail.cpp \
    GalleryItemModel.cpp \
    PhotoViewerDialog.cpp

HEADERS  += GalleryWindow.h \
    InstagramMediaSource.h \
    Thumbnail.h \
    GalleryItemModel.h \
    PhotoViewerDialog.h

FORMS    += GalleryWindow.ui \
    PhotoViewerDialog.ui

RESOURCES += \
    resources.qrc
