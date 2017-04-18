#include "prerefrence.h"
#include "ui_prerefrence.h"

Prerefrence::Prerefrence(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Prerefrence)
{
    ui->setupUi(this);
    QSettings *config = new QSettings("config.ini", QSettings::IniFormat);
    int sizePolicy = config->value("size_policy", QVariant(0)).toInt();
    switch(sizePolicy){
    case 0:
        ui->radio1->setChecked(true);break;
    case 1:
        ui->radio2->setChecked(true);break;
    case 2:
        ui->radio3->setChecked(true);
    }

    int maxSize = config->value("max_size", QVariant(300)).toInt();
    ui->spinBox->setValue(maxSize);
}

void Prerefrence::accept(){
    int sizePolicy = 0;
    if(ui->radio1->isChecked())
        sizePolicy = 0;
    if(ui->radio2->isChecked())
        sizePolicy = 1;
    if(ui->radio3->isChecked())
        sizePolicy = 3;

    int maxSize = ui->spinBox->value();
    QSettings *config = new QSettings("config.ini", QSettings::IniFormat);
    if(config->isWritable()){
        config->setValue("size_policy", sizePolicy);
        config->setValue("max_size", maxSize);
    }
    delete config;
    QDialog::accept();
}

Prerefrence::~Prerefrence()
{
    delete ui;
}
