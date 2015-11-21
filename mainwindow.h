#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <QFile>

#include "types.h"
#include "rsacipher.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_rbtnCipher_clicked();

	void on_rbtnDecipher_clicked();

	void on_rbtnBreak_clicked();

	void on_btnChooseInputFile_clicked();

	void on_btnChooseOutputFile_clicked();

	void on_btnProcess_clicked();

	void cipheringDone();

signals:
	void doWork();
	void pause();
	void start();

private:
	Ui::MainWindow *ui;

	void enableCipherMode(bool enabled);
	void enableDecipherMode(bool enabled);
	void enableBreakMode(bool enabled);

	QString fileAsWords(QString fileName);
	QString getInputFileName();
	QString getOutputFileName();
	void displayError(ErrorType errorType);
	void cipherMode();
	QString fileAsBytes(QString fileName);
};

#endif // MAINWINDOW_H
