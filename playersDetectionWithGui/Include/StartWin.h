#pragma once

#include <QWidget>
#include "ui_StartWin.h"
//#include "../x64/Debug/uic/ui_StartWin.h"

#include <QFileDialog>
#include <QMessageBox>
#include "Application.h"

class StartWin : public QWidget
{
	Q_OBJECT

public:
	StartWin(Application* app, QWidget *parent = Q_NULLPTR);
	~StartWin();

private slots:
	void on_loadBackground_clicked();
	void on_loadVideo_clicked();
	void on_next_clicked();

signals:
	void goNext();

private:
	Ui::StartWin ui;
	Application* app;
	bool videoReady;
	bool imgReady;
	void reset();
};
