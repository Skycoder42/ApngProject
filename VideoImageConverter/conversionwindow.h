#ifndef CONVERSIONWINDOW_H
#define CONVERSIONWINDOW_H

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
	class ConversionWindow;
}

class ConversionWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConversionWindow(QGenericListModel<ConverterStatus> *converterModel, QWidget *parent = nullptr);
	~ConversionWindow();

protected:
	void showEvent(QShowEvent *event) override;
	void closeEvent(QCloseEvent *event) final;

private slots:
	void updateRamUsage();
	void postMessage(ConverterStatus *info, QString text, const QtMsgType &msgType, bool updateInfo);
	void lastFinished();

private:
	Ui::ConversionWindow *ui;
	bool canClose;
	bool isAborting;
#ifdef Q_OS_WIN
	QWinTaskbarButton *taskBarButton;
#endif

	QGenericListModel<ConverterStatus> *converterModel;
	QObjectProxyModel *proxyModel;

	QProgressBar *ramBar;
	QLabel *ramLabel;
	QTimer *ramUsageTimer;
};

#endif // CONVERSIONWINDOW_H
