#include "converterstatus.h"

#include <QFile>

QIcon ConverterStatus::iconForStatus(Status status)
{
	switch (status) {
	case Waiting:
		return QIcon(QStringLiteral(":/icons/status/waiting.ico"));
	case Converting:
		return QIcon(QStringLiteral(":/icons/status/converting.ico"));
	case Converted:
		return QIcon(QStringLiteral(":/icons/status/converted.ico"));
	case Transforming:
		return QIcon(QStringLiteral(":/icons/status/transforming.ico"));
	case Transformed:
		return QIcon(QStringLiteral(":/icons/status/transformed.ico"));
	case Caching:
		return QIcon(QStringLiteral(":/icons/status/caching.ico"));
	case Cached:
		return QIcon(QStringLiteral(":/icons/status/cached.ico"));
	case Saving:
		return QIcon(QStringLiteral(":/icons/status/saving.ico"));
	case Success:
		return QIcon(QStringLiteral(":/icons/status/success.ico"));
	case Error:
		return QIcon(QStringLiteral(":/icons/status/error.ico"));
	default:
		Q_UNREACHABLE();
		return QIcon();
	}
}

QString ConverterStatus::textForStatus(Status status)
{
	switch (status) {
	case Waiting:
		return tr("Waiting…");
	case Converting:
		return tr("Converting…");
	case Converted:
		return tr("Converted! Waiting for transformation…");
	case Transforming:
		return tr("Transforming…");
	case Transformed:
		return tr("Transformed! Waiting for caching…");
	case Caching:
		return tr("Caching frames…");
	case Cached:
		return tr("Frames cached! Waiting for saving…");
	case Saving:
		return tr("Saving file…");
	case Success:
		return tr("Successful!");
	case Error:
		return tr("Error!");
	default:
		Q_UNREACHABLE();
		return QString();
	}
}

ConverterStatus::ConverterStatus(const QString &filename, QObject *parent) :
	QObject(parent),
	_filename(filename),
	_status(Waiting),
	_progress(-1),
	_progressBaseText(),
	_resultText()
{}

bool ConverterStatus::isValid() const
{
	return QFile::exists(_filename);
}

QString ConverterStatus::filename() const
{
	return _filename;
}

ConverterStatus::Status ConverterStatus::status() const
{
	return _status;
}

QIcon ConverterStatus::statusIcon() const
{
	return iconForStatus(status());
}

QString ConverterStatus::statusText() const
{
	return textForStatus(status());
}

double ConverterStatus::progress() const
{
	return _progress;
}

QIcon ConverterStatus::progressIcon() const
{
	return QIcon();//TODO piedrawer
}

QString ConverterStatus::progressText() const
{
	return _progress >= 0.0 ?
				_progressBaseText.arg((int)(_progress * 100)) :
				_progressBaseText;
}

QString ConverterStatus::resultText() const
{
	return _resultText;
}

void ConverterStatus::updateStatus(ConverterStatus::Status status)
{
	if (_status == status)
		return;

	_status = status;
	emit statusChanged();
}

void ConverterStatus::setProgress(double progress)
{
	if (_progress == progress)
		return;

	_progress = progress;
	emit progressChanged();
}

void ConverterStatus::resetProgress()
{
	setProgress(-1.0);
}

void ConverterStatus::setProgressBaseText(QString progressText)
{
	if (_progressBaseText == progressText)
		return;

	_progressBaseText = progressText;
	emit progressChanged();
}

void ConverterStatus::setResultText(QString resultText)
{
	if (_resultText == resultText)
		return;

	_resultText = resultText;
	emit resultTextChanged();
}
