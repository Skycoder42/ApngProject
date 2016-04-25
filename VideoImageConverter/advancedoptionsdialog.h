#ifndef ADVANCEDOPTIONSDIALOG_H
#define ADVANCEDOPTIONSDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QVariant>

namespace Ui {
	class AdvancedOptionsDialog;
}

class AdvancedOptionsDialog : public QDialog
{
	Q_OBJECT

public:
	enum AdvancedSettings {
		CachingDirectory
	};
	Q_ENUM(AdvancedSettings)

	explicit AdvancedOptionsDialog(QWidget *parent = Q_NULLPTR);
	~AdvancedOptionsDialog();

	static void showAdvancedSettings(QWidget *parent = Q_NULLPTR);
	static QVariant getValue(AdvancedSettings settings, const QVariant &defaultValue = QVariant());

private slots:
	void on_cacheDirectoryResetButton_clicked();
	void on_buttonBox_clicked(QAbstractButton *button);

	void save();

private:
	Ui::AdvancedOptionsDialog *ui;
};

#endif // ADVANCEDOPTIONSDIALOG_H
