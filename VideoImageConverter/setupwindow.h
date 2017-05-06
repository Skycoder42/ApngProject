#ifndef SETUPWINDOW_H
#define SETUPWINDOW_H

#include <QDialog>
#include <QSlider>
#include <QFileIconProvider>
#include <QScopedPointer>
#include <QVariantHash>
#include "videomimeselector.h"

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
	static QHash<int, double> speedMap;

	Ui::SetupWindow *ui;
	QScopedPointer<QFileIconProvider> iconProvider;
	VideoMimeSelector *mimeSelector;

	void getFolderFiles(bool recursive);
	Q_INVOKABLE void addFileItem(const QString &file);
	Q_INVOKABLE void showFolderResult(int count);
};

#endif // SETUPWINDOW_H
