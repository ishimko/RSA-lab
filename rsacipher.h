#ifndef RSAWORKER_H
#define RSAWORKER_H

#include <QObject>
#include <QFile>

#include "types.h"
#include "rsaroutines.h"



class RSACipher : public QObject
{
	Q_OBJECT
public:
	explicit RSACipher(QString inputFileName, QString outputFileName, word key, word r, QObject *parent = 0);

private:
	word key, r;
	QString inputFileName, outputFileName;

signals:
	void progress(int progressValue);
	void done();

public slots:
	void cipher();

};

#endif // RSAWORKER_H
