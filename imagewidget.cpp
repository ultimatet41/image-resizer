#include "imagewidget.h"
#include "ui_imagewidget.h"
#include <QFile>
#include <QDesktopServices>
#include <QUrl>

ImageWidget::ImageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageWidget)
{
    ui->setupUi(this);
    connect(ui->descPb, SIGNAL(clicked()), SLOT(s_showInfoImg()));
    connect(ui->openImgPb, SIGNAL(clicked()), SLOT(showOriginImg()));

    ui->descTe->setVisible(false);
    ui->descTe->setReadOnly(true);

    ui->okLb->setVisible(false);
}

ImageWidget::~ImageWidget()
{
    delete ui;
}

void ImageWidget::setImage(QString dir, QString &fileName)
{
    fileNameImg = fileName;

    linkImg =dir+"/"+fileName;

    fileSize = getSizeFile(linkImg);

    QPixmap pix(linkImg);
    imgSize = pix.size();
    QSize size(120, 120);
    if((imgSize.width() > size.width()) || (imgSize.height() > size.height())){
        QPixmap result;
        result = pix.scaled(size, Qt::KeepAspectRatio);
        ui->checkBox->setIconSize(result.size());
        ui->checkBox->setIcon(result);
    }else {
        ui->checkBox->setIconSize(pix.size());
        ui->checkBox->setIcon(pix);
    }

    QString str = "<b>Имя файла: </b>" + fileName +", <b>Разрешение: </b>" + QString::number(imgSize.width())+"x"+
            QString::number(imgSize.height())+", <b>Размер файла: </b>" + fileSize;
    ui->descTe->setHtml(trUtf8(str.toAscii()));
}

void ImageWidget::checkImage()
{
    ui->checkBox->setChecked(true);
}

void ImageWidget::uncheckedImage()
{
    ui->checkBox->setChecked(false);
}

QString ImageWidget::getSizeFile(QString &link)
{
    qint64 nSize = 0;
    QFile myFile(link);
    if (myFile.open(QIODevice::ReadOnly)){
        nSize = myFile.size();
        myFile.close();
    }

    qint64 i = 0;
    for (; nSize > 1023; nSize /= 1024, ++i) { }
    return QString().setNum(nSize) + "BKMGT"[i];
}

void ImageWidget::s_showInfoImg()
{
    if (!ui->descTe->isVisible()){
       ui->descTe->setVisible(true);
    }else {
       ui->descTe->setVisible(false);
    }
}

void ImageWidget::showOriginImg()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile(linkImg) );
}

void ImageWidget::showInfoImg()
{
   ui->descTe->setVisible(true);
}

void ImageWidget::hideInfoImg()
{
  ui->descTe->setVisible(false);
}

bool ImageWidget::isVisibleInfoImg()
{
    return ui->descTe->isVisible();
}

QString &ImageWidget::getFileNameImg()
{
    return fileNameImg;
}

QString &ImageWidget::getLinkImg()
{
    return linkImg;
}

void ImageWidget::showOkIcon()
{
    ui->okLb->setVisible(true);
}

bool ImageWidget::isCheckedImage()
{
    return ui->checkBox->isChecked();
}
