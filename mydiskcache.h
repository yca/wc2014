#ifndef MYDISKCACHE_H
#define MYDISKCACHE_H

#include <QNetworkDiskCache>
#include <QAbstractNetworkCache>

class MyDiskCache : public QAbstractNetworkCache
{
	Q_OBJECT
public:
	explicit MyDiskCache(QObject *parent = 0);
	virtual qint64 cacheSize () const;
	virtual QIODevice *	data ( const QUrl & url );
	virtual void insert ( QIODevice * device );
	virtual QNetworkCacheMetaData	metaData ( const QUrl & url );
	virtual QIODevice *	prepare ( const QNetworkCacheMetaData & metaData );
	virtual bool remove ( const QUrl & url );
	virtual void updateMetaData ( const QNetworkCacheMetaData & metaData );
public slots:
	virtual void clear();
protected:
	QNetworkDiskCache dc;

};

#endif // MYDISKCACHE_H
