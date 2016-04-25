#include "advancedoptionsdialog.h"
#include "ui_advancedoptionsdialog.h"
#include <dialogmaster.h>
#include <QDir>
#include <QSettings>
#include <QMetaEnum>

#define KEY(key) QMetaEnum::fromType<AdvancedSettings>().valueToKey(key)

AdvancedOptionsDialog::AdvancedOptionsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::AdvancedOptionsDialog)
{
	ui->setupUi(this);
	DialogMaster::masterDialog(this, true);

	QSettings settings;
	settings.beginGroup(QStringLiteral("advanced"));
	this->ui->cacheDirectoryPathEdit->setPath(settings.value(KEY(CachingDirectory), QDir::tempPath()).toString());
	settings.endGroup();
}

AdvancedOptionsDialog::~AdvancedOptionsDialog()
{
	delete ui;
}

void AdvancedOptionsDialog::showAdvancedSettings(QWidget *parent)
{
	AdvancedOptionsDialog dialog(parent);
	dialog.exec();
}

QVariant AdvancedOptionsDialog::getValue(AdvancedOptionsDialog::AdvancedSettings settings, const QVariant &defaultValue)
{
	return QSettings().value(QStringLiteral("advanced/%1").arg(KEY(settings)),
							 defaultValue);
}

void AdvancedOptionsDialog::on_cacheDirectoryResetButton_clicked()
{
	this->ui->cacheDirectoryPathEdit->setPath(QDir::tempPath());
}

void AdvancedOptionsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
	switch(this->ui->buttonBox->standardButton(button)) {
	case QDialogButtonBox::Apply:
		this->save();
		break;
	case QDialogButtonBox::Ok:
		this->save();
		this->accept();
		break;
	case QDialogButtonBox::Cancel:
		this->reject();
		break;
	}
}

void AdvancedOptionsDialog::save()
{
	QSettings settings;
	settings.beginGroup(QStringLiteral("advanced"));
	settings.setValue(KEY(CachingDirectory), this->ui->cacheDirectoryPathEdit->path());
	settings.endGroup();
}
