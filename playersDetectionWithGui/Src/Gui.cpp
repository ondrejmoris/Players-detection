#include "../Include/Gui.h"

Gui::Gui(Application* app, QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->app = app;
    
    //this->sw = sw;
}

void Gui::on_start_clicked()
{
    std::cout << "Start" << std::endl;
    setParams();
    if (app->run() == -1)
        QMessageBox::critical(this, "Pozor!", "Chyba!");
}

void Gui::on_back_clicked()
{
    this->hide();
    emit back_clicked();
}

void Gui::on_getPrew_clicked()
{
    std::cout << "get prew clicked" << std::endl;
    setParams();
    showPreviw();
}

void Gui::showPreviw()
{
    std::cout << "get prew started" << std::endl;
    cv::Mat preview = this->app->getPreview();
    cv::cvtColor(preview, preview, CV_BGR2RGB);
    QImage img((uchar*)preview.data, preview.cols, preview.rows, preview.step1(), QImage::Format_RGB888);
    this->ui.previewImg->setPixmap(QPixmap::fromImage(img));
    this->ui.previewImg->setScaledContents(true);
}

void Gui::setParams()
{
    app->setWhereTrack(this->ui.whereTrack->currentIndex());
    app->setTracker(this->ui.trackerType->currentIndex());
    app->setCrop(cv::Point(this->ui.firstX->value(), this->ui.firstY->value()),
        cv::Point(this->ui.secondX->value(), this->ui.secondY->value()));
    app->setFramesToWrite(this->ui.framesToWrite->value());
    app->setErodeSize(this->ui.erodeSize->value());
    app->setDialteSize(this->ui.dilateSize->value());
    app->setTreshold(this->ui.thresh->value());
    app->setMinBoxArea(this->ui.minArea->value());
}

