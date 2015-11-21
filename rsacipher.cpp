#include "rsacipher.h"

RSACipher::RSACipher(QString inputFileName, QString outputFileName, int q, int p, int secretKey, QObject *parent):
	QObject(parent),
	q(q),
	p(p),
	secretKey(secretKey),
	inputFileName(inputFileName),
	outputFileName(outputFileName)
{
}

void RSACipher::cipher()
{
	QFile inputFile(inputFileName);
	QFile outputFile(outputFileName);
	inputFile.open(QIODevice::ReadOnly);
	outputFile.open(QIODevice::WriteOnly);

	word eulerValue = (q - 1)*(p - 1);
	word r = p*q;
	word openKey = getMultiplicativeInverse(secretKey, eulerValue);
	quint64 fileSize = inputFile.size();

	char in, firstByte, secondByte;
	word out;
	for (quint64 i = 0; i < fileSize; i++){
		inputFile.read(&in, 1);
		out = fastModularExponentiation(in, openKey, r);
		firstByte = out & 255;
		secondByte = out >> 8;
		outputFile.write(&secondByte, 1);
		outputFile.write(&firstByte, 1);
		if (!(i % 100000)){
			emit progress(100 * i/fileSize);
		}
	}
	inputFile.close();
	outputFile.close();
	emit progress(100);
	emit done();
}




