#ifndef COMMON_H
#define COMMON_H

#include <QUrl>
#include <QObject>

class MyDiskCache;
class QNetworkAccessManager;

class Common : public QObject
{
	Q_OBJECT
public:
	Common(QObject *parent = NULL);
	int getUrl(QUrl url, QByteArray &result, int timeout = 0);
private:
	QNetworkAccessManager *nm;
	MyDiskCache *diskCache;

	int cacheResult(const QUrl url, const QByteArray &ba);
	int checkCache(const QUrl url, QByteArray &ba);
};

#endif // COMMON_H
