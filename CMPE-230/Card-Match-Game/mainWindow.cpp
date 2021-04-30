#include <QApplication>
#include "card.h"
#include "myTimer.h"
#include "mygrid.h"
#include "myScorer.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);           // the application
    QWidget *cw = new QWidget;              // central widget
    MyTimer *mt= new MyTimer();             // timer
    MyGrid *gl = new MyGrid(mt->timer);     // game area
    myscorer *ms = new myscorer();          // scorer
    QObject::connect(gl, SIGNAL(paired()),  ms, SLOT(increaseScore())); // if a pair is matched, increase the score
    QObject::connect(mt, SIGNAL(timerunout()), gl, SLOT(endgame()));    // if time limit is exceeded, end the game
    gl->createNew();                                                    // create new game at the beginning

    // Initializing and adjusting New Game and Quit Buttons
    QPalette pal; // palette to color the PushButtons (NewGame & Quit)
    pal.setColor(QPalette::Button, QColor(Qt::darkGray));
    pal.setColor(QPalette::ButtonText, QColor(Qt::white));
    QPushButton *newGame = new QPushButton("New Game");     // the button to start a new game
    QPushButton *quit = new QPushButton("Quit");            // the button to quit the game
    newGame->setPalette(pal);
    newGame->setAutoFillBackground(true);
    newGame->setFlat(true);
    quit->setPalette(pal);
    quit->setFlat(true);
    quit->setAutoFillBackground(true);
    QObject:: connect(newGame, SIGNAL(clicked()), gl, SLOT(createNew()));   // create new game layout
    QObject:: connect(newGame, SIGNAL(clicked()), mt, SLOT(reset()));       // reset the game timer
    QObject:: connect(newGame, SIGNAL(clicked()), ms, SLOT(reset()));       // reset the game score
    QObject:: connect(quit, SIGNAL(clicked()), &app, SLOT(quit()));         // quit the application

    // Initializing and adjusting widgets that show time and score
    QPalette pal2; // palette to color the Widgets (Timer & Scorer)
    pal2.setColor(QPalette::Window, QColor(Qt::darkGray));
    pal2.setColor(QPalette::WindowText, QColor(Qt::white));
    QWidget *timerwidget = mt->label;           // create widget with label of the timer
    timerwidget->setPalette(pal2);
    timerwidget->setAutoFillBackground(true);
    QWidget *scorerwidget = ms->label;          // create widget with label of the scorer
    scorerwidget->setPalette(pal2);
    scorerwidget->setAutoFillBackground(true);

    // forming the general layout as a grid
   QGridLayout *maingrid = new QGridLayout();
    maingrid->addWidget(timerwidget,0,0,1,1);
    maingrid->addWidget(scorerwidget,0,1,1,1);
    maingrid->addWidget(newGame,0,4,1,1);
    maingrid->addWidget(quit,0,5,1,1);
    maingrid->addLayout(gl,5,0,1,6);
    // setting constraints on widht and height for a better user experience
    cw->setMinimumWidth(800);
    cw->setMaximumHeight(500);
    cw->setMaximumWidth(1000);
    cw->setLayout(maingrid);

    // adjusting the central widget
    QPalette pal3;
    pal3.setColor(QPalette::Background, Qt::white);
    cw->setAutoFillBackground(true);
    cw->setPalette(pal3);
    cw->setWindowTitle("Card Match Game");
    cw->resize(800, 400);
    cw->show();

    return app.exec();
}
