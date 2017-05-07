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

#define SEPERATOR(parent) [&]() -> QAction* {\
	auto action = new QAction(parent);\
	action->setSeparator(true);\
	return action;\
}()

SetupWindow::SetupWindow(QWidget *parent) :
	QDialog(parent, Qt::Window |
			Qt::WindowCloseButtonHint |
			Qt::WindowMinMaxButtonsHint),
	ui(new Ui::SetupWindow),
	iconProvider(new QFileIconProvider()),
	mimeSelector(new VideoMimeSelector(this))
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
												mimeSelector->generateNameFilterString(),
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

	auto dir = mimeSelector->setupDirFilters(folder);
	if(!folder.isEmpty() && dir.exists()) {
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
		slider->setToolTip(tr("×%1").arg(pow(2, position)));
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
		setup.insert(QStringLiteral("speed"), pow(2, ui->targetSpeedRelativeSlider->value()));
		if(ui->outputDirectoryCheckBox->isChecked())
			setup.insert(QStringLiteral("outDir"), ui->outputDirectoryPathEdit->path());
		emit startConversion(files, setup);
		close();
	}
}
