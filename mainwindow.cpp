#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	enableCipherMode(true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::enableCipherMode(bool enabled)
{
	ui->grpCipher->setEnabled(enabled);

	if (enabled){
		enableDecipherMode(false);
		enableBreakMode(false);
	}
}

void MainWindow::enableDecipherMode(bool enabled)
{
	ui->grpDecipher->setEnabled(enabled);

	if (enabled){
		enableBreakMode(false);
		enableCipherMode(false);
	}
}

void MainWindow::enableBreakMode(bool enabled)
{
	ui->grpBreak->setEnabled(enabled);
	if (enabled){
		enableDecipherMode(false);
		enableCipherMode(false);
	}
}

void MainWindow::on_rbtnCipher_clicked()
{
	enableCipherMode(true);
}

void MainWindow::on_rbtnDecipher_clicked()
{
	enableDecipherMode(true);
}

void MainWindow::on_rbtnBreak_clicked()
{
	enableBreakMode(true);
}

void MainWindow::on_btnChooseInputFile_clicked()
{
	QString inputFilename = QFileDialog::getOpenFileName(this, "Открыть файл", 0, "Все файлы (*)");
	if (!inputFilename.isNull()){
		ui->edtInputFile->setText(inputFilename);
	}
}

void MainWindow::on_btnChooseOutputFile_clicked()
{
	QString outputFilename = QFileDialog::getSaveFileName(this, "Сохранить файл", 0, "Все файлы (*)");
	if (!outputFilename.isNull()){
		ui->edtOutputFile->setText(outputFilename);
	}
}
