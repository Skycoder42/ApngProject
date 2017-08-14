#ifndef CONVERSIONWINDOW_H
#define CONVERSIONWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <qobjectlistmodel.h>
#include <qobjectproxymodel.h>
#include <qtaskbarcontrol.h>
#include <QSet>
#include "converterstatus.h"
#include "converterengine.h"

namespace Ui {
	class ConversionWindow;
}

class ConversionWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ConversionWindow(ConverterEngine *engine, QWidget *parent = nullptr);
	~ConversionWindow();

public slots:
	void open(const QStringList &streamNames, QSet<QString> folders);

protected:
	void closeEvent(QCloseEvent *event) final;

private slots:
	void updateRamUsage();
	void postMessage(ConverterStatus *info, QString text, const QtMsgType &msgType, bool updateInfo);
	void updateProgress(int index, int progress);
	void lastFinished();

private:
	ConverterEngine *engine;

	Ui::ConversionWindow *ui;
	QTaskbarControl *taskbar;
	int progressMax;
	int progressCurrent;

	bool canClose;
	bool isAborting;

	QGenericListModel<ConverterStatus> *converterModel;
	QObjectProxyModel *proxyModel;
	QHash<int, QProgressBar*> streamBars;
	QSet<QString> folders;

	QProgressBar *ramBar;
	QLabel *ramLabel;
	QTimer *ramUsageTimer;
};

#endif // CONVERSIONWINDOW_H
