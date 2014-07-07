#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPixmap>

namespace Ui {
class ImageWidget;
}

class ImageWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ImageWidget(QWidget *parent = 0);
    ~ImageWidget();


    bool isCheckedImage();
    void setImage(QString dir, QString &fileName);
    void checkImage();
    void uncheckedImage();
    void showInfoImg();
    void hideInfoImg();
    bool isVisibleInfoImg();
    QString& getFileNameImg();
    QString& getLinkImg();
    void showOkIcon();

private:
    Ui::ImageWidget *ui;
    QSize imgSize;
    QString fileSize;
    QString linkImg;
    QString fileNameImg;

    QString getSizeFile(QString &link);

public slots:
    void s_showInfoImg();
    void showOriginImg();

};

#endif // IMAGEWIDGET_H
