#ifndef CONVERTFILEMODEL_H
#define CONVERTFILEMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include <QFileIconProvider>
#include <QScopedPointer>
#include "convertfileinfo.h"
#include "piedrawer.h"

class ConvertFileModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit ConvertFileModel(QObject *parent = 0);

	ConvertFileInfo *item(const QModelIndex &index) const;
	void addItem(ConvertFileInfo *item);
	bool removeItem(const QModelIndex &index);
	bool removeItems(const QModelIndexList &indexes);

	QList<ConvertFileInfo *> allItems() const;

	// QAbstractItemModel interface
	int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
	int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const Q_DECL_OVERRIDE;

private slots:
	void reloadInfo(int column);

private:
	QList<ConvertFileInfo*> fileItems;
	QScopedPointer<QFileIconProvider> iconProvider;
	QScopedPointer<PieDrawer> pieDrawer;
};

#endif // CONVERTFILEMODEL_H
