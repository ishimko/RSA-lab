#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setInputValidators();

	enableCipherMode(true);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::setInputValidators(){
	QIntValidator *inputValidator = new QIntValidator(1, 65535, this);

	ui->edtP->setValidator(inputValidator);
	ui->edtQ->setValidator(inputValidator);
	ui->edtRBreak->setValidator(inputValidator);
	ui->edtRDecipher->setValidator(inputValidator);
	ui->edtSecretKeyCipher->setValidator(inputValidator);
	ui->edtSecretKeyDecipher->setValidator(inputValidator);
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
	word block;
	for (qint64 i = 0; i < f.size(); i += 2){
		f.read(((char *)&block) + 1, 1);
		f.read((char *)&block, 1);
		result.append(QString::number(block) + " ");
	}
	f.close();

	return result;
}

QString MainWindow::fileAsBytes(QString fileName){
	QFile f(fileName);
	f.open(QIODevice::ReadOnly);
	QString result;
	char block;
	for (qint64 i = 0; i < f.size(); i++){
		f.read(&block, 1);
		result.append(QString::number((byte)block) + " ");
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
		case E_INVALID_KEY:
			QMessageBox::critical(this, "Ошибка", "Ключ должен быть взаимно простым со значением функции Эйлера от r!");
			break;
		default:
			QMessageBox::critical(this, "Ошибка", "Неизвестная ошибка!");
			break;
	}

}

void MainWindow::decipherMode(){
	word secretKey = ui->edtSecretKeyDecipher->text().toUInt();
	word r = ui->edtRDecipher->text().toUInt();

	QString inputFileName = getInputFileName(), outputFileName = getOutputFileName();

	ui->txtLog->clear();

	QFile f(inputFileName);
	ui->txtLog->appendPlainText("Размер входного файла: " + QString::number(f.size()) + " байт");

	ui->txtLog->appendPlainText("\nВходной файл: ");

	ui->txtLog->appendPlainText(fileAsWords(inputFileName));

	MainWindow::setEnabled(false);

	RSAWorker *rsaDecipher = new RSAWorker(inputFileName, outputFileName, secretKey, r, MODE_DECIPHER);
	QThread *cipheringThread = new QThread();

	connect(this, SIGNAL(destroyed()), rsaDecipher, SLOT(deleteLater()));
	connect(rsaDecipher, SIGNAL(destroyed()), cipheringThread, SLOT(quit()));
	connect(cipheringThread, SIGNAL(finished()), cipheringThread, SLOT(deleteLater()));
	rsaDecipher->moveToThread(cipheringThread);
	connect(this, SIGNAL(doWork()), rsaDecipher, SLOT(startWork()));
	connect(rsaDecipher, SIGNAL(done()), this, SLOT(decipheringDone()));
	connect(rsaDecipher, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

	cipheringThread->start();
	emit doWork();
}

void MainWindow::breakMode(){
	word openKey = ui->edtOpenKey->text().toUInt();
	word r = ui->edtRBreak->text().toUInt();
	word q = getFirstDivider(r);
	word p = r / q;
	word eulerValue = (p - 1)*(q - 1);
	word secretKey = getMultiplicativeInverse(openKey, eulerValue);

	QString inputFileName = getInputFileName(), outputFileName = getOutputFileName();

	ui->txtLog->clear();
	ui->txtLog->appendPlainText("p = " + QString::number(p) + ", q = " + QString::number(q));
	ui->txtLog->appendPlainText("\nСекретный ключ: (" + QString::number(secretKey) + ", " + QString::number(r) + ")");
	QFile f(inputFileName);
	ui->txtLog->appendPlainText("\nРазмер входного файла: " + QString::number(f.size()) + " байт");

	ui->txtLog->appendPlainText("\nВходной файл: ");

	ui->txtLog->appendPlainText(fileAsWords(inputFileName));

	MainWindow::setEnabled(false);

	RSAWorker *rsaDecipher = new RSAWorker(inputFileName, outputFileName, secretKey, r, MODE_DECIPHER);
	QThread *cipheringThread = new QThread();

	connect(this, SIGNAL(destroyed()), rsaDecipher, SLOT(deleteLater()));
	connect(rsaDecipher, SIGNAL(destroyed()), cipheringThread, SLOT(quit()));
	connect(cipheringThread, SIGNAL(finished()), cipheringThread, SLOT(deleteLater()));
	rsaDecipher->moveToThread(cipheringThread);
	connect(this, SIGNAL(doWork()), rsaDecipher, SLOT(startWork()));
	connect(rsaDecipher, SIGNAL(done()), this, SLOT(decipheringDone()));
	connect(rsaDecipher, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

	cipheringThread->start();
	emit doWork();
}

QThread *MainWindow::getRSAWorkerThread(RSAWorker *worker){

}

void MainWindow::cipherMode(){
	word q = ui->edtQ->text().toUInt();
	word p = ui->edtP->text().toUInt();
	uint32 r = q * p;
	word secretKey = ui->edtSecretKeyCipher->text().toUInt();

	bool error = false;

	if (r > 65535){
		displayError(E_TOO_BIG_P_Q);
		error = true;
	}

	word eulerValue = (q - 1)*(p - 1);

	if (gcd(eulerValue, secretKey) != 1){
		displayError(E_INVALID_KEY);
		error = true;
	}

	if (!isPrime(p)){
		displayError(E_NOT_PRIME_P);
		error = true;
	}
	if (!isPrime(q)){
		displayError(E_NOT_PRIME_Q);
		error = true;
	}

	if (error){
		return;
	}

	word openKey = getMultiplicativeInverse(secretKey, eulerValue);

	QString inputFileName = getInputFileName(), outputFileName = getOutputFileName();


	ui->txtLog->clear();

	ui->txtLog->appendPlainText("Открытый ключ: (" + QString::number(openKey) + ", " + QString::number(r) + ")");

	QFile f(inputFileName);
	ui->txtLog->appendPlainText("\nРазмер входного файла: " + QString::number(f.size()) + " байт");

	ui->txtLog->appendPlainText("\nВходной файл: ");

	ui->txtLog->appendPlainText(fileAsBytes(inputFileName));

	MainWindow::setEnabled(false);

	RSAWorker *rsaCipher = new RSAWorker(inputFileName, outputFileName, openKey, r, MODE_CIPHER);
	QThread *decipheringThread = new QThread();

	connect(this, SIGNAL(destroyed()), rsaCipher, SLOT(deleteLater()));
	connect(rsaCipher, SIGNAL(destroyed()), decipheringThread, SLOT(quit()));
	connect(decipheringThread, SIGNAL(finished()), decipheringThread, SLOT(deleteLater()));
	rsaCipher->moveToThread(decipheringThread);
	connect(this, SIGNAL(doWork()), rsaCipher, SLOT(startWork()));
	connect(rsaCipher, SIGNAL(done()), this, SLOT(decipheringDone()));
	connect(rsaCipher, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

	decipheringThread->start();
	emit doWork();
}

void MainWindow::on_btnProcess_clicked()
{
	if (ui->rbtnCipher->isChecked()){
		cipherMode();
		return;
	}

	if (ui->rbtnDecipher->isChecked()){
		decipherMode();
		return;
	}

	if (ui->rbtnBreak->isChecked()){
		breakMode();
		return;
	}
}

QString MainWindow::getInputFileName(){
	return ui->edtInputFile->text();
}

QString MainWindow::getOutputFileName(){
	return ui->edtOutputFile->text();
}

void MainWindow::cipheringDone()
{
	RSAWorker *rsaWorker = static_cast<RSAWorker *>(sender());
	delete rsaWorker;

	ui->txtLog->appendPlainText("\nВыходной файл: ");
	ui->txtLog->appendPlainText(fileAsWords(getOutputFileName()));

	MainWindow::setEnabled(true);

	QMessageBox::information(this, "Информация", "Процесс завершен!");
	ui->progressBar->setValue(0);

}

void MainWindow::decipheringDone()
{
	RSAWorker *rsaWorker = static_cast<RSAWorker *>(sender());
	delete rsaWorker;

	ui->txtLog->appendPlainText("\nВыходной файл: ");
	ui->txtLog->appendPlainText(fileAsBytes(getOutputFileName()));

	MainWindow::setEnabled(true);

	QMessageBox::information(this, "Информация", "Процесс завершен!");
	ui->progressBar->setValue(0);
}
