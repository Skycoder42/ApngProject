#include "convertfilemodel.h"

ConvertFileModel::ConvertFileModel(QObject *parent) :
	QAbstractTableModel(parent),
	fileItems(),
	iconProvider(new QFileIconProvider()),
	pieDrawer(new PieDrawer())
{}

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
	item->setParent(this);
	connect(item, &ConvertFileInfo::statusChanged,
			this, [this]() {this->reloadInfo(0);});
	connect(item, &ConvertFileInfo::resultTextChanged,
			this, [this]() {this->reloadInfo(2);});
	connect(item, &ConvertFileInfo::currentProgressChanged,
			this, [this]() {this->reloadInfo(3);});
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
		this->fileItems.takeAt(index.row())->deleteLater();
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
			this->fileItems.takeAt(pIndex.row())->deleteLater();
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
		return 4;
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
		case 2:
			return this->fileItems[index.row()]->resultText();
		case 3:
			return this->fileItems[index.row()]->progressText();
		}
		break;
	case Qt::DecorationRole:
		switch (index.column()) {
		case 0:
			return this->fileItems[index.row()]->statusIcon();
		case 1:
			return this->iconProvider->icon(this->fileItems[index.row()]->filename());
		case 3:
		{
			auto item = this->fileItems[index.row()];
			if(!item->progressText().isEmpty())
				return this->pieDrawer->getPie(item->currentProgress());
			break;
		}
		}
		break;
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
		case 2:
			return tr("Status-Text");
		case 3:
			return tr("Progress");
		}
	}

	return QVariant();
}

void ConvertFileModel::reloadInfo(int column)
{
	auto index = this->fileItems.indexOf(qobject_cast<ConvertFileInfo*>(QObject::sender()));
	if(index >= 0)
		emit dataChanged(this->index(index, column), this->index(index, column));
}
