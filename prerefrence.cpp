#include "prerefrence.h"
#include "ui_prerefrence.h"

Prerefrence::Prerefrence(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Prerefrence)
{
    ui->setupUi(this);
}

void Prerefrence::accept(){
    bool sizePolicy =  ui->checkBox->isChecked();
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
