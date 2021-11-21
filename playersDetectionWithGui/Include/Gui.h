#pragma once

#include <QtWidgets/QWidget>
#include <QFileDialog>
#include <QMessageBox>
#include "ui_Gui.h"
#include "Application.h"

class Gui : public QWidget
{
    Q_OBJECT

public:
    Gui(Application* app, QWidget *parent = Q_NULLPTR);
private slots:
    //void on_loadBackground_clicked();
    //void on_loadVideo_clicked();
    void on_start_clicked();
    void on_back_clicked();
    void on_getPrew_clicked();

signals:
    void back_clicked();

private:
    Ui::GuiClass ui;
    Application* app;

    void showPreviw();
    void setParams();
};
