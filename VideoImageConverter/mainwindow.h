#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QSlider>
#include <QFileIconProvider>
#include <QScopedPointer>
#include "conversionprogressdialog.h"

namespace Ui {
	class MainWindow;
}

class MainWindow : public QDialog
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_actionAdd_Files_triggered();
	void on_actionAdd_Folder_triggered();
	void on_actionAdd_Folder_Recursive_triggered();
	void on_actionRemove_Selected_Files_triggered();

	void on_targetSpeedRelativeSlider_sliderMoved(int position);
	void on_targetSpeedRelativeSlider_sliderPressed();
	void updateSliderTooltip(QSlider *slider, bool updatePositon = false, int position = 0);

	void on_startConversionButton_clicked();
	void on_advancedOptionsButton_clicked();

private:
	static QStringList supportedFormatsList;
	static QString supportedFormatsString;
	static QHash<int, double> speedMap;

	Ui::MainWindow *ui;
	QScopedPointer<QFileIconProvider> iconProvider;

	ConversionProgressDialog *mainConverter;

	void getFolderFiles(bool recursive);
	Q_INVOKABLE void addFileItem(const QString &file);
	Q_INVOKABLE void showFolderResult(int count);
};

#endif // MAINWINDOW_H
