#include "octaldialog.h"
#include "ui_octaldialog.h"

OctalDialog::OctalDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OctalDialog),
    m_octalValStr("0000")
{
    ui->setupUi(this);
    ui->label->setText(m_octalValStr.c_str());
    ui->rb0->setChecked(true);
    ui->rb0_8->setChecked(true);
    ui->rb0_9->setChecked(true);
    connect(ui->rb0, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb1, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb2, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb3, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb4, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb5, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb6, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb7, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb0_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb1_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb2_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb3_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb4_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb5_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb6_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb7_8, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb0_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb1_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb2_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb3_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb4_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb5_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb6_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
    connect(ui->rb7_9, SIGNAL(clicked(bool)), this, SLOT(on_radio_btn_clicked()));
}

OctalDialog::~OctalDialog()
{
    delete ui;
}

const string& OctalDialog::getOctalValStr() const
{
    return m_octalValStr;
}

void OctalDialog::on_radio_btn_clicked()
{
    if(ui->rb0->isChecked())
    {
        m_octalValStr[1] = '0';
    }
    else if(ui->rb1->isChecked())
    {
        m_octalValStr[1] = '1';
    }
    else if(ui->rb2->isChecked())
    {
        m_octalValStr[1] = '2';
    }
    else if(ui->rb3->isChecked())
    {
        m_octalValStr[1] = '3';
    }
    else if(ui->rb4->isChecked())
    {
        m_octalValStr[1] = '4';
    }
    else if(ui->rb5->isChecked())
    {
        m_octalValStr[1] = '5';
    }
    else if(ui->rb6->isChecked())
    {
        m_octalValStr[1] = '6';
    }
    else if(ui->rb7->isChecked())
    {
        m_octalValStr[1] = '7';
    }

    if(ui->rb0_8->isChecked())
    {
        m_octalValStr[2] = '0';
    }
    else if(ui->rb1_8->isChecked())
    {
        m_octalValStr[2] = '1';
    }
    else if(ui->rb2_8->isChecked())
    {
        m_octalValStr[2] = '2';
    }
    else if(ui->rb3_8->isChecked())
    {
        m_octalValStr[2] = '3';
    }
    else if(ui->rb4_8->isChecked())
    {
        m_octalValStr[2] = '4';
    }
    else if(ui->rb5_8->isChecked())
    {
        m_octalValStr[2] = '5';
    }
    else if(ui->rb6_8->isChecked())
    {
        m_octalValStr[2] = '6';
    }
    else if(ui->rb7_8->isChecked())
    {
        m_octalValStr[2] = '7';
    }

    if(ui->rb0_9->isChecked())
    {
        m_octalValStr[3] = '0';
    }
    else if(ui->rb1_9->isChecked())
    {
        m_octalValStr[3] = '1';
    }
    else if(ui->rb2_9->isChecked())
    {
        m_octalValStr[3] = '2';
    }
    else if(ui->rb3_9->isChecked())
    {
        m_octalValStr[3] = '3';
    }
    else if(ui->rb4_9->isChecked())
    {
        m_octalValStr[3] = '4';
    }
    else if(ui->rb5_9->isChecked())
    {
        m_octalValStr[3] = '5';
    }
    else if(ui->rb6_9->isChecked())
    {
        m_octalValStr[3] = '6';
    }
    else if(ui->rb7_9->isChecked())
    {
        m_octalValStr[3] = '7';
    }
    ui->label->setText(m_octalValStr.c_str());
}
