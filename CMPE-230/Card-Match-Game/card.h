#ifndef Card_H
#define Card_H
#include <QPushButton>
#include <QPalette>

// Card is the main item in the game
class Card : public QPushButton{
    Q_OBJECT

public:
    Card(const QString& text, QWidget* parent = 0);
    QString cardText;   // the word written on the card
    bool paired;        // shows whether the card has been matched with it's pair successfully
    bool open;          // shows whether the card has been turned open by the user

public slots:
    void openCard();         // flips the card
    void closeCard();
    void enableCard();
signals:
    void newCardOpened();   // indicates a card has been turned open
};

#endif // Card_H
