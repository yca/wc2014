#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "common.h"

#include <QDebug>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <unistd.h>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	QByteArray res;
	Common cnm(this);
	//int err = cnm.getUrl(QUrl("http://www.whoscored.com/Statistics"), res);
	//int err = cnm.getUrl(QUrl("http://www.whoscored.com/StatisticsFeed/0/BestEleven"), res);
	int err = cnm.getUrl(QUrl("http://www.whoscored.com/Teams/37"), res);
	qDebug() << err << res.size();

	page = new QWebPage;
	connect(page, SIGNAL(loadProgress(int)), SLOT(pageLoadProgress(int)));
	connect(page, SIGNAL(loadFinished(bool)), SLOT(pageLoadFinished(bool)));
	page->mainFrame()->setHtml(QString::fromUtf8(res));
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::pageLoadFinished(bool ok)
{
	if (!ok) {
		qDebug() << "error parsing page";
		return;
	}
	QWebElement el = page->mainFrame()->documentElement();
	while (!el.isNull()) {
		examineChildElements(el, 0);
		el = el.nextSibling();
	}

	for (int i = 0; i < scripts.size(); i++) {
		ui->listScripts->addItem(QString("Script %1").arg(i + 1));
	}
}

void MainWindow::pageLoadProgress(int p)
{
	qDebug() << "load" << p;
}

void MainWindow::examineChildElements(const QWebElement &parentElement, int debug)
{
	QWebElement element = parentElement.firstChild();
	while (!element.isNull())  {
		if (debug > 0)
			qDebug() << element.tagName();
		if (element.tagName() == "SCRIPT") {
			scripts << element.toPlainText();
			examineChildElements(element, 1);
		} else
			examineChildElements(element, debug);
		element = element.nextSibling();
	}
}

class MyArray
{
public:
	MyArray()
	{

	}
	QString data()
	{
		return nodeData;
	}
	MyArray * child(int i)
	{
		return &children[i];
	}
	int childCount()
	{
		return children.size();
	}
	void append(QString s)
	{
		if (s.isEmpty())
			return;
		qDebug() << "pushing node" << s;
		children << MyArray(s);
	}
	void append(MyArray arr)
	{
		qDebug() << "pushing child node" << arr.childCount();
		children << arr;
	}
	void dump()
	{
		if (!nodeData.isEmpty())
			qDebug() << nodeData;
		for (int i = 0; i < children.size(); i++)
			children[i].dump();
		//foreach (QString d, data)
			//qDebug() << data;
	}

protected:
	MyArray(QString s)
	{
		nodeData = s;
	}

	QList<MyArray> children;
	QString nodeData;
};
#include <QStack>
static MyArray parseLoadList(const QString &part)
{
	QStack<MyArray> stack;
	stack.push(MyArray());
	QString str;
	for (int i = 0; i < part.size(); i++) {
		QChar c = part[i];
		if (c == '(')
			str.clear();
		else if (c == '{')
			stack.push(MyArray());
		else if (c == '[') {
			str.clear();
			stack.push(MyArray());
		} else if (c == ']' || c == '}') {
			stack.top().append(str.trimmed());
			MyArray top = stack.pop();
			stack.top().append(top);
			str.clear();
		} else if (c == ',') {
			stack.top().append(str.trimmed());
			str.clear();
		} else
			str.append(c);
	}
	return stack.top();
}

static QString extractPart(QString scr, QString starter, QString ender)
{
	if (scr.contains(starter)) {
		int start = scr.indexOf(starter);
		QString part = scr.mid(start);
		int end = part.indexOf(ender);
		if (end > 0)
			part = part.mid(0, end + ender.size());
		return part;
	}
	return "";
}

#include <QTreeView>
#include <QStandardItemModel>

QStandardItem * arrayToItem(MyArray *array)
{
	QStandardItem *item = new QStandardItem(array->data());
	if (item->text().isEmpty())
		item->setText("Array");
	for (int i = 0; i < array->childCount(); i++) {
		item->appendRow(arrayToItem(array->child(i)));
	}
	return item;
}

void MainWindow::on_listScripts_currentRowChanged(int currentRow)
{
	if (currentRow < 0)
		return;

	QString scr = scripts[currentRow];
	ui->plainScriptSource->setPlainText(scr);
#if 0
	QScriptEngine e;
	QScriptSyntaxCheckResult r = e.checkSyntax(scr);
	if (r.state() == QScriptSyntaxCheckResult::Valid) {
		e.evaluate(scr);
		QScriptValue v = e.globalObject();
		qDebug() << e.hasUncaughtException() << e.uncaughtExceptionLineNumber();
		v = v.property("teamFixtures");
		qDebug() << v.isArray() << v.isFunction() << v.toString();
#if 1
		while (v.isObject()) {
			QScriptValueIterator it(v);
			while (it.hasNext()) {
				it.next();
				qDebug() << it.name();
			}
			v = v.prototype();
		}
#endif
	}
#endif
	QString part = extractPart(scr, "teamFixtures.load([", "]]);");
	if (!part.isEmpty()) {
		//MyArray array = parseLoadList(part);
		//array.child(1).child(5).dump();
	}

	part = extractPart(scr, "DataStore.prime('stage-player-stat'", "]);");
	if (!part.isEmpty()) {
		MyArray array = parseLoadList(part);
		QTreeView v;
		QStandardItemModel *model = new QStandardItemModel;
		model->setColumnCount(2);
		model->invisibleRootItem()->appendRow(arrayToItem(&array));
		v.setModel(model);
		v.show();
		while (v.isVisible())
			QApplication::processEvents();
	}
}
