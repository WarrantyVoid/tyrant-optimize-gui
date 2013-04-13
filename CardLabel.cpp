#include "CardLabel.h"
#include "ui_CardLabel.h"

CardLabel::CardLabel(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CardLabel)
{
    ui->setupUi(this);
}

CardLabel::~CardLabel()
{
    delete ui;
}
