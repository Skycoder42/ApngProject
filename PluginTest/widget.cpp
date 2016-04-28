#include "widget.h"
#include "ui_widget.h"
#include <QMovie>

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget)
{
	ui->setupUi(this);

	//load images
	this->ui->labelImagePng->setPixmap(QPixmap::fromImage(QImage("C:/Users/felix/Programming/QtProjects/ApngProject/PluginTest/testData/pngSample.png")));
	this->ui->labelImageApng->setPixmap(QPixmap::fromImage(QImage("C:/Users/felix/Programming/QtProjects/ApngProject/PluginTest/testData/apngSample.apng")));
	this->ui->labelImagePpng->setPixmap(QPixmap::fromImage(QImage("C:/Users/felix/Programming/QtProjects/ApngProject/PluginTest/testData/ppngSample.png")));

	auto moviePng = new QMovie("C:/Users/felix/Programming/QtProjects/ApngProject/PluginTest/testData/pngSample.png");
	moviePng->start();
	this->ui->labelMoviePng->setMovie(moviePng);
	auto movieApng = new QMovie("C:/Users/felix/Programming/QtProjects/ApngProject/PluginTest/testData/apngSample.apng");
	movieApng->start();
	this->ui->labelMovieApng->setMovie(movieApng);
	auto moviePpng = new QMovie("C:/Users/felix/Programming/QtProjects/ApngProject/PluginTest/testData/ppngSample.png");
	moviePpng->start();
	this->ui->labelMoviePpng->setMovie(moviePpng);
}

Widget::~Widget()
{
	delete ui;
}
