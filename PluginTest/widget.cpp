#include "widget.h"
#include "ui_widget.h"
#include <QMovie>

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);

	//load images
	this->ui->labelImagePng->setPixmap(QPixmap::fromImage(QImage(":/testData/pngSample.png")));
	this->ui->labelImageApng->setPixmap(QPixmap::fromImage(QImage(":/testData/apngSample.apng")));
	this->ui->labelImagePpng->setPixmap(QPixmap::fromImage(QImage(":/testData/ppngSample.png")));

	auto moviePng = new QMovie(":/testData/pngSample.png");
	moviePng->setCacheMode(QMovie::CacheAll);
	moviePng->start();
	this->ui->labelMoviePng->setMovie(moviePng);
	auto movieApng = new QMovie(":/testData/apngSample.apng");
	movieApng->setCacheMode(QMovie::CacheAll);
	movieApng->start();
	this->ui->labelMovieApng->setMovie(movieApng);
	auto moviePpng = new QMovie(":/testData/ppngSample.png");
	moviePpng->setCacheMode(QMovie::CacheAll);
	moviePpng->start();
	this->ui->labelMoviePpng->setMovie(moviePpng);
}

Widget::~Widget()
{
	delete ui;
}
