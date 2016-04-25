#include "convertfilemodel.h"

ConvertFileModel::ConvertFileModel(QObject *parent) :
	QAbstractTableModel(parent),
	fileItems(),
	iconProvider(new QFileIconProvider())
{}

ConvertFileModel::~ConvertFileModel()
{
	qDeleteAll(this->fileItems);
}

ConvertFileInfo *ConvertFileModel::item(const QModelIndex &index) const
{
	if(!index.isValid() ||
	   index.row() < 0 ||
	   index.row() >= this->fileItems.size()) {
		return Q_NULLPTR;
	} else
		return this->fileItems[index.row()];
}

void ConvertFileModel::addItem(ConvertFileInfo *item)
{
	this->beginInsertRows(QModelIndex(), this->fileItems.size(), this->fileItems.size());
	this->fileItems.append(item);
	this->endInsertRows();
}

bool ConvertFileModel::removeItem(const QModelIndex &index)
{
	if(!index.isValid() ||
	   index.row() < 0 ||
	   index.row() >= this->fileItems.size()) {
		return false;
	} else {
		this->beginRemoveRows(QModelIndex(), index.row(), index.row());
		delete this->fileItems.takeAt(index.row());
		this->endRemoveRows();
		return true;
	}
}

bool ConvertFileModel::removeItems(const QModelIndexList &indexes)
{
	QList<QPersistentModelIndex> persIdxs;
	foreach (auto index, indexes)
		persIdxs.append(index);

	foreach (auto pIndex, persIdxs) {
		if(pIndex.isValid() &&
		   pIndex.row() >= 0 &&
		   pIndex.row() < this->fileItems.size()) {
			this->beginRemoveRows(QModelIndex(), pIndex.row(), pIndex.row());
			delete this->fileItems.takeAt(pIndex.row());
			this->endRemoveRows();
		}
	}

	return true;
}

QList<ConvertFileInfo*> ConvertFileModel::allItems() const
{
	return this->fileItems;
}

int ConvertFileModel::rowCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;
	else
		return this->fileItems.size();
}

int ConvertFileModel::columnCount(const QModelIndex &parent) const
{
	if(parent.isValid())
		return 0;
	else
		return 2;
}

QVariant ConvertFileModel::data(const QModelIndex &index, int role) const
{
	switch (role) {
	case Qt::DisplayRole:
		switch (index.column()) {
		case 0:
			return this->fileItems[index.row()]->statusText();
		case 1:
			return this->fileItems[index.row()]->filename();
		}
	case Qt::DecorationRole:
		switch (index.column()) {
		case 0:
			return this->fileItems[index.row()]->statusIcon();
		case 1:
			return this->iconProvider->icon(this->fileItems[index.row()]->filename());
		}
	case Qt::ToolTipRole:
		return this->fileItems[index.row()]->filename();
	}

	return QVariant();
}

QVariant ConvertFileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation == Qt::Horizontal &&
	   role == Qt::DisplayRole) {
		switch(section) {
		case 0:
			return tr("Status");
		case 1:
			return tr("Filename");
		}
	}

	return QVariant();
}

void ConvertFileModel::reloadInfo(ConvertFileInfo *info)
{
	auto index = this->fileItems.indexOf(info);
	if(index >= 0)
		emit dataChanged(this->index(index, 0), this->index(index, 1));
}
