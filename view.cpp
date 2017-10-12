#include "view.h"
#include "ui_view.h"
#include "model.h"
#include <QMessageBox>

#include <unistd.h>

View::View(Model& model, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::View)
{
    ui->setupUi(this);

    waitTime = 500;
    waitTimeForUser = 50;
    youLoseBox.setHidden(true);


    //ui->redButton->setStyleSheet( QString("QPushButton {background-color: rgb(150,0,0);} QPushButton:pressed {background-color: rgb(255,0,0);}"));
    //ui->blueButton->setStyleSheet( QString("QPushButton {background-color: rgb(0,0,150);} QPushButton:pressed {background-color: rgb(0,0,255);}"));

    //connect start button clicked signal to model's startGame slot
    connect(ui->startButton, &QPushButton::clicked, &model, &Model::startGame);
    //connect gameStarted signal to setEnabled slot for start button
    connect(&model, &Model::gameStarted, ui->startButton, &QPushButton::setEnabled);

    //connect playersTurn signal to setEnabled slot for red and blue buttons
    connect(&model, &Model::enablePlayerButtons, ui->blueButton, &QPushButton::setEnabled);
    connect(&model, &Model::enablePlayerButtons, ui->redButton, &QPushButton::setEnabled);
    //connect(&model, &Model::playersTurn, this, &View::playersTurn);

    //connect hitRed(Blue)Button signals to changeRed(Blue)ButtonColor slots
    connect(&model, &Model::hitRedButton, this, &View::highlightRedButton);
    connect(&model, &Model::hitBlueButton, this, &View::highlightBlueButton);

    //connect the view's playNextInPattern signal to the model's playNextInPattern slot
    connect(this, &View::playNextInPattern, &model, &Model::playNextInPattern);

    //connect red and blue buttons to their pressedButton slots
    connect(ui->blueButton, &QPushButton::pressed, this, &View::pressedBlueButton);
    connect(ui->blueButton, &QPushButton::pressed, &model, &Model::pressedBlueButton);
    connect(ui->redButton, &QPushButton::pressed, this, &View::pressedRedButton);
    connect(ui->redButton, &QPushButton::pressed, &model, &Model::pressedRedButton);

    connect(&model, &Model::endGame, this, &View::endGame);

    //connect model to progress bar to update it
    connect(&model, &Model::updateProgress, ui->progressBar, &QProgressBar::setValue);

    connect(&model, &Model::makeFaster, this, &View::makeFaster);

    connect(this, &View::computersTurnS, &model, &Model::computersTurn);

}

//called when the user makes a mistake in repeating the pattern back (only turns one of these black because the button gets
//unhighlighed after (not a big deal b/c this isn't what we should do anyway just fyi))
void View::endGame(){
    waitTime = 500;
    ui->redButton->setEnabled(false);
    ui->blueButton->setEnabled(false);
    ui->startButton->setEnabled(true);
    ui->startButton->setText("Restart");
    ui->progressBar->setStyleSheet("background-color: #05B8CC;");
    ui->lcdLevel->display(1);
    youLoseBox.setText("You Lose!!");
    youLoseBox.setHidden(false);
}

void View::makeFaster(){
    displayTimer.singleShot(500, this, &View::computersTurn);
    waitTime = waitTime/1.15;
    int val = ui->lcdLevel->intValue();
    val++;
    ui->lcdLevel->display(val);
}

void View::computersTurn(){
    emit computersTurnS();
}

// SLOTS USED FOR LETTING THE USER PLAY BACK THE PATTERN

//called when the user presses the blue button. highlights the button, and after waiting for waitTimeForUser
//unhighlights the button.
void View::pressedBlueButton(){
    ui->blueButton->setStyleSheet("background-color: rgb(0,0,255);");
    displayTimer.singleShot(waitTimeForUser, this, &View::unhighlightBlueButton);
}

//unhighlights the blue button
void View::unhighlightBlueButton(){
    ui->blueButton->setStyleSheet("background-color: rgb(0,0,150);");
}

//called when the user presses the red button. highlights the button, and after waiting for waitTimeForUser
//unhighlights the button.
void View::pressedRedButton(){
    ui->redButton->setStyleSheet("background-color: rgb(255,0,0);");
    displayTimer.singleShot(waitTimeForUser, this, &View::unhighlightRedButton);
}

//unhighlights the red button
void View::unhighlightRedButton(){
    ui->redButton->setStyleSheet("background-color: rgb(150,0,0);");
}


// SLOTS THAT DEAL WITH LETTING THE COMPUTER PLAY OUT THE PATTERN

//used when the computer is playing out the pattern - highlights the blue button and then calls the
//display timer, which will call the unhighlightButtons method when it's done
void View::highlightRedButton(){
    ui->redButton->setStyleSheet("background-color: rgb(255,0,0);");
    displayTimer.singleShot(waitTime, this, &View::unhighlightButtons);
}

//used when the computer is playing out the pattern - highlights the blue button and then calls the
//display timer, which will call the unhighlightButtons method when it's done
void View::highlightBlueButton(){
    ui->blueButton->setStyleSheet("background-color: rgb(0,0,255);");
    displayTimer.singleShot(waitTime, this, &View::unhighlightButtons);
}

//used when the computer is playing out the pattern - unhighlights the buttons when the displayTimer runs out,
//and then starts the display timer again (which calls the model to play the next color in the pattern when it
//runs out)
void View::unhighlightButtons(){
    ui->blueButton->setStyleSheet("background-color: rgb(0,0,150);");
    ui->redButton->setStyleSheet("background-color: rgb(150,0,0);");
    displayTimer.singleShot(waitTime, this, &View::playNextInPattern);
}

View::~View()
{
    delete ui;
}
