#ifndef EGGSIZERML_H
#define EGGSIZERML_H

// Qt Native Stuff
#include <QDir>
#include <QFileDialog>
#include <QImageReader>
#include <QImageWriter>
#include <QMainWindow>
#include <QMessageBox>
#include <QStandardPaths>

// OpenCV2 Inclusions
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

QT_BEGIN_NAMESPACE namespace Ui { class eggsizerML; }
QT_END_NAMESPACE

class eggsizerML : public QMainWindow {
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
  void outputResult(const QString &filename,
                    std::vector<std::string> &imageNames,
                    const std::vector<int> &eggNumbers,
                    const std::vector<double> &avgAreas,
                    const std::vector<double> &otsusAreas,
                    const std::vector<double> &blobAreas,
                    const std::vector<double> &certainties, int outputFormat,
                    int emitImage);
  void on_saveResults_btn_clicked();
  void on_folderOpen_btn_clicked();
  void openFolder();
  // < ---------------------------------------- >

private:
  Ui::eggsizerML *ui;
  QStringList imageFiles;    // Store selected image paths
  int currentImageIndex = 0; // Track which image is displayed

  void loadImageAtIndex(int index);
};
#endif // EGGSIZERML_H
