#include "../Include/StartWin.h"

StartWin::StartWin(Application* app, QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
    this->app = app;
}

void StartWin::on_loadBackground_clicked() {
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "", tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "Pozor!", "Nebylo vybrano zadne pozadi!");
        return;
    }
    if (app->loadBackground(fileName.toStdString())) {
        QMessageBox::information(this, "Uspech!", "Pozadi bylo nacteno!");
        this->imgReady = true;
    }
    else
        QMessageBox::warning(this, "Chyba!", "Chyba pri cteni pozadi!");
}

void StartWin::on_loadVideo_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Video"), "", tr("Image Files (*.mp4 *.avi)"));
    if (fileName.isEmpty()) {
        QMessageBox::warning(this, "Pozor!", "Nebylo vybrano zadne video!");
        return;
    }
    if (app->loadVideo(fileName.toStdString())) {
        QMessageBox::information(this, "Uspech!", "Video bylo nacteno!");
        this->videoReady = true;
    }
    else
        QMessageBox::warning(this, "Chyba!", "Chyba pri cteni videa!");
}

void StartWin::on_next_clicked()
{
    if (this->videoReady && this->imgReady) {
        this->hide();
        this->reset();
        emit goNext();
    }
    else {
        if(!this->videoReady && this->imgReady)
            QMessageBox::warning(this, "Chyba!", "Video nebylo pridano!");
        else if(!this->imgReady && this->videoReady)
            QMessageBox::warning(this, "Chyba!", "Pozadi nebylo pridano!");
        else
            QMessageBox::warning(this, "Chyba!", "Video ani pozadi nebylo pridano!");
    }
}

void StartWin::reset()
{
    this->imgReady = false;
    this->videoReady = false;
}

StartWin::~StartWin()
{
    //delete this->secondWin;
}
