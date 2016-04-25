#include "conversionprogressdialog.h"
#include "ui_conversionprogressdialog.h"
#include <QStatusBar>
#include <QCloseEvent>
#include <QSettings>
#include <dialogmaster.h>
#include "rammanager.h"

ConversionProgressDialog::ConversionProgressDialog(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::ConversionProgressDialog),
	canClose(false),
	isAborting(false),
#ifdef Q_OS_WIN
	taskBarButton(new QWinTaskbarButton(this)),
#endif
	fileModel(new ConvertFileModel(this)),
	loader(new VideoLoader(this)),
	transformer(new ImageTransformator(this)),
	cacher(new CachingGenerator(this)),
	assembler(new ApngAssembler(this)),
	ramBar(new QProgressBar(this)),
	ramLabel(new QLabel(this)),
	ramUsageTimer(new QTimer(this))
{
	ui->setupUi(this);
	this->ui->tabWidget->addTab(this->ui->progressTreeView,
								tr("File Status"));
	this->ui->tabWidget->addTab(this->ui->logListWidget,
								tr("Error-Log"));
	this->ui->progressTreeView->setModel(this->fileModel);

	//statusbar
	this->ramBar->setTextVisible(false);
	this->ramBar->setRange(0, 4096);
#ifdef Q_OS_WIN
	this->ramBar->setFixedSize(110, 16);
#endif
	this->statusBar()->addPermanentWidget(this->ramLabel);
	this->statusBar()->addPermanentWidget(this->ramBar);
	connect(this->ramUsageTimer, &QTimer::timeout,
			this, &ConversionProgressDialog::updateRamUsage);
	this->ramUsageTimer->start(500);

	//loader
	connect(this->loader, &VideoLoader::progressUpdate,
			this->ui->conversionProgressBar, &QProgressBar::setValue);
	connect(this->loader, &VideoLoader::showMessage,
			this, &ConversionProgressDialog::postMessage);
	//transformer
	connect(this->transformer, &ImageTransformator::progressUpdate,
			this->ui->transformationProgressBar, &QProgressBar::setValue);
	connect(this->transformer, &ImageTransformator::showMessage,
			this, &ConversionProgressDialog::postMessage);
	//cacher
	connect(this->cacher, &CachingGenerator::progressUpdate,
			this->ui->cachingProgressBar, &QProgressBar::setValue);
	connect(this->cacher, &CachingGenerator::showMessage,
			this, &ConversionProgressDialog::postMessage);
	//assembler
	connect(this->assembler, &ApngAssembler::progressUpdate,
			this->ui->savingProgressBar, &QProgressBar::setValue);
	connect(this->assembler, &ApngAssembler::showMessage,
			this, &ConversionProgressDialog::postMessage);

	//conversion pipe
	this->loader->pipeTo(this->transformer);
	this->transformer->pipeTo(this->cacher);
	this->cacher->pipeTo(this->assembler);
	connect(this->loader, &VideoLoader::chainFinished,
			this, &ConversionProgressDialog::lastFinished);

	//settings stuff
	QSettings settings;
	settings.beginGroup(QStringLiteral("progressWindow"));
	this->restoreGeometry(settings.value(QStringLiteral("geom")).toByteArray());
	this->restoreState(settings.value(QStringLiteral("state")).toByteArray());
	this->ui->progressTreeView->header()->restoreState(settings.value(QStringLiteral("header")).toByteArray());
	settings.endGroup();
}

ConversionProgressDialog::~ConversionProgressDialog()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("progressWindow"));
	settings.setValue(QStringLiteral("geom"), this->saveGeometry());
	settings.setValue(QStringLiteral("state"), this->saveState());
	settings.setValue(QStringLiteral("header"), this->ui->progressTreeView->header()->saveState());
	settings.endGroup();

	delete ui;
}

void ConversionProgressDialog::startConversion(const QStringList &files, QList<ConverterStream::SetupInfo*> setup)
{
	foreach(auto file, files)
		this->fileModel->addItem(new ConvertFileInfo(file));
	this->ui->conversionProgressBar->setMaximum(files.size());
	this->ui->transformationProgressBar->setMaximum(files.size());
	this->ui->cachingProgressBar->setMaximum(files.size());
	this->ui->savingProgressBar->setMaximum(files.size());
	this->show();
	this->loader->startConversion(this->fileModel->allItems(), setup);
}

#ifdef Q_OS_WIN
void ConversionProgressDialog::showEvent(QShowEvent *event)
{
	this->QMainWindow::showEvent(event);
	if(!this->taskBarButton->window()) {
		auto parent = this->parentWidget();
		if(parent) {
			this->taskBarButton->setWindow(parent->windowHandle());
			auto bar = this->taskBarButton->progress();
			bar->setRange(0, this->fileModel->allItems().size());
			bar->show();
			connect(this->cacher, &VideoLoader::progressUpdate,
					bar, &QWinTaskbarProgress::setValue);
		}
	}
}
#endif

void ConversionProgressDialog::closeEvent(QCloseEvent *event)
{
	event->ignore();
	if(this->canClose)
		qApp->quit();
	else if(!this->isAborting) {
		bool checked = false;
		DialogMaster::MessageBoxInfo config = DialogMaster::createQuestion(tr("Do you realy want to cancel the conversion? All movies that "
																			  "have not been completed will be discarded."),
																		   this);
		config.title = tr("Cancel conversion?");
		config.checked = &checked;
		config.checkString = tr("Delete already completed conversions");
		if(DialogMaster::msgBox(config) == QMessageBox::Yes) {
			this->isAborting = true;
			this->loader->abortChain();
		}
	}
}

void ConversionProgressDialog::updateRamUsage()
{
	auto size = RamManager::fetchMegaBytesUsage();
	this->ramLabel->setText(tr("RAM-usage: %L1 MB").arg(size));
	if(size > 4096)
		this->ramBar->setRange(0, 0);
	else {
		this->ramBar->setRange(0, 4096);
		this->ramBar->setValue((int)size);
	}
}

void ConversionProgressDialog::postMessage(ConvertFileInfo *info, QString text, const QMessageBox::Icon &icon, bool updateInfo)
{
	if(info) {
		if(updateInfo)
			info->setResultText(text);
		text.prepend(tr("File \"%1\": ").arg(info->filename()));
	}

	if(icon ==  QMessageBox::Warning ||
	   icon == QMessageBox::Critical)
		this->statusBar()->showMessage(text);

	auto sp = QStyle::SP_MessageBoxInformation;
	switch (icon) {
	case QMessageBox::Warning:
		sp = QStyle::SP_MessageBoxWarning;
#ifdef Q_OS_WIN
		if(!this->taskBarButton->progress()->isStopped())
			this->taskBarButton->progress()->pause();
#endif
		break;
	case QMessageBox::Critical:
		sp = QStyle::SP_MessageBoxCritical;
#ifdef Q_OS_WIN
		this->taskBarButton->progress()->stop();
#endif
		break;
	case QMessageBox::Question:
		sp = QStyle::SP_MessageBoxQuestion;
		break;
	default:
		break;
	}
	auto item = new QListWidgetItem(this->style()->standardIcon(sp),
									text,
									this->ui->logListWidget);
	item->setToolTip(text);

	if(icon == QMessageBox::Critical)
		DialogMaster::critical(this, text, tr("Critical Error occured!"));
}

void ConversionProgressDialog::lastFinished()
{
	this->canClose = true;
	if(this->isAborting) {
		DialogMaster::information(this, tr("Abort completed!"));
	} else {
		bool openAll = false;
		auto config = DialogMaster::createInformation(QString(), this);
		config.title = tr("Conversion completed!");
		config.checked = &openAll;
		config.checkString = tr("Open all target folders");
		QStringList errorFiles;
		foreach(auto item, this->fileModel->allItems()) {
			if(item->status() == ConvertFileInfo::Error) {
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

		DialogMaster::msgBox(config);
		//TODO openAll
	}
}
