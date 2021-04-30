#ifndef MYGRID_H
#define MYGRID_H
#include <QGridLayout>
#include <QMessageBox>
#include <QTimer>
#include <card.h>
#include <QPalette>
#include <myTimer.h>

// mygrid is the game area, containing a grid of Cards
class MyGrid : public QGridLayout{
    Q_OBJECT

public:
    MyGrid(QTimer* timer);
    QTimer* timer;
    int matchedpairs;
    int opencards;

public slots:
    void endgame();
    void checkPaired();
    void createNew();

signals:
    void paired();
};


#endif // MYGRID_H
