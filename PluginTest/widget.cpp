#include "widget.h"
#include "ui_widget.h"
#include <QMovie>
#include <QDebug>
#include <QBuffer>

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);

	qDebug() << QImageReader::supportedImageFormats();
	qDebug() << QMovie::supportedFormats();

	qDebug() << QImageReader(":/testData/pngSample.png", "apng").supportsAnimation();
	qDebug() << QImageReader(":/testData/apngSample.apng", "apng").supportsAnimation();
	qDebug() << QImageReader(":/testData/ppngSample.png", "apng").supportsAnimation();

	//load images
	this->ui->labelImagePng->setPixmap(QPixmap::fromImage(QImage(":/testData/pngSample.png")));
	this->ui->labelImageApng->setPixmap(QPixmap::fromImage(QImage(":/testData/apngSample.apng")));
	this->ui->labelImagePpng->setPixmap(QPixmap::fromImage(QImage(":/testData/ppngSample.png")));

	auto moviePng = new QMovie(":/testData/pngSample.png");
	moviePng->start();
	this->ui->labelMoviePng->setMovie(moviePng);

	auto movieApng = new QMovie(":/testData/apngSample.apng");
	movieApng->start();
	this->ui->labelMovieApng->setMovie(movieApng);

	auto moviePpng = new QMovie(":/testData/ppngSample.png");
	moviePpng->start();
	this->ui->labelMoviePpng->setMovie(moviePpng);
}

Widget::~Widget()
{
	delete ui;
}
