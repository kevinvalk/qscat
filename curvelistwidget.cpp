#include "curvelistwidget.h"
#include "ui_curvelistwidget.h"
#include "scatool.h"
#include <QTableWidgetItem>
#include <QValueAxis>
#include <QColorDialog>

CurveListWidget::CurveListWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurveListWidget)
{
    ui->setupUi(this);
    this->firstDisplayed = true;

    ui->type_box->addItem("float32");
    ui->type_box->addItem("uint32");
    ui->type_box->addItem("int32");
    ui->type_box->addItem("uint16");
    ui->type_box->addItem("int16");
    ui->type_box->addItem("uint8");
    ui->type_box->addItem("int8");
    connect(ui->type_box,SIGNAL(currentIndexChanged(int)),this,SLOT(global_type_changed(int)));

}
CurveListWidget::~CurveListWidget()
{
    delete ui;
}

void CurveListWidget::clear()
{
    int rowCount = ui->table_curve->rowCount();
    for(int i = 0; i < rowCount; i++)
    {
        ui->table_curve->removeRow(i);
    }
    ui->table_curve->setRowCount(0);
}

void CurveListWidget::addCurve(Curve *curve)
{
    int rowidx = ui->table_curve->rowCount();
    int colidx = 0;
    ui->table_curve->insertRow(rowidx);

    //Nb
    ui->table_curve->setItem(rowidx,colidx,new QTableWidgetItem(QString::number(rowidx)));
    ui->table_curve->item(rowidx,colidx)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);

    //Color
    colidx ++;
    QPushButton *colorbtn = new QPushButton(this);
    curve->setcolorbtn(colorbtn);
    ui->table_curve->setCellWidget(rowidx,colidx,colorbtn);


    //Name
    colidx ++;
    ui->table_curve->setItem(rowidx,colidx,new QTableWidgetItem(curve->cname));
    ui->table_curve->item(rowidx,colidx)->setToolTip(curve->fn);
    ui->table_curve->item(rowidx,colidx)->setFlags(Qt::ItemIsEnabled|Qt::ItemIsSelectable);

    //Type
    colidx++;
    QComboBox *cmbbox = new QComboBox(this);
    cmbbox->addItem("float32");
    cmbbox->addItem("uint32");
    cmbbox->addItem("int32");
    cmbbox->addItem("uint16");
    cmbbox->addItem("int16");
    cmbbox->addItem("uint8");
    cmbbox->addItem("int8");
    curve->settypecmbbox(cmbbox);
    ui->table_curve->setCellWidget(rowidx,colidx,cmbbox);

    //Offset
    colidx++;
    ui->table_curve->setItem(rowidx,colidx,new QTableWidgetItem(QString::number(curve-> xoffset)));
    ui->table_curve->item(rowidx,colidx)->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable|Qt::ItemIsSelectable);

    //Displayed
    colidx++;
    QCheckBox *chkbox = new QCheckBox(this);
    curve->setchkbox(chkbox);
    ui->table_curve->setCellWidget(rowidx,colidx,chkbox);

    // Handler
    connect(colorbtn,&QPushButton::pressed,curve,&Curve::colorbtn_pressed);
    connect(chkbox,&QCheckBox::toggled,curve,&Curve::chkbox_toggled);
    connect(cmbbox,SIGNAL(currentIndexChanged(int)),curve,SLOT(curve_type_changed(int)));
    connect(curve,&Curve::shifted,this,&CurveListWidget::updateshiftvalue);
    connect(ui->table_curve,&QTableWidget::cellPressed,this,&CurveListWidget::rowselected);
}

Curve * CurveListWidget::getSelectedCurve()
{

    QList<QTableWidgetItem *> itemlist = ui->table_curve->selectedItems();

    if (itemlist.length() < 1)
        return 0;
    else
    {
        int rowidx = itemlist.first()->row();
        return ScaTool::getCurveByName(ui->table_curve->item(rowidx,2)->text());
    }
}

QList<Curve *> CurveListWidget::getSelectedCurves()
{

    QList<QTableWidgetItem *> itemlist = ui->table_curve->selectedItems();

    QList<Curve*> clist = QList<Curve*>();
    for (int i = 0 ; i < itemlist.length() ; i ++)
    {
        int rowidx = itemlist.at(i)->row();
        clist.append(ScaTool::getCurveByName(ui->table_curve->item(rowidx,2)->text()));
    }
    return clist;
}

void CurveListWidget::global_type_changed(int type)
{
    Curve * curve;
    for(int rowidx = 0; rowidx < ui->table_curve->rowCount(); rowidx++)
    {
        curve = ScaTool::getCurveByName(ui->table_curve->item(rowidx,2)->text());
        if (curve == 0)
            return;
        curve->type = type;

        curve->type_cmbbox->setCurrentIndex(type);

        curve->updateDisplaySeries();
    }
}

void CurveListWidget::on_clearall_pressed()
{

    qDeleteAll(ScaTool::synchrodialog->synchropasses.begin(),ScaTool::synchrodialog->synchropasses.end());

    this->clear();

    if (ScaTool::curves->length() > 0)
    {
        qDeleteAll(ScaTool::curves->begin(),ScaTool::curves->end());
        ScaTool::curves->clear();
    }
    if (ScaTool::dockcurves->isHidden())
        ScaTool::dockcurves->show();
    for (int i = 0 ; i < ScaTool::main_plot->chart()->axes(Qt::Horizontal).length(); i ++)
        ScaTool::main_plot->chart()->removeAxis(ScaTool::main_plot->chart()->axes().at(i));
    for (int i = 0 ; i < ScaTool::main_plot->chart()->axes(Qt::Vertical).length(); i ++)
        ScaTool::main_plot->chart()->removeAxis(ScaTool::main_plot->chart()->axes().at(i));
    ScaTool::curve_table->firstDisplayed = true;

}

void CurveListWidget::on_displayall_pressed()
{
    Curve *curve;
    for(int i = 0; i < ui->table_curve->rowCount() ; i++)
    {
        curve = ScaTool::getCurveByName(ui->table_curve->item(i,2)->text());
        curve->chkbox->setChecked(true);
    }
}

void CurveListWidget::on_displayoff_pressed()
{
    Curve *curve;
    for(int i = 0; i < ui->table_curve->rowCount() ; i++)
    {
        curve = ScaTool::getCurveByName(ui->table_curve->item(i,2)->text());
        curve->chkbox->setChecked(false);
    }
}

void CurveListWidget::on_delete_2_pressed()
{
    int rowidx;
    QList<QTableWidgetItem *> itemlist = ui->table_curve->selectedItems();

    for(int i = 0 ; i < itemlist.length(); i++)
    {
        rowidx = itemlist.at(i)->row();
        Curve *c = ScaTool::getCurveByName(ui->table_curve->item(rowidx,2)->text());
        if (c->displayed)
        {
            c->chkbox->setChecked(false);
            ScaTool::main_plot->chart()->removeSeries(c->getDisplaySeries());
        }
        ui->table_curve->removeRow(rowidx);

        Q_ASSERT(ScaTool::curves->removeOne(c));

        delete c;
    }
}

void CurveListWidget::updateshiftvalue()
{
    Curve *c = (Curve*)sender();
    for (int i = 0; i < ui->table_curve->rowCount() ; i++)
    {
        if (ui->table_curve->item(i,2)->text() == c->cname)
        {
            ui->table_curve->item(i,4)->setText(QString::number(c->xoffset));
        }
    }

}

void CurveListWidget::rowselected(int row, int column)
{
    /*Curve *c = ScaTool::getCurveByName(ui->table_curve->item(row,2)->text());
    if (c->displayed)
        printf("TODO : to bring front");*/
}
