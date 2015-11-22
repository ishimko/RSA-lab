#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	setInputValidators();

	connect(ui->edtInputFile, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtOutputFile, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtOpenKey, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtP, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtQ, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtRBreak, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtRDecipher, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtSecretKeyCipher, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));
	connect(ui->edtSecretKeyDecipher, SIGNAL(textEdited(QString)), this, SLOT(checkFields()));

	connect(ui->rbtnBreak, SIGNAL(clicked()), this, SLOT(checkFields()));
	connect(ui->rbtnCipher, SIGNAL(clicked()), this, SLOT(checkFields()));
	connect(ui->rbtnDecipher, SIGNAL(clicked()), this, SLOT(checkFields()));

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

void MainWindow::displayError(ErrorType errorType){
	QMessageBox errorMessageBox;
	errorMessageBox.setWindowTitle("Ошибка");
	errorMessageBox.setIcon(QMessageBox::Critical);

	switch(errorType){
		case E_TOO_BIG_KEY:
			errorMessageBox.setText("Значение ключа больше двух байт!");
			break;
		case E_TOO_BIG_P_Q:
			errorMessageBox.setText("Произведение p и q больше двух байт!");
			break;
		case E_NOT_PRIME_P:
			errorMessageBox.setText("p не является простым числом!");
			break;
		case E_NOT_PRIME_Q:
			errorMessageBox.setText("q не является простым числом!");
			break;
		case E_INVALID_KEY:
			errorMessageBox.setText("Ключ должен быть взаимно простым со значением функции Эйлера от r!");
			break;
		case E_INVALID_INPUT_FILE:
			errorMessageBox.setText("Ошибка чтения входного файла!");
			break;
		case E_INVALID_OUTPUT_FILE:
			errorMessageBox.setText("Ошибка записи в выходной файл!");
			break;
		case E_TOO_SMALL_P_Q:
			errorMessageBox.setText("Произведение p и q должно быть больше 255!");
			break;
		default:
			errorMessageBox.setText("Неизвестная ошибка!");
			break;
	}
	errorMessageBox.exec();
}

void MainWindow::decipherMode(QString inputFileName, QString outputFileName){
	word secretKey = ui->edtSecretKeyDecipher->text().toUInt();
	word r = ui->edtRDecipher->text().toUInt();

	MainWindow::setEnabled(false);

	RSAWorker *rsaWorker = new RSAWorker(inputFileName, outputFileName, secretKey, r, MODE_DECIPHER);
	QThread *workerThread = getRSAWorkerThread(rsaWorker);

	workerThread->start();
	emit doWork();
}

void MainWindow::breakMode(QString inputFileName, QString outputFileName){
	word openKey = ui->edtOpenKey->text().toUInt();
	word r = ui->edtRBreak->text().toUInt();
	word q = getFirstDivider(r);
	word p = r / q;
	word eulerValue = (p - 1)*(q - 1);
	word secretKey = getMultiplicativeInverse(openKey, eulerValue);

	ui->txtLog->appendPlainText("\nНайденные сомножители: " + QString::number(p) + ", " + QString::number(q));
	ui->txtLog->appendPlainText("\nСекретный ключ: (" + QString::number(secretKey) + ", " + QString::number(r) + ")");

	MainWindow::setEnabled(false);

	RSAWorker *rsaWorker = new RSAWorker(inputFileName, outputFileName, secretKey, r, MODE_DECIPHER);
	QThread *workerThread = getRSAWorkerThread(rsaWorker);

	workerThread->start();
	emit doWork();
}

QThread *MainWindow::getRSAWorkerThread(RSAWorker *worker)
{
	QThread *workerThread= new QThread();

	connect(this, SIGNAL(destroyed()), worker, SLOT(deleteLater()));
	connect(worker, SIGNAL(destroyed()), workerThread, SLOT(quit()));
	connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
	worker->moveToThread(workerThread);
	connect(this, SIGNAL(doWork()), worker, SLOT(startWork()));


	connect(worker, SIGNAL(done(QString, QString)), this, SLOT(workDone(QString,QString)));

	connect(worker, SIGNAL(progress(int)), ui->progressBar, SLOT(setValue(int)));

	return workerThread;
}

bool MainWindow::validCipherModeFields()
{
	bool zero = ui->edtP->text().toInt() == 0 ||
				ui->edtQ->text().toInt() == 0 ||
				ui->edtSecretKeyCipher->text().toInt() == 0;

	return	ui->edtP->text() != "" &&
			ui->edtQ->text() != "" &&
			ui->edtSecretKeyCipher->text() != "" &&
			!zero;
}

bool MainWindow::validDecipherModeFields()
{
	bool zero = ui->edtRDecipher->text().toInt() == 0 ||
				ui->edtSecretKeyDecipher->text().toInt() == 0;

	return	ui->edtRDecipher->text() != "" &&
			ui->edtSecretKeyDecipher->text() != "" &&
			!zero;
}

bool MainWindow::validBreakModeFields()
{
	bool zero = ui->edtRBreak->text().toInt() == 0 ||
				ui->edtOpenKey->text().toInt() == 0;

	return	ui->edtRBreak->text() != "" &&
			ui->edtOpenKey->text() != "" &&
			!zero;
}

bool MainWindow::validFilesFields()
{
	return	ui->edtInputFile->text() != "" &&
			ui->edtOutputFile->text() != "";
}

void MainWindow::cipherMode(QString inputFileName, QString outputFileName){
	word q = ui->edtQ->text().toUInt();
	word p = ui->edtP->text().toUInt();
	uint32 r = q * p;
	word secretKey = ui->edtSecretKeyCipher->text().toUInt();

	bool error = false;

	if (r > 65535){
		displayError(E_TOO_BIG_P_Q);
		error = true;
	} else {
		if (r < 256){
			displayError(E_TOO_SMALL_P_Q);
			error = true;
		}
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
		ui->txtLog->clear();
		return;
	}

	word openKey = getMultiplicativeInverse(secretKey, eulerValue);

	ui->txtLog->appendPlainText("\nОткрытый ключ: (" + QString::number(openKey) + ", " + QString::number(r) + ")");

	MainWindow::setEnabled(false);

	RSAWorker *rsaWorker = new RSAWorker(inputFileName, outputFileName, openKey, r, MODE_CIPHER);
	QThread *workerThread = getRSAWorkerThread(rsaWorker);

	workerThread->start();
	emit doWork();
}

void MainWindow::on_btnProcess_clicked()
{
	QString inputFileName = getInputFileName(), outputFileName = getOutputFileName();
	QFile input(inputFileName), output(outputFileName);

	if (!input.open(QIODevice::ReadOnly)){
		displayError(E_INVALID_INPUT_FILE);
		return;
	}

	if (!output.open(QIODevice::WriteOnly)){
		displayError(E_INVALID_OUTPUT_FILE);
		return;
	}

	input.close();
	output.close();

	ui->txtLog->clear();
	ui->txtLog->appendPlainText("Размер входного файла: " + QString::number(input.size()) + " байт");

	if (ui->rbtnCipher->isChecked()){
		cipherMode(inputFileName, outputFileName);
		return;
	}

	if (ui->rbtnDecipher->isChecked()){
		decipherMode(inputFileName, outputFileName);
		return;
	}

	if (ui->rbtnBreak->isChecked()){
		breakMode(inputFileName, outputFileName);
		return;
	}
}

QString MainWindow::getInputFileName(){
	return ui->edtInputFile->text();
}

QString MainWindow::getOutputFileName(){
	return ui->edtOutputFile->text();
}

void MainWindow::checkFields()
{
	bool validFields = validFilesFields();

	if (ui->rbtnBreak->isChecked()){
		validFields = validFields &&  validBreakModeFields();
	}

	if (ui->rbtnCipher->isChecked()){
		validFields = validFields && validCipherModeFields();

	}
	if (ui->rbtnDecipher->isChecked()){
		validFields = validFields && validDecipherModeFields();
	}

	ui->btnProcess->setEnabled(validFields);
}

void MainWindow::workDone(QString srcFile, QString resultFile)
{
	RSAWorker *rsaWorker = static_cast<RSAWorker *>(sender());
	delete rsaWorker;

	ui->txtLog->appendPlainText("\nВходной файл: ");
	ui->txtLog->appendPlainText(srcFile);

	ui->txtLog->appendPlainText("\nВыходной файл: ");
	ui->txtLog->appendPlainText(resultFile);

	MainWindow::setEnabled(true);

	QMessageBox::information(this, "Информация", "Процесс завершен!");
	ui->progressBar->setValue(0);

}
