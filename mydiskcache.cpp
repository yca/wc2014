#include "mydiskcache.h"

#include <QDebug>

MyDiskCache::MyDiskCache(QObject *parent) :
	QAbstractNetworkCache(parent)
{
	dc.setCacheDirectory("cache");
	dc.setMaximumCacheSize(1024 * 1024 * 500);
}

qint64 MyDiskCache::cacheSize() const
{
	return dc.cacheSize();
}

QIODevice *MyDiskCache::data(const QUrl &url)
{
	qDebug() << "data";
	return dc.data(url);
}

void MyDiskCache::insert(QIODevice *device)
{
	qDebug() << "insert";
	dc.insert(device);
}

QNetworkCacheMetaData MyDiskCache::metaData(const QUrl &url)
{
	QNetworkCacheMetaData md = dc.metaData(url);
	return md;
}

QIODevice * MyDiskCache::prepare(const QNetworkCacheMetaData &metaData)
{
	QNetworkCacheMetaData md(metaData);
	md.setSaveToDisk(true);
	QIODevice *dev = dc.prepare(md);
	return dev;
}

bool MyDiskCache::remove(const QUrl &url)
{
	qDebug() << "remove";
	return dc.remove(url);
}

void MyDiskCache::updateMetaData(const QNetworkCacheMetaData &metaData)
{
	qDebug() << "update";
	dc.updateMetaData(metaData);
}

void MyDiskCache::clear()
{
	dc.clear();
}
