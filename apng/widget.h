#ifndef WIDGET_H
#define WIDGET_H

#include "apngreader.h"

#include <QWidget>
#include <QDir>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
	Q_OBJECT

public:
	explicit Widget(QWidget *parent = nullptr);
	~Widget();

private:
	Ui::Widget *ui;
	ApngReader *_reader;
	QDir _dir;
};

#endif // WIDGET_H
