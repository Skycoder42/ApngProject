#ifndef CONVERSIONPROGRESSDIALOG_H
#define CONVERSIONPROGRESSDIALOG_H

#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif
#include "convertfilemodel.h"
#include "videoloader.h"
#include "imagetransformator.h"
#include "cachinggenerator.h"
#include "apngassembler.h"

namespace Ui {
	class ConversionProgressDialog;
}

class ConversionProgressDialog : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConversionProgressDialog(QWidget *parent = 0);
	~ConversionProgressDialog();

public slots:
	void startConversion(const QStringList &files, QList<ConverterStream::SetupInfo *> setup);

protected:
#ifdef Q_OS_WIN
	void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
#endif
	void closeEvent(QCloseEvent *event) Q_DECL_FINAL;

private slots:
	void updateRamUsage();
	void postMessage(ConvertFileInfo *info, QString text, const QMessageBox::Icon &icon);
	void lastFinished();

private:
	Ui::ConversionProgressDialog *ui;
	bool canClose;
	bool isAborting;
#ifdef Q_OS_WIN
	QWinTaskbarButton *taskBarButton;
#endif

	ConvertFileModel *fileModel;
	VideoLoader *loader;
	ImageTransformator *transformer;
	CachingGenerator *cacher;
	ApngAssembler *assembler;

	QProgressBar *ramBar;
	QLabel *ramLabel;
	QTimer *ramUsageTimer;
};

#endif // CONVERSIONPROGRESSDIALOG_H
