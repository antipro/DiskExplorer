#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <QMainWindow>
#include <QNetworkReply>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QStringListModel>
#include <QMovie>
#include <QBuffer>
#include <QLabel>
#include <QList>
#include <QInputDialog>
#include <QStandardItemModel>
#include "flowlayout.h"
#include "imagelabel.h"
#include "about.h"
#include "prerefrence.h"

Q_DECLARE_METATYPE(QStandardItem*)

namespace Ui {
class MainWindow;
}

/**
 * Main Window
 * @author antipro
 * @date 2016-11-02
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	void downloadFile(const QModelIndex &index, const QUrl &url);

	enum MetaDataType{
		PATH = Qt::UserRole,
		DOWNLOAD_URL,
		FOLDER_NAME,
		FILE_NAME
	};

public slots:
	void changeEvent(QEvent *e);
private slots:
    void on_comboBox_currentIndexChanged(const QString &name);
    void connectToDisk();
    void changeToken();
    void exit();
    void getAll();
    void switchSideBar();
	void about();
    void prerefrence();
	void switchLang(QAction *action);
	void newFolder();
	void uploadFile();
	void deleteFolder();
	void refreshFolder();

	void on_listView_doubleClicked(const QModelIndex &index);

private:
	std::map<QNetworkReply*, QModelIndex> replyMap;
    Ui::MainWindow *ui;
	QTranslator translator;
	About *aboutDialog;
    Prerefrence *refDialog;
	QStandardItemModel *model;
	QNetworkDiskCache *diskCache;
    QByteArray token;
	QNetworkRequest getRequest(const QUrl &url);
	QStandardItem* getViewItem(const QString &name, const QString &preview);
    void setupUiEx();
	ImageLabel * getImageLabel();
	QNetworkAccessManager *downloadManager;
};

#endif // MAINWINDOW_H
