#ifndef CONVERTERSTATUS_H
#define CONVERTERSTATUS_H

#include <QObject>
#include <QVariant>
#include <QIcon>

class ConverterStatus : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QString filename READ filename CONSTANT FINAL)

	Q_PROPERTY(Status status READ status WRITE updateStatus NOTIFY statusChanged)
	Q_PROPERTY(QIcon statusIcon READ statusIcon NOTIFY statusChanged)
	Q_PROPERTY(QString statusText READ statusText NOTIFY statusChanged)

	Q_PROPERTY(double progress READ progress WRITE setProgress RESET resetProgress NOTIFY progressChanged)
	Q_PROPERTY(QIcon progressIcon READ progressIcon NOTIFY progressChanged)
	Q_PROPERTY(QString progressText READ progressText NOTIFY progressChanged)

	Q_PROPERTY(QString resultText READ resultText WRITE setResultText NOTIFY resultTextChanged)

public:
	enum Status {
		Waiting,
		Converting,
		Converted,
		Transforming,
		Transformed,
		Saving,
		Success,
		Error
	};
	Q_ENUM(Status)

	static QIcon iconForStatus(Status status);
	static QString textForStatus(Status status);

	explicit ConverterStatus(const QString &filename = QString(), QObject *parent = nullptr);

	bool isValid() const;

	QString filename() const;
	Status status() const;
	QIcon statusIcon() const;
	QString statusText() const;
	double progress() const;
	QIcon progressIcon() const;
	QString progressText() const;
	QString resultText() const;

public slots:
	void updateStatus(ConverterStatus::Status status);
	void setProgress(double progress);
	void resetProgress();
	void setProgressBaseText(QString progressText);
	void setResultText(QString resultText);

signals:
	void statusChanged();
	void progressChanged();
	void resultTextChanged();

private:
	QString _filename;
	Status _status;
	double _progress;
	QString _progressBaseText;
	QString _resultText;
};

#endif // CONVERTERSTATUS_H
