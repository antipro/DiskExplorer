#ifndef AUTHORITY_H
#define AUTHORITY_H

#include <QDialog>
#include <QNetworkReply>
#include <QNetworkDiskCache>
#include <QNetworkRequest>
#include <QSettings>
#include <QMessageBox>

namespace Ui {
class Authority;
}

class Authority : public QDialog
{
    Q_OBJECT

public:
    explicit Authority(QWidget *parent = 0);
    ~Authority();

Q_SIGNALS:
    void success();

private:
    Ui::Authority *ui;
    QNetworkAccessManager *netManager;
};

#endif // AUTHORITY_H
