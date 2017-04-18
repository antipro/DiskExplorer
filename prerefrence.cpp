#include "prerefrence.h"
#include "ui_prerefrence.h"

Prerefrence::Prerefrence(QSettings *config, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Prerefrence)
{
    ui->setupUi(this);
    this->config = config;
    int sizePolicy = config->value("size_policy", QVariant(0)).toInt();
    qDebug() << sizePolicy;
    switch(sizePolicy){
    case 0:{
        ui->radio1->setChecked(true);
    }break;
    case 1:{
        ui->radio2->setChecked(true);
    }break;
    case 2:{
        ui->radio3->setChecked(true);
    }break;
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
        sizePolicy = 2;

    int maxSize = ui->spinBox->value();
    if(config->isWritable()){
        config->setValue("size_policy", sizePolicy);
        config->setValue("max_size", maxSize);
    }
    QDialog::accept();
}

Prerefrence::~Prerefrence()
{
    delete ui;
}
