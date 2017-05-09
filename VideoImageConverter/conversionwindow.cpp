#include "conversionwindow.h"
#include "ui_conversionwindow.h"
#include <QStatusBar>
#include <QCloseEvent>
#include <QSettings>
#include <dialogmaster.h>
#include "rammanager.h"

ConversionWindow::ConversionWindow(ConverterEngine *engine, QWidget *parent) :
	QMainWindow(parent),
	engine(engine),
	ui(new Ui::ConversionWindow),
	canClose(false),
	isAborting(false),
#ifdef Q_OS_WIN
	taskBarButton(new QWinTaskbarButton(this)),
#endif
	converterModel(engine->model()),
	proxyModel(new QObjectProxyModel({tr("Status"), tr("Filename"), tr("Status-Text"), tr("Progress")}, this)),
	streamBars(),
	ramBar(new QProgressBar(this)),
	ramLabel(new QLabel(this)),
	ramUsageTimer(new QTimer(this))
{
	ui->setupUi(this);
	ui->tabWidget->addTab(ui->progressTreeView,
								tr("File Status"));
	ui->tabWidget->addTab(ui->logListWidget,
								tr("Error-Log"));

	//model
	proxyModel->setSourceModel(converterModel);
	proxyModel->addMapping(0, Qt::DisplayRole, "statusText");
	proxyModel->addMapping(0, Qt::DecorationRole, "statusIcon");
	proxyModel->addMapping(1, Qt::DisplayRole, "filename");
	proxyModel->addMapping(2, Qt::DisplayRole, "resultText");
	proxyModel->addMapping(3, Qt::DisplayRole, "progressText");
	proxyModel->addMapping(3, Qt::DecorationRole, "progressIcon");
	ui->progressTreeView->setModel(proxyModel);

	//statusbar
	ramBar->setTextVisible(false);
	ramBar->setRange(0, 4096);
	ramBar->setFixedWidth(125);
	ramBar->setMaximumHeight(16);
	statusBar()->addPermanentWidget(ramLabel);
	statusBar()->addPermanentWidget(ramBar);
	connect(ramUsageTimer, &QTimer::timeout,
			this, &ConversionWindow::updateRamUsage);
	ramUsageTimer->start(500);

	//engine
	connect(engine, &ConverterEngine::showProgress,
			this, &ConversionWindow::open);
	connect(engine, &ConverterEngine::postMessage,
			this, &ConversionWindow::postMessage);
	connect(engine, &ConverterEngine::updateProgress,
			this, &ConversionWindow::updateProgress);
	connect(engine, &ConverterEngine::completed,
			this, &ConversionWindow::lastFinished);

	//settings stuff
	QSettings settings;
	settings.beginGroup(QStringLiteral("progressWindow"));
	resize(settings.value(QStringLiteral("size")).toSize());
	restoreState(settings.value(QStringLiteral("state")).toByteArray());
	ui->progressTreeView->header()->restoreState(settings.value(QStringLiteral("header")).toByteArray());
	settings.endGroup();
}

ConversionWindow::~ConversionWindow()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("progressWindow"));
	settings.setValue(QStringLiteral("size"), size());
	settings.setValue(QStringLiteral("state"), saveState());
	settings.setValue(QStringLiteral("header"), ui->progressTreeView->header()->saveState());
	settings.endGroup();

	delete ui;
}

void ConversionWindow::open(const QStringList &streamNames)
{
	auto index = 0;
	foreach(auto name, streamNames) {
		auto bar = new QProgressBar(ui->progressBox);
		bar->setMaximum(converterModel->rowCount());
		bar->setValue(0);
		ui->progressLayout->addRow(tr("%1 Progress:").arg(name), bar);
		streamBars.insert(index++, bar);
	}

	show();
	raise();
	activateWindow();
}

#ifdef Q_OS_WIN
void ConversionWindow::showEvent(QShowEvent *event)
{
	QMainWindow::showEvent(event);
	if(!taskBarButton->window()) {
		taskBarButton->setWindow(windowHandle());
		auto bar = taskBarButton->progress();
		bar->setRange(0, fileModel->allItems().size());
		bar->show();
		connect(assembler, &VideoLoader::progressUpdate,
				bar, &QWinTaskbarProgress::setValue);
	}
}
#endif

void ConversionWindow::closeEvent(QCloseEvent *event)
{
	if(canClose)
		event->accept();
	else {
		event->ignore();
		if(!isAborting) {
			if(DialogMaster::question(this,
									  tr("All movies that have not been completed will be discarded. Movies already "
										 "converted will not be deleted. If you activated original file deletion, those "
										 "will be already deleted and not restored."),
									  tr("Do you realy want to cancel the conversion?"),
									  tr("Cancel conversion?"))
			   == QMessageBox::Yes) {
				isAborting = true;
				engine->abortConversion();
			}
		}
	}
}

void ConversionWindow::updateRamUsage()
{
	auto size = RamManager::fetchMegaBytesUsage();
	ramLabel->setText(tr("RAM-usage: %L1 MB").arg(size));
	if(size > 4096)
		ramBar->setRange(0, 0);
	else {
		ramBar->setRange(0, 4096);
		ramBar->setValue((int)size);
	}
}

void ConversionWindow::postMessage(ConverterStatus *info, QString text, const QtMsgType &msgType, bool updateInfo)
{
	if(info) {
		if(updateInfo)
			info->setResultText(text);
		text.prepend(tr("File \"%1\": ").arg(info->filename()));
	}

	if(msgType ==  QtWarningMsg ||
	   msgType == QtCriticalMsg)
		statusBar()->showMessage(text);

	auto sp = QStyle::SP_MessageBoxInformation;
	switch (msgType) {
	case QtWarningMsg:
		sp = QStyle::SP_MessageBoxWarning;
#ifdef Q_OS_WIN
		if(!taskBarButton->progress()->isStopped())
			taskBarButton->progress()->pause();
#endif
		break;
	case QtCriticalMsg:
		sp = QStyle::SP_MessageBoxCritical;
#ifdef Q_OS_WIN
		taskBarButton->progress()->stop();
#endif
		break;
	default:
		break;
	}
	auto item = new QListWidgetItem(style()->standardIcon(sp),
									text,
									ui->logListWidget);
	item->setToolTip(text);
	ui->logListWidget->scrollToBottom();

	if(msgType == QtCriticalMsg)
		DialogMaster::critical(this, text, tr("Critical Error occured!"));
}

void ConversionWindow::updateProgress(int index, int progress)
{
	auto bar = streamBars.value(index, nullptr);
	if(bar)//TODO update total
		bar->setValue(progress);
}

void ConversionWindow::lastFinished()
{
	canClose = true;
	if(isAborting) {
		if(DialogMaster::information(this,
									 tr("You can now check the output and close the application."),
									 tr("Abort completed!"),
									 tr("Abort"),
									 QMessageBox::Ok | QMessageBox::Close)
		   == QMessageBox::Close) {
			QMetaObject::invokeMethod(this, "close", Qt::QueuedConnection);
		}
	} else {
		auto openAll = false;
		auto config = DialogMaster::createInformation(QString(), this);
		config.title = tr("Conversion completed!");
		config.checked = &openAll;
		config.checkString = tr("Open all target folders");
		QStringList errorFiles;
		foreach(auto item, converterModel->objects()) {
			if(item->status() == ConverterStatus::Error) {
				auto error = tr("File \"%1\", Error: %2")
							 .arg(item->filename())
							 .arg(item->resultText());
				errorFiles += error;
			}
		}
		if(errorFiles.isEmpty()) {
			config.text = tr("All files have been successfully converted. Check the error"
							 "log for warnings and errors");
		} else {
			config.icon = QMessageBox::Warning;
			config.text = tr("Some files have been successfully converted, but some files failed. "
							 "Check the details and the error log for more information");
			config.details = tr("Files that failed to convert:\n\n%1")
							 .arg(errorFiles.join(QLatin1Char('\n')));
		}

		DialogMaster::messageBox(config);
		//TODO openAll
	}
}
