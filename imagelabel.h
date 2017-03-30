#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QDebug>

class ImageLabel : public QLabel
{
	Q_PROPERTY(QString path READ path WRITE setPath NOTIFY pathChanged)
	Q_PROPERTY(QString downloadUrl READ downloadUrl WRITE setDownloadUrl NOTIFY downloadUrlChanged)
	Q_PROPERTY(QString folderName READ folderName WRITE setFolderName NOTIFY folderNameChanged)
	Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
public:
	ImageLabel(QWidget *parent = 0);

};

#endif // IMAGELABEL_H
