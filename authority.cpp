#include "authority.h"
#include "ui_authority.h"

Authority::Authority(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Authority)
{
    ui->setupUi(this);
    netManager = ui->webView->page()->networkAccessManager();
    connect(netManager, &QNetworkAccessManager::finished, [this](QNetworkReply *reply) {
        QString scheme = reply->url().scheme();
        if(scheme == "myapp"){
            this->hide();
            QString url = reply->url().url();
            QString token = url.split("#").at(1).split("&").at(0).split("=").at(1);
            QSettings *config = new QSettings("config.ini", QSettings::IniFormat);
            if(config->isWritable()){
                config->setValue("token", "OAuth " + token);
                emit success();
            }else
                QMessageBox::about(NULL, "Infomation", "can not edit config.ini file.");
        }
    });
    ui->webView->load(QUrl("https://oauth.yandex.com/authorize?response_type=token&client_id=45a15bca743c438e9adb2e2716194c07&client_secret=2ae9f957eab3406da166c9221b2a7cad"));

}

Authority::~Authority()
{
    delete ui;
}
