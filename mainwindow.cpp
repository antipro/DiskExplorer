#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUiEx();
	qApp->installTranslator(&translator);

    QSettings *config = new QSettings("config.ini", QSettings::IniFormat);
	// 请在config.ini中添加好自己的Token。
	token = config->value("token", QVariant("OAth")).toByteArray();
    if(config->isWritable())
        config->setValue("token", QString(token));
    delete config;

	diskCache = new QNetworkDiskCache(this);
    diskCache->setCacheDirectory("cacheDir");
	diskCache->setMaximumCacheSize(104857600);
	
	downloadManager = new QNetworkAccessManager(this);
	downloadManager->setCache(diskCache);
	connect(downloadManager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
		QModelIndex index = this->replyMap[reply];
		this->replyMap.erase(reply);
		if (reply->error() != QNetworkReply::NoError)
		{
			ui->statusBar->showMessage(reply->errorString());
			return;
		}
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if(statusCode == 302)
		{
			QUrl newUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
			qDebug() << "redirected to " + newUrl.toString();
			downloadFile(index, newUrl);
			return;
		}
		ui->listView->model()->setData(index, reply->url().toString(), MainWindow::DOWNLOAD_URL);
		QByteArray *bytes = new QByteArray(reply->readAll());
		QVariant fromCache = reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
		qDebug() << "page from cache?" << fromCache.toBool();
		QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
		ImageLabel *label = getImageLabel();
		label->setProperty("path", ui->listView->model()->data(index, MainWindow::PATH));
		label->setProperty("folderName", ui->listView->model()->data(index, MainWindow::FOLDER_NAME));
		label->setProperty("fileName", ui->listView->model()->data(index, MainWindow::FILE_NAME));
		//label->setToolTip(ui->listView->model()->data(index, MainWindow::PATH).toString());
		if(contentType == "image/gif"){
			QBuffer *buffer = new QBuffer(bytes);
			QMovie *movie = new QMovie(buffer);
			label->setMovie(movie);
			movie->start();
		}else{
			QPixmap pixmap;
			pixmap.loadFromData(*bytes);
			label->setPixmap(pixmap);
		}
		ui->scrollAreaWidgetContents->layout()->addWidget(label);
		reply->deleteLater();
	});
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiEx()
{
	//reset layout of scroll area
    FlowLayout *flowlayout = new FlowLayout();
    ui->scrollAreaWidgetContents->setLayout(flowlayout);
	//set width of left widget
    int width = this->size().width();
    int leftWidth = width * 0.3;
    int rightWidth = width - leftWidth;
    QList<int> list;
    list << leftWidth << rightWidth;
    ui->splitter->setSizes(list);
	model = new QStandardItemModel();
	ui->listView->setIconSize(QSize(50,50));
	ui->listView->setModel(model);
	ui->listView->setContextMenuPolicy(Qt::ActionsContextMenu);

	//init delete right-click menu for list view
	QAction *del = new QAction(tr("delete"), this);
	connect(del, &QAction::triggered, [this](){
		QModelIndexList modelIndexList = ui->listView->selectionModel()->selectedIndexes();
		foreach (QModelIndex modelIndex, modelIndexList){
			QString path = model->itemFromIndex(modelIndex)->data(MainWindow::PATH).toString();
			QUrl url("https://cloud-api.yandex.net/v1/disk/resources?path=/" + path);
			QNetworkRequest request = getRequest(url);
			QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
			connect(netManager, &QNetworkAccessManager::finished, [modelIndex, this](QNetworkReply *reply) {
				if (reply->error() != QNetworkReply::NoError)
				{
					ui->statusBar->showMessage(reply->errorString());
					return;
				}
				int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
				if(statusCode == 204){
					ui->statusBar->showMessage(tr("Delete Successfully"));
					model->removeRow(modelIndex.row());
				}else{
					ui->statusBar->showMessage(tr("Delete Failed"));
				}
			});
			netManager->deleteResource(request);
		}
	});

	//init move right-click menu for list view
	QAction *moveTo = new QAction(tr("moveTo..."), this);
	QMenu *subMenu = new QMenu(this);
	moveTo->setMenu(subMenu);
	connect(subMenu, &QMenu::triggered, [this](QAction *action){
		QModelIndexList modelIndexList = ui->listView->selectionModel()->selectedIndexes();
		foreach (QModelIndex modelIndex, modelIndexList){
			QString fromPath = model->itemFromIndex(modelIndex)->data(MainWindow::PATH).toString();
			QString fileName = model->itemFromIndex(modelIndex)->data(MainWindow::FILE_NAME).toString();
			QString toPath = action->text() + "/" + fileName;
			qDebug() << toPath;
			QUrl url("https://cloud-api.yandex.net/v1/disk/resources/move?from=/" + fromPath + "&path=/" + toPath);
			QNetworkRequest request = getRequest(url);
			request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
			QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
			connect(netManager, &QNetworkAccessManager::finished, [modelIndex, this](QNetworkReply *reply) {
				if (reply->error() != QNetworkReply::NoError)
				{
					ui->statusBar->showMessage(reply->errorString());
					return;
				}
				int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
				if(statusCode == 201){
					ui->statusBar->showMessage(tr("Move Successfully"));
					model->removeRow(modelIndex.row());
				}else{
					ui->statusBar->showMessage(tr("Move Failed"));
				}
			});
			QByteArray postData;
			netManager->post(request, postData);
		}
	});
	ui->listView->addAction(del);
	ui->listView->addAction(moveTo);

	aboutDialog = new About(this);
    refDialog = new Prerefrence(this);

	ui->actionSimple_Chinese->setData("Simple Chinese");
	ui->actionEnglish->setData("English");
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &folderName)
{
	if(folderName == "")
		return;
	QUrl url("https://cloud-api.yandex.net/v1/disk/resources?path=/" + folderName + "&limit=500");
	QNetworkRequest request = getRequest(url);
	QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
	connect(netManager, &QNetworkAccessManager::finished, [folderName, this](QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError)
		{
			ui->statusBar->showMessage(reply->errorString());
			return;
		}
		QByteArray *bytes = new QByteArray(reply->readAll());
		QJsonParseError json_error;
		QJsonDocument doc = QJsonDocument::fromJson(*bytes, &json_error);
		QJsonObject obj = doc.object();
		QJsonArray items = obj.value("_embedded").toObject().value("items").toArray();
		QList<QStandardItem*> filelist;
		for(int i = 0; i < items.size(); i++){
			QString fileName = items.at(i).toObject().value("name").toString();
			QString preview = items.at(i).toObject().value("preview").toString();
			QStandardItem *viewItem = getViewItem(fileName, preview);
			viewItem->setData(folderName + "/" + fileName, MainWindow::PATH);
			viewItem->setData(folderName, MainWindow::FOLDER_NAME);
			viewItem->setData(fileName, MainWindow::FILE_NAME);
			filelist << viewItem;
		}
		model->clear();
		model->appendColumn(filelist);
		reply->deleteLater();
	});
	netManager->get(request);
}

/**
 * make a new image label
 */
ImageLabel* MainWindow::getImageLabel()
{
	ImageLabel *label = new ImageLabel(this);
	label->setContextMenuPolicy(Qt::ActionsContextMenu);
	QAction *del = new QAction(tr("delete"), this);
	connect(del, &QAction::triggered, [label, this](){
		QString path = label->property("path").toString();
		QUrl url("https://cloud-api.yandex.net/v1/disk/resources?path=/" + path);
		QNetworkRequest request = getRequest(url);
		QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
		connect(netManager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
			if (reply->error() != QNetworkReply::NoError)
			{
				ui->statusBar->showMessage(reply->errorString());
				return;
			}
			int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			if(statusCode == 204){
				ui->statusBar->showMessage(tr("Delete Successfully"));
			}else{
				ui->statusBar->showMessage(tr("Delete Failed"));
			}
		});
		netManager->deleteResource(request);
	});
	QAction *moveTo = new QAction(tr("moveTo..."), this);
	QMenu *subMenu = new QMenu(this);
	moveTo->setMenu(subMenu);
	for(int i = 0; i < ui->comboBox->count(); i++){
		subMenu->addAction(new QAction(ui->comboBox->itemText(i), this));
	}
	connect(subMenu, &QMenu::triggered, [label, this](QAction *action){
		QString fromPath = label->property("path").toString();
		QString fileName = label->property("fileName").toString();
		QString toPath = action->text() + "/" + fileName;
		qDebug() << toPath;
		QUrl url("https://cloud-api.yandex.net/v1/disk/resources/move?from=/" + fromPath + "&path=/" + toPath);
		QNetworkRequest request = getRequest(url);
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
		QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
		connect(netManager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
			if (reply->error() != QNetworkReply::NoError)
			{
				ui->statusBar->showMessage(reply->errorString());
				return;
			}
			int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
			if(statusCode == 201){
				ui->statusBar->showMessage(tr("Move Successfully"));
			}else{
				ui->statusBar->showMessage(tr("Move Failed"));
			}
		});
		QByteArray postData;
		netManager->post(request, postData);
	});
	moveTo->setMenu(subMenu);
	label->addAction(del);
	label->addAction(moveTo);
	return label;
}

void MainWindow::downloadFile(const QModelIndex &index, const QUrl &url)
{
	QNetworkRequest request = getRequest(url);
	QNetworkReply *reply = downloadManager->get(request);
	replyMap[reply] = index;
	connect(reply, &QNetworkReply::downloadProgress, [this](qint64 bytesReceived, qint64 bytesTotal) {
		//qDebug() << bytesReceived << ":" << bytesTotal;
		if(bytesTotal <= 0) return;
		int progress = 1.0 * bytesReceived / bytesTotal * 100;
		if(progress > 100)progress = 100;
		ui->statusBar->showMessage(QString("%1%").arg(progress));
	});
}

void MainWindow::connectToDisk()
{
    QUrl url("https://cloud-api.yandex.net/v1/disk/resources?path=/");
	QNetworkRequest request = getRequest(url);
	QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
	connect(netManager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError)
		{
			ui->statusBar->showMessage(reply->errorString());
			return;
		}
		ui->comboBox->clear();
		QJsonParseError json_error;
		QJsonDocument doc = QJsonDocument::fromJson(reply->readAll(), &json_error);
		QJsonObject obj = doc.object();
		QJsonArray items = obj.value("_embedded").toObject().value("items").toArray();

		QMenu *subMenu = ui->listView->actions().at(1)->menu();
		subMenu->clear();
		for(int i = 0; i < items.size(); i++){
			QString name = items.at(i).toObject().value("name").toString();
			ui->comboBox->addItem(name);
			subMenu->addAction(new QAction(name, this));
		}
//		ui->listView->actions().at(1)->menu()->deleteLater();
//		ui->listView->actions().at(1)->setMenu(subMenu);
		reply->deleteLater();
	});
	netManager->get(request);
}

void MainWindow::changeToken()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("New Token"),
                                         tr("Token:"), QLineEdit::Normal,
                                         token, &ok);
    if (ok && !text.isEmpty()){
        this->token = text.toLatin1();
        QSettings *config = new QSettings("config.ini", QSettings::IniFormat);
        config->setValue("token", text);
    }
}

void MainWindow::exit()
{
    qApp->exit(0);
}

void MainWindow::getAll()
{
    QLayoutItem *child;
    while ((child = ui->scrollAreaWidgetContents->layout()->takeAt(0)) != 0) {
        delete child->widget();
		delete child;
    }
    int count = ui->listView->model()->rowCount();
    for(int i = 0; i < count; i++){
        QModelIndex index = ui->listView->model()->index(i, 0);
		this->on_listView_doubleClicked(index);
    }
}

void MainWindow::switchSideBar()
{
    int width = this->size().width();
    QList<int> list = ui->splitter->sizes();
    if(list.at(0) == 0){
        list.replace(0, width*0.3);
        list.replace(1, width - width * 0.3);
		ui->actionSideBar->setChecked(true);
    }else{
        list.replace(0, 0);
        list.replace(1, width);
		ui->actionSideBar->setChecked(false);
    }
	ui->splitter->setSizes(list);
}

void MainWindow::about()
{
    aboutDialog->show();
}

void MainWindow::prerefrence()
{
    refDialog->show();
}

void MainWindow::switchLang(QAction *action)
{
	QByteArray text = action->data().toByteArray();
	if(text == "English"){
		translator.load("translations/en_US.qm");
		ui->actionEnglish->setChecked(true);
		ui->actionSimple_Chinese->setChecked(false);
	}
	if(text == "Simple Chinese"){
		translator.load("translations/zh_CN.qm");
		ui->actionSimple_Chinese->setChecked(true);
		ui->actionEnglish->setChecked(false);
	}
}

void MainWindow::newFolder()
{
	bool ok;
	QString folderName = QInputDialog::getText(this, tr("New Folder"),
										 tr("Folder Name:"), QLineEdit::Normal,
										 QString(""), &ok);
	if (!ok || folderName.isEmpty()){
		ui->statusBar->showMessage(tr("No folder name specified"));
		return;
	}
	QUrl url("https://cloud-api.yandex.net/v1/disk/resources?path=/" + folderName);
	QNetworkRequest request = getRequest(url);
	request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
	QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
	connect(netManager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError)
		{
			ui->statusBar->showMessage(reply->errorString());
			return;
		}
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if(statusCode == 201){
			ui->statusBar->showMessage(tr("Create Successfully"));
			connectToDisk();
		}else{
			ui->statusBar->showMessage(tr("Create Failed"));
		}
	});
	QByteArray postData;
	netManager->put(request, postData);
}

void MainWindow::uploadFile()
{

}

void MainWindow::deleteFolder()
{
	QString folderName = ui->comboBox->currentText();
	if(folderName == ""){
		ui->statusBar->showMessage(tr("No folder selected"));
		return;
	}
	int fileCount = ui->listView->model()->rowCount();
	if(fileCount > 0){
		ui->statusBar->showMessage(tr("Non-empty folder can't be deleted"));
		return;
	}
	QUrl url("https://cloud-api.yandex.net/v1/disk/resources?path=/" + folderName);
	QNetworkRequest request = getRequest(url);
	QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
	connect(netManager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError)
		{
			ui->statusBar->showMessage(reply->errorString());
			return;
		}
		int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
		if(statusCode == 204){
			ui->statusBar->showMessage(tr("Delete Successfully"));
			connectToDisk();
		}else{
			ui->statusBar->showMessage(tr("Delete Failed"));
		}
	});
	netManager->deleteResource(request);
}

void MainWindow::refreshFolder()
{
	QString folderName = ui->comboBox->currentText();
	if(folderName != "")
		this->on_comboBox_currentIndexChanged(folderName);
}

QNetworkRequest MainWindow::getRequest(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
    request.setRawHeader("Authorization", token);
    QSslConfiguration conf = request.sslConfiguration();
    conf.setPeerVerifyMode(QSslSocket::VerifyNone);
    conf.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(conf);;
	return request;
}

QStandardItem *MainWindow::getViewItem(const QString &name, const QString &preview)
{
	QStandardItem *viewItem = new QStandardItem(name);
	QUrl url(preview);
	QNetworkRequest request = getRequest(url);
	QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
	netManager->setCache(diskCache);
	connect(netManager, &QNetworkAccessManager::finished, [viewItem, this](QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError)
		{
			ui->statusBar->showMessage(reply->errorString());
			return;
		}
		QVariant fromCache = reply->attribute(QNetworkRequest::SourceIsFromCacheAttribute);
		qDebug() << "page from cache?" << fromCache.toBool();
		QPixmap pixmap;
		pixmap.loadFromData(reply->readAll());
		viewItem->setIcon(QIcon(pixmap));
		reply->deleteLater();
	});
	netManager->get(request);
	return viewItem;
}

void MainWindow::changeEvent(QEvent *e)
{
	if(e->type() == QEvent::LanguageChange)
	{
		ui->retranslateUi(this);
		aboutDialog->retranslateUi();
		QLayout *layout = ui->scrollAreaWidgetContents->layout();
		for(int i = 0; i < layout->count(); i++){
			ImageLabel *label = static_cast<ImageLabel*>(layout->itemAt(i)->widget());
			label->actions().at(0)->setText(tr("delete"));
			label->actions().at(1)->setText(tr("moveTo..."));
		}
		ui->listView->actions().at(0)->setText(tr("delete"));
		ui->listView->actions().at(1)->setText(tr("moveTo..."));
	}
	QMainWindow::changeEvent(e);
}

void MainWindow::on_listView_doubleClicked(const QModelIndex &index)
{
	QVariant tooltip = ui->listView->model()->data(index, MainWindow::DOWNLOAD_URL);
	if(tooltip.isValid()){
		QUrl url(tooltip.toString());
		downloadFile(index, url);
		return;
	}
	QString name = ui->listView->model()->data(index).toString();
	QUrl url("https://cloud-api.yandex.net/v1/disk/resources/download?path=/"+ ui->comboBox->currentText() + "/" + name);
	QNetworkRequest request = getRequest(url);
	QNetworkAccessManager *netManager = new QNetworkAccessManager(this);
	connect(netManager, &QNetworkAccessManager::finished, [index, this](QNetworkReply *reply) {
		if (reply->error() != QNetworkReply::NoError)
		{
			ui->statusBar->showMessage(reply->errorString());
			return;
		}
		QByteArray *bytes = new QByteArray(reply->readAll());
		QJsonParseError json_error;
		QJsonDocument doc = QJsonDocument::fromJson(*bytes, &json_error);
		QJsonObject obj = doc.object();
		QString href = obj.value("href").toString();
		// cache url
		ui->listView->model()->setData(index, href, MainWindow::DOWNLOAD_URL);
		QUrl url(href);
		downloadFile(index, url);
		reply->deleteLater();
	});
	netManager->get(request);
}
