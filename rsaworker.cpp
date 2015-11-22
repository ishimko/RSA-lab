#include "rsaworker.h"

RSAWorker::RSAWorker(QString inputFileName, QString outputFileName, word key, word r, WorkerMode workerMode, QObject *parent):
	QObject(parent),
	key(key),
	r(r),
	inputFileName(inputFileName),
	outputFileName(outputFileName),
	workerMode(workerMode)
{
}

void RSAWorker::startWork()
{
	QFile inputFile(inputFileName);
	QFile outputFile(outputFileName);
	inputFile.open(QIODevice::ReadOnly);
	outputFile.open(QIODevice::WriteOnly);

	quint64 fileSize = inputFile.size();
	if (workerMode == MODE_DECIPHER)
		fileSize /= 2;

	word in;
	word out;
	ResultWriter writeResult = (workerMode == MODE_CIPHER)?&writeCipheredWord:&writeDecipheredByte;
	InputReader readBlock = (workerMode == MODE_CIPHER)?&readPlaintextByte:&readCipheredWord;

	QString srcFile, resultFile;

	for (quint64 i = 0; i < fileSize; i++){
		in = readBlock(&inputFile);
		srcFile.append(QString::number(in) + " ");

		out = fastModularExponentiation(in, key, r);
		resultFile.append(QString::number(out) + " ");

		writeResult(out, &outputFile);

		if (!(i % 100000)){
			emit progress(100 * i/fileSize);
		}
	}

	inputFile.close();
	outputFile.close();

	emit progress(100);
	emit done(srcFile, resultFile);
}




