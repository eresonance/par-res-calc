/*  Copyright (c) 2010, Devin Linnington
    All rights reserved.
    eresonance@gmail.com

    See LICENSE.txt for rights of use
*/

#include <QtGui/QApplication>
#include "parrescalc.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ParResCalc w;
    w.show();

    return a.exec();
}
