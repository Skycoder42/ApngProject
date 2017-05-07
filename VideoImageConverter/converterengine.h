#ifndef CONVERTERENGINE_H
#define CONVERTERENGINE_H

#include <QObject>

class ConverterEngine : public QObject
{
	Q_OBJECT
public:
	explicit ConverterEngine(QObject *parent = 0);

signals:

public slots:
};

#endif // CONVERTERENGINE_H