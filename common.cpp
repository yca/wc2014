#include "common.h"
#include "mydiskcache.h"

#include <QDir>
#include <QTimer>
#include <QDebug>
#include <QProcess>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkCookie>
#include <QNetworkRequest>
#include <QNetworkDiskCache>
#include <QNetworkAccessManager>

#include <errno.h>

static int getFile(QString url, QString localName, int timeout = 0, int expectedSize = 0)
{
	int err = 0;
	QTimer t;
	QEventLoop el;
	QNetworkAccessManager nm;
	QObject::connect(&nm, SIGNAL(finished(QNetworkReply*)), &el, SLOT(quit()));
	QNetworkReply *reply = nm.get(QNetworkRequest(QUrl(url)));
	QObject::connect(reply, SIGNAL(finished()), &el, SLOT(quit()));
	QObject::connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &el, SLOT(quit()));
	QObject::connect(&t, SIGNAL(timeout()), &el, SLOT(quit()));
	if (timeout)
		t.start(timeout);
	el.exec();
	if (reply->error() != QNetworkReply::NoError) {
		return reply->error();
	} else if (reply->isFinished()) {
		QByteArray ba = reply->readAll();
		if (expectedSize == 0 || ba.size() == expectedSize) {
			QFile f(localName);
			f.open(QIODevice::WriteOnly);
			f.write(ba);
			f.close();
		} else
			err = -EIO;
	} else {
		reply->abort();
		return -ETIMEDOUT;
	}
	reply->deleteLater();
	return err;
}

Common::Common(QObject *parent)
	: QObject(parent)
{
	nm = new QNetworkAccessManager(this);
	diskCache = new MyDiskCache(this);
	//nm->setCache(diskCache);
}

int Common::getUrl(QUrl url, QByteArray &result, int timeout)
{
	if (!checkCache(url, result)) {
		qDebug() << "returning from cache";
		return 0;
	}
	int err = 0;
	QTimer t;
	QEventLoop el;
	QNetworkRequest req(url);
	req.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
	req.setAttribute(QNetworkRequest::CacheSaveControlAttribute, true);
	QNetworkReply *reply = nm->get(req);
	connect(reply, SIGNAL(finished()), &el, SLOT(quit()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &el, SLOT(quit()));
	connect(&t, SIGNAL(timeout()), &el, SLOT(quit()));
	if (timeout)
		t.start(timeout);
	el.exec();
	if (reply->error() != QNetworkReply::NoError) {
		err = reply->error();
	} else if (reply->isFinished()) {
		result.append(reply->readAll());
		/* cache result */
		err = cacheResult(url, result);
	} else {
		reply->abort();
		err = -ETIMEDOUT;
	}

	reply->deleteLater();
	return err;
}

int Common::cacheResult(const QUrl url, const QByteArray &ba)
{
	QDir d("");
	d.mkpath(QString("cache/%1").arg(url.host()));
	QString filename = QString("cache/%1/%2").arg(url.host()).arg(url.path());
	/*if (!url.fragment().isEmpty()) {
		d.mkpath(filename);
		filename = QString("cache/%1/%2/%3").arg(url.host()).arg(url.path()).arg(url.fragment());
	}*/
	d.mkpath(filename);
	QFile f(filename.append(".cache"));
	if (!f.open(QIODevice::WriteOnly))
		return -1;
	f.write(ba);
	f.close();
	return 0;
}

int Common::checkCache(const QUrl url, QByteArray &ba)
{
	QString filename = QString("cache/%2/%3").arg(url.host()).arg(url.path());
	//if (!url.fragment().isEmpty())
		//filename = QString("cache/%2/%3/").arg(url.host()).arg(url.path()).arg(url.fragment());
	filename.append(".cache");
	if (!QFile::exists(filename))
		return -ENOENT;
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	ba.append(f.readAll());
	f.close();
	return 0;
}
