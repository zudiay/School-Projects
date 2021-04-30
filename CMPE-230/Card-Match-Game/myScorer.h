#ifndef MYSCORER_H
#define MYSCORER_H
#include <QTimer>
#include <QLabel>
#include <QMessageBox>

// scorer counts the number of matched pairs in cumulative manner from the beginning of the game

class myscorer: public QObject{
    Q_OBJECT
public:
    myscorer();
    QLabel *label;
    int counter;

public slots:
    void increaseScore();
    void reset();
};


#endif // MYSCORER_H
