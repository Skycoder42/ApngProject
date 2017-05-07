#ifndef CONVERSIONPROGRESSDIALOG_H
#define CONVERSIONPROGRESSDIALOG_H

#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <qobjectlistmodel.h>
#include <qobjectproxymodel.h>
#ifdef Q_OS_WIN
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif
#include "converterstatus.h"

namespace Ui {
	class ConversionProgressDialog;
}

class ConversionProgressDialog : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConversionProgressDialog(QWidget *parent = nullptr);
	~ConversionProgressDialog();

public slots:
	void startConversion(const QStringList &files, QVariantHash setup);

protected:
#ifdef Q_OS_WIN
	void showEvent(QShowEvent *event) override;
#endif
	void closeEvent(QCloseEvent *event) final;

private slots:
	void updateRamUsage();
	void postMessage(ConverterStatus *info, QString text, const QtMsgType &msgType, bool updateInfo);
	void lastFinished();

private:
	Ui::ConversionProgressDialog *ui;
	bool canClose;
	bool isAborting;
#ifdef Q_OS_WIN
	QWinTaskbarButton *taskBarButton;
#endif

	QGenericListModel<ConverterStatus> *fileModel;
	QObjectProxyModel *proxyModel;

	QProgressBar *ramBar;
	QLabel *ramLabel;
	QTimer *ramUsageTimer;
};

#endif // CONVERSIONPROGRESSDIALOG_H
