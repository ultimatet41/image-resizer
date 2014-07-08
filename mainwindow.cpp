#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QListWidgetItem>
#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->folderImgPb, SIGNAL(clicked()), SLOT(loadImagesList()));
    connect(ui->checkedPb, SIGNAL(clicked()), SLOT(allUnchecked()));
    connect(ui->descImgPb, SIGNAL(clicked()), SLOT(showHideAllInfoImg()));
    connect(ui->saveDirPb, SIGNAL(clicked()), SLOT(setSaveDirImg()));
    connect(ui->runPb, SIGNAL(clicked()), SLOT(resizeImgAndSave()));
    connect(ui->clearPb, SIGNAL(clicked()), SLOT(s_clearListImg()));
    connect(ui->action, SIGNAL(triggered()), SLOT(showAbout()));
    connect(ui->resultFolderCb, SIGNAL(clicked()), SLOT(setAutoCreateFolderResult()));
    connect(ui->showCheckedPb, SIGNAL(clicked()), SLOT(showOnlyChecked()));
    connect(ui->imgsLw->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), ui->imgsLw, SLOT(scrollToBottom()));
    connect(ui->resolutionCb, SIGNAL(activated(int)), SLOT(setResolution()));
    connect(ui->wImgLe, SIGNAL(textEdited(QString)), SLOT(resetResolution()));
    connect(ui->hImgLe, SIGNAL(textEdited(QString)), SLOT(resetResolution()));
    connect(ui->saveResolCb, SIGNAL(clicked()), SLOT(saveResolution()));

    QRegExp reg("(\\d+)");
    QValidator *validator = new QRegExpValidator(reg);
    ui->wImgLe->setValidator(validator);
    ui->hImgLe->setValidator(validator);

    createListResolutions();

    QCoreApplication::setOrganizationName("ultimatet41");
    QCoreApplication::setOrganizationDomain("none.ru");
    QCoreApplication::setApplicationName("Image Resize");
    loadSettings();
    ui->progressBar->setVisible(false);

}

MainWindow::~MainWindow()
{
    clearListImg();

    delete ui;
}

void MainWindow::clearListImg()
{
    for (int i = 0; i < ui->imgsLw->count(); i++){
        ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
        delete img;
    }
    ui->imgsLw->clear();
    dirList.clear();
}

void MainWindow::createResultFolder()
{
    if (!ui->imgDirLe->text().isEmpty()) {
        ui->saveDirLe->setText(ui->imgDirLe->text()+"/result");
        QDir dir(ui->saveDirLe->text());
        if (!dir.exists()){
            dir.mkpath(".");
        }
        saveDir = ui->saveDirLe->text();
        ui->saveDirPb->setEnabled(false);
    }
}

void MainWindow::createListResolutions()
{
    ui->resolutionCb->addItem("   --   ");
    ui->resolutionCb->addItem("600x400");
    ui->resolutionCb->addItem("800x600");
    ui->resolutionCb->addItem("1024x768");
    ui->resolutionCb->addItem("1280x720");
    ui->resolutionCb->addItem("1280x800");
    ui->resolutionCb->addItem("1600x900");
    ui->resolutionCb->addItem("1920x1080");
}

void MainWindow::saveSettings()
{
    QSettings settings;
    settings.setValue("defaltResolution", ui->resolutionCb->currentIndex());
    settings.setValue("aspectRatio", ui->saveRatioRb->isChecked());
    settings.setValue("speedEdit", ui->fastScaleRb->isChecked());
    settings.setValue("openSaveDir", ui->openDirChB->isChecked());
    settings.setValue("createResultDir", ui->resultFolderCb->isChecked());
}

void MainWindow::loadSettings()
{
    QSettings settings;

    int index = settings.value("defaltResolution", 0).toInt();
    ui->resolutionCb->setCurrentIndex(index);
    setResolution();

    bool b = settings.value("aspectRatio", true).toBool();
    if (b) {
        ui->saveRatioRb->setChecked(true);
        ui->ignorRb->setChecked(false);
    }
    else {
        ui->saveRatioRb->setChecked(false);
        ui->ignorRb->setChecked(true);
    }

    b = settings.value("speedEdit", true).toBool();
    if (b) {
        ui->fastScaleRb->setChecked(true);
        ui->smoothScaleRb->setChecked(false);
    }
    else {
        ui->fastScaleRb->setChecked(false);
        ui->smoothScaleRb->setChecked(true);
    }

    b = settings.value("openSaveDir", false).toBool();
    ui->openDirChB->setChecked(b);

    b = settings.value("createResultDir", true).toBool();
    if (b){
        ui->resultFolderCb->setChecked(b);
        ui->saveDirPb->setEnabled(false);
    }

}

void MainWindow::closeEvent(QCloseEvent *)
{
    saveSettings();
}


void MainWindow::loadImagesList()
{
    s_clearListImg();
    QString dirExist = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"", QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);

    if (dirExist.isEmpty()){return;}

    QString dirStr;

    QDir dir(dirExist);
    dirList = dir.entryList(QStringList() << "*.jpg" << "*.jpeg" << "*.png", QDir::Files);


    QString str;

    if (dirList.size() > 0){

        ui->progressBar->reset();
        ui->progressBar->setVisible(true);
        int progressSize = 100 / dirList.size();
        for (int i = 0; i < dirList.size(); i++){

            QListWidgetItem *item = new QListWidgetItem(ui->imgsLw);

            ImageWidget *imgw = new ImageWidget();
            imgw->setImage(dir.absolutePath(), dirList[i]);
            imgw->checkImage();

            ui->imgsLw->setItemWidget(ui->imgsLw->item(i), imgw);
            ui->imgsLw->item(i)->setSizeHint(QSize (200, 100));

            ui->progressBar->setValue(ui->progressBar->value() + progressSize);
        }

        ui->progressBar->setValue(100);

        str = "<b>Картинок:</b> " + QString::number(dirList.size()) + " шт.";
        ui->imgListLabel->setText(trUtf8(str.toAscii()));

        ui->checkedPb->setIcon(QIcon(":/rsc/unchecked_checkbox.png"));
        connect(ui->checkedPb, SIGNAL(clicked()), SLOT(allUnchecked()));

        ui->runPb->setEnabled(true);

        ui->progressBar->setVisible(false);

        ui->imgDirLe->setText(dirExist);
        setAutoCreateFolderResult();
    }
    else {
        ui->runPb->setEnabled(false);
    }
}

void MainWindow::allChecked()
{
    for (int i = 0; i < ui->imgsLw->count(); i++){
        ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
        img->checkImage();
    }
    ui->checkedPb->disconnect();
    ui->checkedPb->setIcon(QIcon(":/rsc/unchecked_checkbox.png"));
    connect(ui->checkedPb, SIGNAL(clicked()), SLOT(allUnchecked()));
}

void MainWindow::allUnchecked()
{
    for (int i = 0; i < ui->imgsLw->count(); i++){
        ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
        img->uncheckedImage();
    }
    ui->checkedPb->disconnect();
    ui->checkedPb->setIcon(QIcon(":/rsc/checked_checkbox.png"));
    connect(ui->checkedPb, SIGNAL(clicked()), SLOT(allChecked()));
}

void MainWindow::setSaveDirImg()
{
    saveDir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),ui->imgDirLe->text(), QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if (saveDir.isEmpty()) {
        saveDir = ui->imgDirLe->text();
        ui->saveDirLe->setText(saveDir);
    }
    else {
        ui->saveDirLe->setText(saveDir);
    }
}

void MainWindow::showHideAllInfoImg()
{
    if(ui->descImgPb->isChecked()){
        for (int i = 0; i < ui->imgsLw->count(); i++){
            ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
            img->hideInfoImg();
        }
    }else {
        for (int i = 0; i < ui->imgsLw->count(); i++){
            ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
            img->showInfoImg();
        }
    }
}

void MainWindow::resizeImgAndSave()
{
    if (!ui->saveResolCb->isChecked()){
        if (ui->saveDirLe->text().isEmpty() || ui->wImgLe->text().isEmpty() || ui->hImgLe->text().isEmpty()){
            QMessageBox::warning(this, trUtf8("Внимание"),
                                 trUtf8("Вы не выбрали папку сохранения или не указали разрешение!"),
                                 QMessageBox::Ok);
            return;
        }
    }

    ui->progressBar->reset();
    ui->progressBar->setVisible(true);
    int progressSize = 100 / ui->imgsLw->count();

    int quality;
    int aspectRatio ;
    int w;
    int h;

    if (ui->fastScaleRb->isChecked()){
        quality = 0;
    }else {
        quality = 1;
    }
    if (ui->ignorRb->isChecked()){
        aspectRatio = Qt::IgnoreAspectRatio;
    }else if(ui->saveRatioRb->isChecked()){
        aspectRatio = Qt::KeepAspectRatio;

    }

    for (int i = 0; i < ui->imgsLw->count(); i++){
        ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
        if (img->isCheckedImage()){
            QPixmap source(img->getLinkImg());
            if (!ui->saveResolCb->isChecked()) {
                w = ui->wImgLe->text().toInt();
                h = ui->wImgLe->text().toInt();
            }
            else {
                w = source.width();
                h = source.height();
            }

            QPixmap result;
            result = source.scaled(QSize(w, h),
                                           (Qt::AspectRatioMode) aspectRatio, (Qt::TransformationMode)quality);

            result.save(saveDir +"/"+img->getFileNameImg());
            img->showOkIcon();                        
        }
       ui->progressBar->setValue(ui->progressBar->value() + progressSize);
    }

    ui->progressBar->setValue(100);

    if (ui->openDirChB->isChecked()){
        QDesktopServices::openUrl( QUrl::fromLocalFile(saveDir) );
    }

    ui->progressBar->setVisible(false);
}

void MainWindow::s_clearListImg()
{
    clearListImg();
    ui->imgDirLe->clear();
    imagesDir = "";
    ui->saveDirLe->clear();
    saveDir = "";
    QString str = "<b>Картинок:</b> " + QString::number(0) + " шт.";
    ui->imgListLabel->setText(trUtf8(str.toAscii()));
    ui->runPb->setEnabled(false);
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, trUtf8("О разработчике"),
                                    trUtf8("<b>Автор программы: </b>Коновалов К.В.\n"
                                           "<b>Email:</b> devkkv@gmail.com"));
}

void MainWindow::setAutoCreateFolderResult()
{
    if (ui->resultFolderCb->isChecked()){
        createResultFolder();
        ui->saveDirPb->setEnabled(false);
    }
    else {
        ui->saveDirLe->setText(ui->imgDirLe->text());
        saveDir = ui->saveDirLe->text();
        ui->saveDirPb->setEnabled(true);
    }
}

void MainWindow::showOnlyChecked()
{
    if (ui->showCheckedPb->isChecked()){
        for (int i = 0; i < ui->imgsLw->count(); i++){
            ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
            if (!img->isCheckedImage()){
                ui->imgsLw->setRowHidden(i, true);
                //img->setVisible(false);
            }
            else {
                //img->setVisible(true);
                ui->imgsLw->setRowHidden(i, false);
            }
        }
    }
    else {
            for (int i = 0; i < ui->imgsLw->count(); i++){
                //ImageWidget *img = (ImageWidget*)ui->imgsLw->itemWidget(ui->imgsLw->item(i));
                ui->imgsLw->setRowHidden(i, false);
            }
    }
}

void MainWindow::setResolution()
{
    switch(ui->resolutionCb->currentIndex()){
        case 0: ui->wImgLe->setText(""); ui->hImgLe->setText(""); break;
        case 1: ui->wImgLe->setText("600"); ui->hImgLe->setText("400"); break;
        case 2: ui->wImgLe->setText("800"); ui->hImgLe->setText("600"); break;
        case 3: ui->wImgLe->setText("1024"); ui->hImgLe->setText("768"); break;
        case 4: ui->wImgLe->setText("1280"); ui->hImgLe->setText("720"); break;
        case 5: ui->wImgLe->setText("1280"); ui->hImgLe->setText("800"); break;
        case 6: ui->wImgLe->setText("1600"); ui->hImgLe->setText("900"); break;
        case 7: ui->wImgLe->setText("1920"); ui->hImgLe->setText("1080"); break;
    }
}

void MainWindow::resetResolution()
{
    ui->resolutionCb->setCurrentIndex(0);
}

void MainWindow::saveResolution()
{
    if (ui->saveResolCb->isChecked()) {
        ui->wImgLe->setEnabled(false);
        ui->hImgLe->setEnabled(false);
        ui->resolutionCb->setEnabled(false);
    }
    else {
        ui->wImgLe->setEnabled(true);
        ui->hImgLe->setEnabled(true);
        ui->resolutionCb->setEnabled(true);
    }
}

