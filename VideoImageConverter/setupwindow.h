#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <QDialog>
#include <QSlider>
#include <QFileIconProvider>
#include <QScopedPointer>
#include "converterstream.h"

namespace Ui {
	class SetupWindow;
}

class SetupWindow : public QDialog
{
	Q_OBJECT

public:
	explicit SetupWindow(QWidget *parent = nullptr);
	~SetupWindow();

signals:
	void startConversion(const QStringList &files, QVariantHash setup);

private slots:
	void on_actionAdd_Files_triggered();
	void on_actionAdd_Folder_triggered();
	void on_actionAdd_Folder_Recursive_triggered();
	void on_actionRemove_Selected_Files_triggered();

	void on_targetSpeedRelativeSlider_sliderMoved(int position);
	void on_targetSpeedRelativeSlider_sliderPressed();
	void updateSliderTooltip(QSlider *slider, bool updatePositon = false, int position = 0);

	void on_startConversionButton_clicked();

private:
	static QStringList supportedFormatsList;
	static QString supportedFormatsString;
	static QHash<int, double> speedMap;

	Ui::SetupWindow *ui;
	QScopedPointer<QFileIconProvider> iconProvider;

	void getFolderFiles(bool recursive);
	Q_INVOKABLE void addFileItem(const QString &file);
	Q_INVOKABLE void showFolderResult(int count);
};

#endif // SETUPWINDOW_H
