#include "parrescalc.h"
#include "ui_parrescalc.h"
//#include <QtDebug>
#include <QVarLengthArray>
#include <math.h>
#include <QDesktopServices>
#include <QUrl>

const double one[] =  { 10.0, 10.2, 10.5, 10.7, 11.0,
                        11.3, 11.5, 11.8, 12.1, 12.4,
                        12.7, 13.0, 13.3, 13.7, 14.0,
                        14.3, 14.7, 15.0, 15.4, 15.8,
                        16.2, 16.5, 16.9, 17.4, 17.8,
                        18.2, 18.7, 19.1, 19.6, 20.0,
                        20.5, 21.0, 21.5, 22.1, 22.6,
                        23.2, 23.7, 24.3, 24.9, 25.5,
                        26.1, 26.7, 27.4, 28.0, 28.7,
                        29.4, 30.1, 30.9, 31.6, 32.4,
                        33.2, 34.0, 34.8, 35.7, 36.5,
                        37.4, 38.3, 39.2, 40.2, 41.2,
                        42.2, 43.2, 44.2, 45.3, 46.4,
                        47.5, 48.7, 49.9, 51.1, 52.3,
                        53.6, 54.9, 56.2, 57.6, 59.0,
                        60.4, 61.9, 63.4, 64.9, 66.5,
                        68.1, 69.8, 71.5, 73.2, 75.0,
                        76.8, 78.7, 80.6, 82.5, 84.5,
                        86.6, 88.7, 90.9, 93.1, 95.3,
                        97.6 };
const double five[] = { 10.0, 11.0, 12.0, 13.0, 15.0,
                        16.0, 18.0, 20.0, 22.0, 24.0,
                        27.0, 30.0, 33.0, 36.0, 39.0,
                        43.0, 47.0, 51.0, 56.0, 62.0,
                        68.0, 75.0, 82.0, 91.0 };

const char suffix[] = { 0, 'k', 'M', 'G' };

ParResCalc::ParResCalc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ParResCalc)
{
    ui->setupUi(this);
}

ParResCalc::~ParResCalc()
{
    delete ui;
}

void ParResCalc::on_btnCalc_clicked()
{
    //Main processing
    double neededR = ui->spnResNeeded->value();
    int decade = 0;
    while(neededR > 50.0)
    {
        neededR /= 10.0;
        decade++;
    }
    if(neededR < 10.0)
    {
        neededR *= 10.0;
        decade--;
    }
    double tol = 0.0;
    switch(ui->cmbPercent->currentIndex())
    {
    case 0://1%
        tol = 0.01;

        break;
    case 1://5%
        tol = 0.05;
        break;
    }
    QVarLengthArray<double> r;
    if(tol == 0.01)
        r.append(one,96);
    else if(tol == 0.05)
        r.append(five,24);
    else
    {
        //qDebug() << "ERROR: invalid tol in r assign" << endl;
        return;
    }

    double lowlim = (1-tol)*neededR;
    double hilim = (1+tol)*neededR;

    double best[] = {0,0,0};
    double cur = 0.0;

    //iterate through each decade
    for(int dec=1;dec < 5;dec++)
    {
        //go through the list of resistors
        for(int i=0;i<r.size();i++)
        {
            for(int j=i;j<r.size();j++)
            {
                //par res calculation
                cur = (r[i] * r[j]) / (r[i] + r[j]);
                if((cur>=lowlim)&&(cur<=hilim))
                {
                    if(qAbs(neededR - cur) < qAbs(neededR - best[0]))
                    {
                        // new value better
                        best[0] = cur;
                        best[1] = r[i];
                        best[2] = r[j];
                    }
                }
            }
        }
        if(best[0] != 0.0) //found a suitable pair!
            break;
        //double size of r with new decade
        //qDebug() << "Doubling r, adding next decade " << dec;
        r.reserve(r.size()*2);
        int rsize = r.size();
        double temp = pow(10,dec);
        for(int i=0;i<rsize;i++)
        {
            r.append(r[i] * temp);
        }
    }
    //convert the resistances back to the right decade
    best[0] *= pow(10,decade);
    best[1] *= pow(10,decade);
    best[2] *= pow(10,decade);
    neededR *= pow(10,decade);//for error output only
    double error = (best[0]/neededR - 1)*100;

    //qDebug() << "Best value after all iterations: " << best[0] << " " << best[1] << " " << best[2] << endl
    //         << "Error in best: " << error << "%";

    //get a nice string for outputting
    QString strR1, strR2, strActual;
    const char *r1suf = &suffix[ui->cmbSuffix->currentIndex()];
    const char *r2suf = &suffix[ui->cmbSuffix->currentIndex()];
    const char *actsuf = &suffix[ui->cmbSuffix->currentIndex()];

    if(best[0]<1000.00)
    {
        strActual.append(QString("%1%2").arg(best[0],0,'f',3).arg(*actsuf));
    }
    else
    {
        actsuf++;
        strActual.append(QString("%1%2").arg(best[0]/100,0,'f',3).arg(*actsuf));
    }
    if(best[1]<1000.00)
    {
        strR1.append(QString("%1%2").arg(best[1],0,'g',4).arg(*r1suf));
    }
    else
    {
        r1suf++;
        strR1.append(QString("%1%2").arg(best[1]/100,0,'g',4).arg(*r1suf));
    }
    if(best[2]<1000.00)
    {
        strR2.append(QString("%1%2").arg(best[2],0,'g',4).arg(*r2suf));
    }
    else
    {
        r2suf++;
        strR2.append(QString("%1%2").arg(best[2]/100,0,'g',4).arg(*r2suf));
    }
    ui->txtR1->setText(strR1);
    ui->txtR2->setText(strR2);
    ui->txtActual->setText(strActual);
    ui->txtDiff->setText(QString("%1%").arg(error,0,'f',3));
}


void ParResCalc::on_btnSearchR1_clicked()
{
    //go to a specially formated URL for a neat digikey search feature
    QString strR1, strTol;
    const QString digikey = "http://search.digikey.com/scripts/DkSearch/dksus.dll?vendor=0&keywords=chip+resistor+";
    switch(ui->cmbPercent->currentIndex())
    {
    case 0://1%
        strTol.append("1%");
        break;
    case 1://5%
        strTol.append("5%");
        break;
    }
    strR1.append(digikey).append(strTol).append('+');
    strR1.append(ui->txtR1->text());
    strR1.append("&stock=1");
    QDesktopServices::openUrl(QUrl(strR1));
}

void ParResCalc::on_btnSearchR2_clicked()
{
    //go to a specially formated URL for a neat digikey search feature
    QString strR2, strTol;
    const QString digikey = "http://search.digikey.com/scripts/DkSearch/dksus.dll?vendor=0&keywords=chip+resistor+";
    switch(ui->cmbPercent->currentIndex())
    {
    case 0://1%
        strTol.append("1%");
        break;
    case 1://5%
        strTol.append("5%");
        break;
    }
    strR2.append(digikey).append(strTol).append('+');
    strR2.append(ui->txtR2->text());
    strR2.append("&stock=1");
    QDesktopServices::openUrl(QUrl(strR2));
}

