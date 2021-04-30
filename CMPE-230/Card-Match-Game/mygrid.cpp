#include "mygrid.h"
#include "card.h"
#include <QThread>
#include <random>

MyGrid::MyGrid(QTimer *timer) : QGridLayout(){ // constructor
    this->timer = timer;
    this->matchedpairs=0;
    this->opencards=0;
}

// if time limit is exceeded end the game and disable the cards, show lost message
void MyGrid::endgame(){
    QMessageBox msgBox;
    msgBox.setText("You lost!");
    msgBox.exec();
    msgBox.setStandardButtons(QMessageBox::Cancel);
    for (int i = 0; i < this->count(); ++i){
        Card *widget = qobject_cast<Card*> (this->itemAt(i)->widget());
        widget->setEnabled(false);
    }
}

// create a new game grid
void MyGrid::createNew(){
    this->opencards=0;
    this->matchedpairs=0;
    // The words are the characters of The Office, they are shuffled.
    std::vector<QString> words {"michael","michael","pam","pam","jim","jim","dwight","dwight","oscar","oscar","angela","angela",
                      "phyllis","phyllis","stanley","stanley","kevin","kevin","ryan","ryan","toby","toby",
                      "andy","andy","creed","creed","meredith","meredith","kelly","kelly"};
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle (words.begin(), words.end(), std::default_random_engine(seed));

    for(int row=0; row<5; row++){
        for(int col=0; col<6; col++){
            QString current= words[row*6+col];                      // select a word from the shuffled array
            Card *randButton = new Card(current);                   // create Card object of current word
            this->addWidget(randButton, row, col, 1, 1);            // add this to the grid layout
            QObject::connect(randButton, SIGNAL(newCardOpened()),   // if a new card is opened, check for match
                                        this, SLOT(checkPaired()));
        }
    }
}

// check for a new pair in game grid
void MyGrid::checkPaired(){
    bool matched = false;   //flag
    this->opencards+=1;     // this slot is called when a new card is opened
    if(this->opencards==2){ // if 2 cards are opened currently, check if they are pairs
        QString unpaired="";                        // text to check for match
        Card *otherpair;                            // the other card that is opened

        for (int i = 0; i < this->count(); ++i){    // disconnect all so that new cards cannot be opened
            Card *wd = qobject_cast<Card*> (this->itemAt(i)->widget());
            QObject::disconnect(wd, SIGNAL(clicked()), wd, SLOT(openCard()));
        }

        for (int i = 0; i < this->count(); ++i){
          Card *widget = qobject_cast<Card*> (this->itemAt(i)->widget());
          if(!widget->paired && widget->open){      // opened but not yet paired
              if(unpaired==""){                     // encountered the first one
                  unpaired=widget->cardText;
                  otherpair=widget;
              }
              else{                                 // encountered the second one
                  if(widget->cardText==unpaired){   // the texts match, set cards as paired and disable
                      // set the cards as paired and format differently
                      widget->paired=true;
                      otherpair->paired=true;
                      QPalette pal = QPalette();
                      pal.setColor(QPalette::Button, QColor(Qt::darkGreen));
                      pal.setColor(QPalette::ButtonText, QColor(Qt::white));
                      otherpair->setPalette(pal);
                      widget->setPalette(pal);
                      matched=true;             // set flag meaning a match has happened
                      this->opencards=0;        // reset number of open cards
                      this->matchedpairs+=1;    // increment number of matched pairs
                      paired();                 // increment score

                      if(matchedpairs==15){     // the game has ended, all pairs have been found
                          this->timer->stop();  // stop the timer and show win message
                          QMessageBox msgBox;
                          msgBox.setText("You won!");
                          msgBox.exec();
                          msgBox.setStandardButtons(QMessageBox::Cancel);
                      }
                      break;
                  }
                  else{ // the texts do not match, keep the cards open for 1 second before flipping them back
                      QTimer::singleShot(1000, widget, SLOT(closeCard()));
                      QTimer::singleShot(1000, otherpair, SLOT(closeCard()));
                      this->opencards=0; // reset number of open cards
                  }
              }
          }
        }
        // enable the cards back again
        for (int i = 0; i < this->count(); ++i){
          Card *wd = qobject_cast<Card*> (this->itemAt(i)->widget());
          if(!wd->paired) {
              if(!matched) // if a match has not happened, do not enable cards until the unmatched cards are flipped back
                  QTimer::singleShot(1000, wd, SLOT(enableCard()));
              else        // if a match has happened, let enableing happen faster
                  QTimer::singleShot(100, wd, SLOT(enableCard()));
          }
        }
    }
}
