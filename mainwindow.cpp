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
	QString inputFileName = QFileDialog::getOpenFileName(this, "Открыть файл", 0, "Все файлы (*)");
	if (!inputFileName.isNull()){
		ui->edtInputFile->setText(inputFileName);
	}
}

void MainWindow::on_btnChooseOutputFile_clicked()
{
	QString outputFileName = QFileDialog::getSaveFileName(this, "Сохранить файл", 0, "Все файлы (*)");
	if (!outputFileName.isNull()){
		ui->edtOutputFile->setText(outputFileName);
	}
}

void MainWindow::on_btnProcess_clicked()
{
	byte q = ui->edtQ->text().toInt();
	byte p = ui->edtP->text().toInt();
	word secretKey = ui->edtSecretKeyCipher->text().toInt();

	RSACipher *rsaCipher = new RSACipher(ui->edtInputFile->text(), ui->edtOutputFile->text(), q, p, secretKey);
	QThread *cipheringThread = new QThread();
	connect(this, SIGNAL(destroyed()), rsaCipher, SLOT(deleteLater()));
	connect(rsaCipher, SIGNAL(destroyed()), cipheringThread, SLOT(quit()));
	connect(cipheringThread, SIGNAL(finished()), cipheringThread, SLOT(deleteLater()));
	rsaCipher->moveToThread(cipheringThread);
	connect(this, SIGNAL(doWork()), rsaCipher, SLOT(cipher()));
	connect(rsaCipher, SIGNAL(done()), this, SLOT(processDone()));
	connect(rsaCipher, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));
	cipheringThread->start();
	emit doWork();
}

void MainWindow::processDone()
{
	QMessageBox::information(this, "Информация", "Процесс завершен!");

}
