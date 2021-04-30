#include "myTimer.h"

MyTimer::MyTimer() { // constructor
    timer = new QTimer(this);
    label = new QLabel("Time (secs): 0");
    counter = 0;
    connect(timer, SIGNAL(timeout()), this, SLOT(MyTimerSlot()));
    timer->start(1000);
}

void MyTimer::MyTimerSlot() {
    counter += 1;
    label->setText("Time (secs): "+ QString::number(this->counter));
    if(counter>=180){           // time limit exceeded
        this->timer->stop();    // stop the timer
        timerunout();           // send necessary signal
    }
}

// if a new game has started, reset the timer
void MyTimer:: reset(){
    this->timer->stop();
    this->counter=0;
    this->timer->start(1000);

}


