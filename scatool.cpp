#include "scatool.h"
#include <QListWidget>
#include <QStringList>
#include <QObject>
#include <QtGlobal>
#include <algorithm>
#include <QThread>
#include "mainwindow.h"
#include "synchro.h"

QVector<Curve*> * ScaTool::curves = 0;
QDockWidget *ScaTool::dockcurves = 0;
CurveListWidget *ScaTool::curve_table = 0;
SynchroDialog *ScaTool::synchrodialog = 0;
Attackwindow *ScaTool::attackdialog = 0;
ChartView *ScaTool::main_plot = 0;
QtCharts::QChartView *ScaTool::down_plot = 0;
QStatusBar * ScaTool::statusbar = 0;
CurveTableModel * ScaTool::curve_table_model = 0;
int ScaTool::global_type = 0;


Curve* ScaTool::getCurveFromDisplaySerie(QLineSeries * serie)
{
    for (QVector<Curve*>::iterator it = ScaTool::curves->begin();
         it != ScaTool::curves->end(); ++it) {

        Curve *curve = *it;

        if (curve->displayseries == serie)
        {
            return curve;

        }
    }
    return 0;
}
