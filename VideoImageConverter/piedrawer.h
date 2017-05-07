#ifndef PIEDRAWER_H
#define PIEDRAWER_H

#include <QIcon>
#include <QHash>

class PieDrawer
{
public:
	static QIcon getPie(double progress);

private:
	static QIcon indeterminateIcon;
	static QHash<int, QIcon> pieMap;

	PieDrawer() Q_DECL_EQ_DELETE;
	~PieDrawer() Q_DECL_EQ_DELETE;
};

#endif // PIEDRAWER_H
