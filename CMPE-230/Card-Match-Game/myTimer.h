#ifndef MYTIMER_H
#define MYTIMER_H
#include <QTimer>
#include <QLabel>
#include <QMessageBox>

// Code Source: PS12 of CMPE230.
// timer counts the seconds in cumulative manner from the beginning of the game
// performs necessary signals when timeout occurs

class MyTimer : public QObject {
    Q_OBJECT

public:
    MyTimer();
    QTimer *timer;
    QLabel *label;
    int counter;

public slots:
    void MyTimerSlot();
    void reset();
signals:
    void timerunout();
};

#endif // MYTIMER_H
