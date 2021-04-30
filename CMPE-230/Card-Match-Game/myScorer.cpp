#include "myScorer.h"

myscorer::myscorer(){ // constructor
    label = new QLabel("Score: 0");
    counter = 0;
}

void myscorer::increaseScore(){     // if a match has been made, upload the score
    counter += 1;
    label->setText("Score: "+ QString::number(this->counter));
}

void myscorer::reset(){             // if a match occured, upload the score
    this->counter = 0;
    this->label->setText("Score: "+ QString::number(this->counter));

}
