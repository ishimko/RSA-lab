#ifndef RSAWORKER_H
#define RSAWORKER_H

#include <QObject>
#include <QFile>

#include "types.h"
#include "rsaroutines.h"



class RSAWorker : public QObject
{
	Q_OBJECT
public:
	explicit RSAWorker(QString inputFileName, QString outputFileName, word key, word r, WorkerMode workerMode, QObject *parent = 0);

private:
	word key, r;
	QString inputFileName, outputFileName;
	WorkerMode workerMode;

signals:
	void progress(int progressValue);
	void done();

public slots:
	void startWork();

};

#endif // RSAWORKER_H
