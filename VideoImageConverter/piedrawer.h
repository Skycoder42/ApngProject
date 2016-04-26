#ifndef PIEDRAWER_H
#define PIEDRAWER_H

#include <QIcon>
#include <QHash>

class PieDrawer
{
public:
	PieDrawer();

	QIcon getPie(int progress);

private:
	QIcon indeterminateIcon;
	QHash<int, QIcon> pieMap;
};

#endif // PIEDRAWER_H
