#include "rsacipher.h"

RSACipher::RSACipher(QString inputFileName, QString outputFileName, word key, word r, QObject *parent):
	QObject(parent),
	key(key),
	r(r),
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

	quint64 fileSize = inputFile.size();

	char in, firstByte, secondByte;
	word out;
	for (quint64 i = 0; i < fileSize; i++){
		inputFile.read(&in, 1);
		out = fastModularExponentiation(in, key, r);
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




