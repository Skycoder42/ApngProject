#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QTimer>

static auto c = 0;

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget),
	_reader(nullptr),
	_dir("/tmp/apng_test")
{
	ui->setupUi(this);

	_dir.removeRecursively();
	_dir.mkpath(".");

	//auto file = new QFile("/run/media/sky/Data/Mine/Anime/Top/aDGwwz7_460sv.apng", this);
	//auto file = new QFile("/home/sky/Programming/QtProjects/ApngProject/PluginTest/testData/apngSample.apng", this);
	auto file = new QFile("/tmp/code_examples/clock4.png", this);
	Q_ASSERT(file->open(QIODevice::ReadOnly | QIODevice::Unbuffered));
	_reader = new ApngReader(file, this);
	Q_ASSERT(_reader->isValid());

	_reader->init();
	Q_ASSERT(_reader->isValid());

	auto t = new QTimer(this);
	t->setInterval(1000);
	t->start();

	connect(t, &QTimer::timeout, this, [this](){
		QPixmap p(_dir.absoluteFilePath(QStringLiteral("%1.png").arg(c++)));
		if(p.isNull()){
			c = 0;
			p = QPixmap(_dir.absoluteFilePath(QStringLiteral("%1.png").arg(c++)));
		}
		ui->label->setPixmap(p);
	});
}

Widget::~Widget()
{
	delete ui;
}
