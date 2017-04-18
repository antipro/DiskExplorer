#ifndef PREREFRENCE_H
#define PREREFRENCE_H

#include <QDialog>
#include <QSettings>
#include <QDebug>

namespace Ui {
class Prerefrence;
}

class Prerefrence : public QDialog
{
    Q_OBJECT

public:
    explicit Prerefrence(QSettings*, QWidget *parent = 0);
    ~Prerefrence();
public Q_SLOTS:
    void accept();
private:
    Ui::Prerefrence *ui;
    QSettings *config;
};

#endif // PREREFRENCE_H
