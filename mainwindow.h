#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include "imagewidget.h"
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    Ui::MainWindow *ui;
    QString imagesDir;
    QString saveDir;
    QStringList dirList;
    void clearListImg();
    void createResultFolder();
    void createListResolutions();
    void saveSettings();
    void loadSettings();

    virtual void closeEvent(QCloseEvent *);

public slots:
    void loadImagesList();
    void allChecked();
    void allUnchecked();
    void setSaveDirImg();
    void showHideAllInfoImg();
    void resizeImgAndSave();
    void s_clearListImg();
    void showAbout();
    void setAutoCreateFolderResult();
    void showOnlyChecked();
    void setResolution();
    void resetResolution();
    void saveResolution();
};

#endif // MAINWINDOW_H
