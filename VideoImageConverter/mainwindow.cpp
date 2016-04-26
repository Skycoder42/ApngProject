#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <QStandardPaths>
#include <QDirIterator>
#include <QToolTip>
#include <dialogmaster.h>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>
#include "advancedoptionsdialog.h"

#include "convertfilemodel.h"
#include "videoloader.h"
#include "imagetransformator.h"
#include "cachinggenerator.h"
#include "apngassembler.h"

#define SEPERATOR(parent) [&]() -> QAction* {\
	auto action = new QAction(parent);\
	action->setSeparator(true);\
	return action;\
}()

QStringList MainWindow::supportedFormatsList;
QString MainWindow::supportedFormatsString;
QHash<int, double> MainWindow::speedMap = {
	{-5, 1./32.},
	{-4, 1./16.},
	{-3, 1./8.},
	{-2, 1./4.},
	{-1, 1./2.},
	{0, 1.},
	{1, 2.},
	{2, 4.},
	{3, 8.},
	{4, 16.},
	{5, 32.},
};

MainWindow::MainWindow(QWidget *parent) :
	QDialog(parent, Qt::Window |
					Qt::WindowCloseButtonHint |
					Qt::WindowMinMaxButtonsHint),
	ui(new Ui::MainWindow),
	iconProvider(new QFileIconProvider())
{
	this->ui->setupUi(this);
	this->updateSliderTooltip(this->ui->targetSpeedRelativeSlider, true);

	//toolbutton actions
	this->ui->addButton->addActions({
										this->ui->actionAdd_Files,
										this->ui->actionAdd_Folder,
										this->ui->actionAdd_Folder_Recursive
									});
	this->ui->addButton->setDefaultAction(this->ui->actionAdd_Files);
	this->ui->removeButton->setDefaultAction(this->ui->actionRemove_Selected_Files);

	//treeview actions
	this->ui->fileListWidget->addActions({
											 this->ui->actionAdd_Files,
											 this->ui->actionAdd_Folder,
											 this->ui->actionAdd_Folder_Recursive,
											 SEPERATOR(this->ui->fileListWidget),
											 this->ui->actionRemove_Selected_Files
									   });

	//formats
	static bool once = true;
	if(once) {
		once = false;
		QList<QStringList> supportedFormats = {
			{ "AVI", "*.avi", "*.divx", "*.amv" },
			{ "MPEG-PS", "*.mpg", "*.mpeg", "*.mpe", "*.m1v", "*.m2v", "*.mpv2", "*.mp2v", "*.m2p", "*.vob", "*.evo", "*.mod" },
			{ "MPEG-TS", "*.ts", "*.m2ts", "*.m2t", "*.mts", "*.pva", "*.tp", "*.tpr" },
			{ "MP4", "*.mp4", "*.m4v", "*.mp4v", "*.mpv4", "*.hdmov" },
			{ "MOV", "*.mov" },
			{ "3GP", "*.3gp", "*.3gpp", "*.3g2", "*.3gp2" },
			{ "Matroska / WebM", "*.mkv", "*.webm" },
			{ "Ogg",  "*.ogm", "*.ogv" },
			{ "Flash Video", "*.flv", "*.f4v" },
			{ "Windows Media", "*.wmv" },
			{ "RealMedia", "*.rmvb", "*.rm" },
			{ "Digital Video", "*.dv" },
			{ "All", "*" }
		};

		foreach(auto lst, supportedFormats) {
			auto str = lst.takeFirst();
			supportedFormatsString.append(MainWindow::tr("%1 Files (%2);;")
										  .arg(str)
										  .arg(lst.join(QLatin1Char(' '))));
			supportedFormatsList.append(lst);
		}

		supportedFormatsString.resize(supportedFormatsString.size() - 2);
		supportedFormatsList.removeAll(QStringLiteral("*"));
	}

	//settings restore
	QSettings settings;
	settings.beginGroup(QStringLiteral("setupWindow"));
	this->restoreGeometry(settings.value(QStringLiteral("geom")).toByteArray());
	this->ui->splitter->restoreState(settings.value(QStringLiteral("splitter")).toByteArray());
	switch(settings.value(QStringLiteral("defaultOpenAction")).toInt()) {
	case 1:
		this->ui->addButton->setDefaultAction(this->ui->actionAdd_Folder);
		break;
	case 2:
		this->ui->addButton->setDefaultAction(this->ui->actionAdd_Folder_Recursive);
		break;
	default:
		break;
	}
	settings.endGroup();
}

MainWindow::~MainWindow()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("setupWindow"));
	settings.setValue(QStringLiteral("geom"), this->saveGeometry());
	settings.setValue(QStringLiteral("splitter"), this->ui->splitter->saveState());
	if(this->ui->addButton->defaultAction() == this->ui->actionAdd_Files)
		settings.setValue(QStringLiteral("defaultOpenAction"), 0);
	else if(this->ui->addButton->defaultAction() == this->ui->actionAdd_Folder)
		settings.setValue(QStringLiteral("defaultOpenAction"), 1);
	else if(this->ui->addButton->defaultAction() == this->ui->actionAdd_Folder_Recursive)
		settings.setValue(QStringLiteral("defaultOpenAction"), 2);
	settings.endGroup();

	delete this->ui;
}

void MainWindow::on_actionAdd_Files_triggered()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("setupWindow"));
	QString selectedFilter = settings.value(QStringLiteral("defaultFilter")).toString();
	auto files = DialogMaster::getOpenFileNames(this,
												tr("Open video files"),
												QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
												supportedFormatsString,
												&selectedFilter);
	settings.setValue(QStringLiteral("defaultFilter"), selectedFilter);
	settings.endGroup();
	foreach (auto file, files) {
		new QListWidgetItem(this->iconProvider->icon(file),
							file,
							this->ui->fileListWidget);
	}
}

void MainWindow::on_actionAdd_Folder_triggered()
{
	this->getFolderFiles(false);
}

void MainWindow::on_actionAdd_Folder_Recursive_triggered()
{
	this->getFolderFiles(true);
}

void MainWindow::getFolderFiles(bool recursive)
{
	auto folder = DialogMaster::getExistingDirectory(this,
													 tr("Open video folder"),
													 QStandardPaths::writableLocation(QStandardPaths::MoviesLocation));

	QDir dir(folder);
	if(!folder.isEmpty() && dir.exists()) {
		dir.setNameFilters(supportedFormatsList);
		dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
		auto dialog = DialogMaster::createProgress(this, tr("Scanning directory…"));
		QtConcurrent::run([=](){
			int count = 0;
			QDirIterator dirIterator(dir, recursive ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags);
			while(dirIterator.hasNext()) {
				if(dialog->wasCanceled())
					break;
				QMetaObject::invokeMethod(this, "addFileItem", Qt::QueuedConnection,
										  Q_ARG(QString, dirIterator.next()));
				count++;
			}
			QMetaObject::invokeMethod(dialog, "cancel", Qt::QueuedConnection);
			QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
			QMetaObject::invokeMethod(this, "showFolderResult", Qt::QueuedConnection,
									  Q_ARG(int, count));
		});
	}
}

void MainWindow::addFileItem(const QString &file)
{
	new QListWidgetItem(this->iconProvider->icon(file),
						file,
						this->ui->fileListWidget);
}

void MainWindow::showFolderResult(int count)
{
	if(count == 0)
		DialogMaster::warning(this, tr("No matiching files found!"));
	else
		DialogMaster::information(this, tr("Found %L1 files that could be video files!").arg(count));
}

void MainWindow::on_actionRemove_Selected_Files_triggered()
{
	qDeleteAll(this->ui->fileListWidget->selectedItems());
}

void MainWindow::on_targetSpeedRelativeSlider_sliderMoved(int position)
{
	this->updateSliderTooltip(this->ui->targetSpeedRelativeSlider, true, position);
}

void MainWindow::on_targetSpeedRelativeSlider_sliderPressed()
{
	this->updateSliderTooltip(this->ui->targetSpeedRelativeSlider);
}

void MainWindow::updateSliderTooltip(QSlider *slider, bool updatePositon, int position)
{
	if(updatePositon)
		slider->setToolTip(tr("×%1").arg(speedMap[position]));
	QPoint pos;
	pos.setY(slider->height() / 2);
	pos.setX(QStyle::sliderPositionFromValue(slider->minimum(),
											 slider->maximum(),
											 slider->value(),
											 slider->width()) - 10);
	QToolTip::showText(slider->mapToGlobal(pos),
					   slider->toolTip(),
					   slider);
}

void MainWindow::on_startConversionButton_clicked()
{
	QStringList files;
	for(int i = 0, max = this->ui->fileListWidget->count(); i < max; ++i)
		files += this->ui->fileListWidget->item(i)->text();
	if(!files.isEmpty()) {
		QList<ConverterStream::SetupInfo*> setup;
		setup.append(Q_NULLPTR);
		setup.append(new ImageTransformator::TransformatorSetup(this->ui->targetSizeSpinBox->value(),
																this->ui->frameRateDoubleSpinBox->value(),
																speedMap[this->ui->targetSpeedRelativeSlider->value()]));
		setup.append(new CachingGenerator::CachingSetup(AdvancedOptionsDialog::getValue(AdvancedOptionsDialog::CachingDirectory).toString()));
		ApngAssembler::AssemblerSetup::CompressionType type;
		if(this->ui->mode7zipButton->isChecked())
			type = ApngAssembler::AssemblerSetup::Mode7zip;
		else if(this->ui->modeZlibButton->isChecked())
			type = ApngAssembler::AssemblerSetup::ModeZlib;
		else if(this->ui->modeZopfliButton->isChecked())
			type = ApngAssembler::AssemblerSetup::ModeZopfli;
		else
			type = ApngAssembler::AssemblerSetup::Mode7zip;
		setup.append(new ApngAssembler::AssemblerSetup(this->ui->outputDirectoryCheckBox->isChecked() ?
														   this->ui->outputDirectoryPathEdit->path() :
														   QString(),
													   this->ui->loopCountSpinBox->value(),
													   this->ui->keepPaletteCheckBox->isChecked(),
													   this->ui->keepColorTypeCheckBox->isChecked(),
													   type,
													   this->ui->compressionIterationsSpinBox->value()));
		emit startConversion(files, setup);
		this->close();
	}
}

void MainWindow::on_advancedOptionsButton_clicked()
{
	AdvancedOptionsDialog::showAdvancedSettings(this);
}
