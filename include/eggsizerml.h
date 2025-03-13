#ifndef EGGSIZERML_H
#define EGGSIZERML_H

// Qt Native Stuff
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>

// OpenCV2 Inclusions
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

QT_BEGIN_NAMESPACE
namespace Ui {
class eggsizerML;
}
QT_END_NAMESPACE

class eggsizerML : public QMainWindow
{
    Q_OBJECT

public:
    eggsizerML(QWidget *parent = nullptr);
    ~eggsizerML();

private slots:
    // < ---------------------------------------- >
    // BASIC IMAGE UPLOAD/PROCESSING WITH OPENCV
    void open();
    void on_fileOpen_btn_clicked();
    bool loadFile(const QString &fileName);
    void showNextImage();
    void showPreviousImage();
    // < ---------------------------------------- >

private:
    Ui::eggsizerML *ui;
    QStringList imageFiles;  // Store selected image paths
    int currentImageIndex = 0; // Track which image is displayed

    void loadImageAtIndex(int index);
};
#endif // EGGSIZERML_H
