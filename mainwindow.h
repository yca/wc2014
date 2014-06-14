#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebElement>

namespace Ui {
class MainWindow;
}

class QWebPage;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
private slots:
	void pageLoadFinished(bool);
	void pageLoadProgress(int);

	void on_listScripts_currentRowChanged(int currentRow);

private:
	void examineChildElements(const QWebElement &parentElement, int debug);
	Ui::MainWindow *ui;
	QWebPage *page;
	QStringList scripts;
};

#endif // MAINWINDOW_H
