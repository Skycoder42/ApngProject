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

	//load images
	this->ui->labelImagePng->setPixmap(QPixmap::fromImage(QImage(":/testData/pngSample.png")));
	this->ui->labelImageApng->setPixmap(QPixmap::fromImage(QImage(":/testData/apngSample.apng")));
	this->ui->labelImagePpng->setPixmap(QPixmap::fromImage(QImage(":/testData/ppngSample.png")));

	auto moviePng = new QMovie(":/testData/pngSample.png");
	connect(moviePng, &QMovie::finished, moviePng, &QMovie::start);
	moviePng->start();
	this->ui->labelMoviePng->setMovie(moviePng);

	auto movieApng = new QMovie(":/testData/apngSample.apng");
	connect(movieApng, &QMovie::finished, movieApng, &QMovie::start);
	movieApng->start();
	this->ui->labelMovieApng->setMovie(movieApng);

	auto moviePpng = new QMovie(":/testData/ppngSample.png");
	connect(moviePpng, &QMovie::finished, moviePpng, &QMovie::start);
	moviePpng->start();
	this->ui->labelMoviePpng->setMovie(moviePpng);
}

Widget::~Widget()
{
	delete ui;
}
