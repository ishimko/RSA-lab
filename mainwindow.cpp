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

QString MainWindow::fileAsWords(QString fileName){
	QFile f(fileName);
	f.open(QIODevice::ReadOnly);
	QString result;
	char block[2];
	for (qint64 i = 0; i < f.size(); i += 2){
		f.read(block + 1, 1);
		f.read(block, 1);
		result.append(QString::number(*(word *)(block)) + " ");
	}
	f.close();

	return result;
}

void MainWindow::displayError(ErrorType errorType){
	switch(errorType){
		case E_TOO_BIG_KEY:
			QMessageBox::critical(this, "Ошибка", "Значение ключа не является двухбайтовым!");
			break;
		case E_TOO_BIG_P_Q:
			QMessageBox::critical(this, "Ошибка", "Произведение p и q не является двухбайтовым!");
			break;
		case E_NOT_PRIME_P:
			QMessageBox::critical(this, "Ошибка", "p не является простым числом!");
			break;
		case E_NOT_PRIME_Q:
			QMessageBox::critical(this, "Ошибка", "q не является простым числом!");
			break;
		case E_NOT_PRIME:
			QMessageBox::critical(this, "Ошибка", "q и p не являются простыми числами!");
			break;
		case E_INVALID_KEY:
			QMessageBox::critical(this, "Ошибка", "Ключ должен быть взаимно простым со значением функции Эйлера от r!");
			break;
		default:
			QMessageBox::critical(this, "Ошибка", "Неизвестная ошибка!");
			break;
	}

}

void MainWindow::on_btnProcess_clicked()
{
	word q= ui->edtQ->text().toUInt();
	word p = ui->edtP->text().toUInt();
	uint32 r = q * p;
	word secretKey = ui->edtSecretKeyCipher->text().toUInt();

	if (r > 65535){
		displayError(E_TOO_BIG_P_Q);
		return;
	}

	word eulerValue = (q - 1)*(p - 1);

	if (gcd(eulerValue, secretKey)){
		displayError(E_INVALID_KEY);
	}

	bool isPrimeP = isPrime(p);
	bool isPrimeQ = isPrime(q);
	if (!isPrimeQ && !isPrimeP){
		displayError(E_NOT_PRIME);
		return;
	} else {
		if (!isPrimeP){
			displayError(E_NOT_PRIME_P);
			return;
		}
		if (!isPrimeQ){
			displayError(E_NOT_PRIME_Q);
			return;
		}
	}

	word openKey = getMultiplicativeInverse(secretKey, eulerValue);

	MainWindow::setEnabled(false);

	RSACipher *rsaCipher = new RSACipher(getInputFileName(), getOutputFileName(), openKey, r);
	QThread *cipheringThread = new QThread();

	connect(this, SIGNAL(destroyed()), rsaCipher, SLOT(deleteLater()));
	connect(rsaCipher, SIGNAL(destroyed()), cipheringThread, SLOT(quit()));
	connect(cipheringThread, SIGNAL(finished()), cipheringThread, SLOT(deleteLater()));
	rsaCipher->moveToThread(cipheringThread);
	connect(this, SIGNAL(doWork()), rsaCipher, SLOT(cipher()));
	connect(rsaCipher, SIGNAL(done()), this, SLOT(cipheringDone()));
	connect(rsaCipher, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

	cipheringThread->start();
	emit doWork();
}

QString MainWindow::getInputFileName(){
	return ui->edtInputFile->text();
}

QString MainWindow::getOutputFileName(){
	return ui->edtOutputFile->text();
}

void MainWindow::cipheringDone()
{
	RSACipher *rsaCipher = static_cast<RSACipher *>(sender());
	delete rsaCipher;
	ui->txtLog->appendPlainText(fileAsWords(getOutputFileName()));
	MainWindow::setEnabled(true);
	QMessageBox::information(this, "Информация", "Процесс завершен!");
	ui->progressBar->setValue(0);

}
