#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

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

private:
	Ui::MainWindow *ui;

	void enableCipherMode(bool enabled);
	void enableDecipherMode(bool enabled);
	void enableBreakMode(bool enabled);

};

#endif // MAINWINDOW_H
