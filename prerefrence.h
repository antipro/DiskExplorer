#ifndef PREREFRENCE_H
#define PREREFRENCE_H

#include <QDialog>
#include <QSettings>

namespace Ui {
class Prerefrence;
}

class Prerefrence : public QDialog
{
    Q_OBJECT

public:
    explicit Prerefrence(QWidget *parent = 0);
    ~Prerefrence();
public Q_SLOTS:
    void accept();
private:
    Ui::Prerefrence *ui;
};

#endif // PREREFRENCE_H
