#include "setupwindow.h"
#include "ui_setupwindow.h"
#include <QMenu>
#include <QStandardPaths>
#include <QDirIterator>
#include <QToolTip>
#include <dialogmaster.h>
#include <QProgressDialog>
#include <QSettings>
#include <QtConcurrent>

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

QStringList SetupWindow::supportedFormatsList;
QString SetupWindow::supportedFormatsString;
QHash<int, double> SetupWindow::speedMap = {
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

SetupWindow::SetupWindow(QWidget *parent) :
	QDialog(parent, Qt::Window |
			Qt::WindowCloseButtonHint |
			Qt::WindowMinMaxButtonsHint),
	ui(new Ui::SetupWindow),
	iconProvider(new QFileIconProvider())
{
	ui->setupUi(this);
	updateSliderTooltip(ui->targetSpeedRelativeSlider, true);

	//toolbutton actions
	ui->addButton->addActions({
								  ui->actionAdd_Files,
								  ui->actionAdd_Folder,
								  ui->actionAdd_Folder_Recursive
							  });
	ui->addButton->setDefaultAction(ui->actionAdd_Files);
	ui->removeButton->setDefaultAction(ui->actionRemove_Selected_Files);

	//treeview actions
	ui->fileListWidget->addActions({
									   ui->actionAdd_Files,
									   ui->actionAdd_Folder,
									   ui->actionAdd_Folder_Recursive,
									   SEPERATOR(ui->fileListWidget),
									   ui->actionRemove_Selected_Files
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
			{ "Matroska/WebM", "*.mkv", "*.webm" },
			{ "Ogg",  "*.ogm", "*.ogv" },
			{ "Flash Video", "*.flv", "*.f4v" },
			{ "Windows Media", "*.wmv" },
			{ "RealMedia", "*.rmvb", "*.rm" },
			{ "Digital Video", "*.dv" },
			{ "All", "*" }
		};

		foreach(auto lst, supportedFormats) {
			auto str = lst.takeFirst();
			supportedFormatsString.append(SetupWindow::tr("%1 Files (%2);;")
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
	restoreGeometry(settings.value(QStringLiteral("geom")).toByteArray());
	ui->splitter->restoreState(settings.value(QStringLiteral("splitter")).toByteArray());
	switch(settings.value(QStringLiteral("defaultOpenAction")).toInt()) {
	case 1:
		ui->addButton->setDefaultAction(ui->actionAdd_Folder);
		break;
	case 2:
		ui->addButton->setDefaultAction(ui->actionAdd_Folder_Recursive);
		break;
	default:
		break;
	}
	settings.endGroup();
	ui->cacheDirectoryPathEdit->setPath(settings.value(QStringLiteral("cache")).toString());
}

SetupWindow::~SetupWindow()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("setupWindow"));
	settings.setValue(QStringLiteral("geom"), saveGeometry());
	settings.setValue(QStringLiteral("splitter"), ui->splitter->saveState());
	if(ui->addButton->defaultAction() == ui->actionAdd_Files)
		settings.setValue(QStringLiteral("defaultOpenAction"), 0);
	else if(ui->addButton->defaultAction() == ui->actionAdd_Folder)
		settings.setValue(QStringLiteral("defaultOpenAction"), 1);
	else if(ui->addButton->defaultAction() == ui->actionAdd_Folder_Recursive)
		settings.setValue(QStringLiteral("defaultOpenAction"), 2);
	settings.endGroup();
	settings.setValue(QStringLiteral("cache"), ui->cacheDirectoryPathEdit->path());

	delete ui;
}

void SetupWindow::on_actionAdd_Files_triggered()
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
	foreach (auto file, files)
		addFileItem(file);
}

void SetupWindow::on_actionAdd_Folder_triggered()
{
	getFolderFiles(false);
}

void SetupWindow::on_actionAdd_Folder_Recursive_triggered()
{
	getFolderFiles(true);
}

void SetupWindow::getFolderFiles(bool recursive)
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

void SetupWindow::addFileItem(const QString &file)
{
	new QListWidgetItem(iconProvider->icon(file),
						file,
						ui->fileListWidget);
}

void SetupWindow::showFolderResult(int count)
{
	if(count == 0)
		DialogMaster::warning(this, tr("No matiching files found!"));
	else
		DialogMaster::information(this, tr("Found %L1 files that could be video files!").arg(count));
}

void SetupWindow::on_actionRemove_Selected_Files_triggered()
{
	qDeleteAll(ui->fileListWidget->selectedItems());
}

void SetupWindow::on_targetSpeedRelativeSlider_sliderMoved(int position)
{
	updateSliderTooltip(ui->targetSpeedRelativeSlider, true, position);
}

void SetupWindow::on_targetSpeedRelativeSlider_sliderPressed()
{
	updateSliderTooltip(ui->targetSpeedRelativeSlider);
}

void SetupWindow::updateSliderTooltip(QSlider *slider, bool updatePositon, int position)
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

void SetupWindow::on_startConversionButton_clicked()
{
	QStringList files;
	for(int i = 0, max = ui->fileListWidget->count(); i < max; ++i)
		files += ui->fileListWidget->item(i)->text();
	if(!files.isEmpty()) {
		QVariantHash setup;
		setup.insert(QStringLiteral("size"), ui->targetSizeSpinBox->value());
		setup.insert(QStringLiteral("frameRate"), ui->frameRateDoubleSpinBox->value());
		setup.insert(QStringLiteral("speed"), speedMap[ui->targetSpeedRelativeSlider->value()]);
		auto path = ui->cacheDirectoryPathEdit->path();
		if(!path.isEmpty())
			setup.insert(QStringLiteral("cacheDir"), path);
		if(ui->outputDirectoryCheckBox->isChecked())
			setup.insert(QStringLiteral("outDir"), ui->outputDirectoryPathEdit->path());
		emit startConversion(files, setup);
		close();
	}
}
