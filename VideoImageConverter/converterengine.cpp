#include "converterengine.h"

ConverterEngine::ConverterEngine(QObject *parent) :
	QObject(parent),
	_model(new QGenericListModel<ConverterStatus>(true, this))
{}

QGenericListModel<ConverterStatus> *ConverterEngine::model() const
{
	return _model;
}

void ConverterEngine::startConversion(const QStringList &files, const QVariantHash &setup)
{
	foreach(auto file, files)
		_model->addObject(new ConverterStatus(file));
	emit showProgress();
}

void ConverterEngine::abortConversion()
{
	emit postMessage(nullptr, tr("Aborting. Please wait…"), QtWarningMsg, false);
	emit completed();
}
