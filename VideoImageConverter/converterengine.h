#ifndef CONVERTERENGINE_H
#define CONVERTERENGINE_H

#include <QObject>

class ConverterEngine : public QObject
{
	Q_OBJECT
public:
	explicit ConverterEngine(QObject *parent = nullptr);

signals:

public slots:
};

#endif // CONVERTERENGINE_H
