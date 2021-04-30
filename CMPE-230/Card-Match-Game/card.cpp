 #include "card.h"
#include <QThread>
#include <QTimer>


Card::Card(const QString& text, QWidget* parent): QPushButton(text, parent){ // constructor
    this->cardText = text;
    this->paired = false;
    this->open = false;
    this->setText("?");
    QPalette pal = palette();
    pal.setColor(QPalette::Button, QColor(Qt::darkRed));
    setPalette(pal);
    setFlat(true);
    setAutoFillBackground(true);
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(openCard())); // when clicked, flip the card open
    update();
}

void Card::openCard(){
    if(this->paired==false && this->open==false){   // can't open an already opened or paired card
            this->open=true;                        // set status as open
            this->setText(this->cardText);          // make the text visible
            this->setEnabled(false);                // disable clicking
            // colouring the currently opened cards differently
            QPalette pal = palette();
            pal.setColor(QPalette::Button, QColor(Qt::darkGray));
            pal.setColor(QPalette::ButtonText, QColor(Qt::white));
            setPalette(pal);
            newCardOpened();                        // signal a new card has been opened
        }
}

void Card::enableCard(){ // connect signal click to card opening slot
    QObject::connect(this, SIGNAL(clicked()), this, SLOT(openCard()));
}

void Card::closeCard(){
    if(this->paired==false && this->open==true){    // can't close a paired or closed card
            this->open=false;                       // set status as closed
            this->setText("?");                     // convert the text to unknown sign again
            this->setEnabled(true);                 // enable clicking again
            // colour again like the closed cards
            QPalette pal = palette();
            pal.setColor(QPalette::Button, QColor(Qt::darkRed));
            setPalette(pal);
        }
}
