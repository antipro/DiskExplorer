#include "mainwindow.h"
#include <QApplication>
#include <iostream>

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
	QByteArray localMsg = msg.toLocal8Bit();

	QString strMsg("");
	switch(type)
	{
	case QtDebugMsg:
		strMsg = QString("Debug:");
		break;
	case QtWarningMsg:
		strMsg = QString("Warning:");
		break;
	case QtCriticalMsg:
		strMsg = QString("Critical:");
		break;
	case QtFatalMsg:
		strMsg = QString("Fatal:");
		break;
	}

	QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");



	#ifdef QT_NO_DEBUG
		QString strMessage = QString("%1 %2")
			.arg(strDateTime).arg(localMsg.constData());
		QFile file("DiskExplorer.log");
		file.open(QIODevice::ReadWrite | QIODevice::Append);
		QTextStream stream(&file);
		stream << strMessage << "\r\n";
		file.flush();
		file.close();
	#else
		QString strMessage = QString("%1 %2 (%3:%4:%5)")
			.arg(strDateTime).arg(localMsg.constData()).arg(context.file).arg(context.function).arg(context.line);
		std::cout << strMessage.toStdString() << std::endl;
	#endif
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	qInstallMessageHandler(customMessageHandler);
    MainWindow w;
    w.show();

    return a.exec();
}
