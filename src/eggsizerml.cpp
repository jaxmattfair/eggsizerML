#include "../include/eggsizerml.h"
#include "../include/asmOpenCV.h"
#include "../include/blobDetect.h"
#include "../include/cannyDetect.h"
#include "../include/measureEdges.h"
#include "../include/ui_eggsizerml.h"
#include <opencv2/core/mat.hpp>
#include <QPainter>
#include <QToolTip>

// GLOBAL APPLICATION DATA STORAGE
//(keep this to a MINIMUM)
cv::Mat orig;     // original image
cv::Mat cannyDst; // canny-detected image
cv::Mat polyDst;  // polygonally-approximated image
cv::Mat blobDst;  // blob-detected image

eggsizerML::eggsizerML(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::eggsizerML) {
  ui->setupUi(this);
  int fixedImageWidth = 400; // Adjust to match your design

  ui->imgDisp_ur->setMinimumWidth(fixedImageWidth);
  ui->imgDisp_ur->setMaximumWidth(fixedImageWidth);

  ui->imgDisp_lr->setMinimumWidth(fixedImageWidth);
  ui->imgDisp_lr->setMaximumWidth(fixedImageWidth);

  connect(ui->nextImageButton, &QPushButton::clicked, this,
          &eggsizerML::showNextImage);
  connect(ui->previousImageButton, &QPushButton::clicked, this,
          &eggsizerML::showPreviousImage);
  connect(ui->folderOpen_btn, &QPushButton::clicked, this,
          &eggsizerML::openFolder);
}

eggsizerML::~eggsizerML() { delete ui; }

// < ---------------------------------------- >
// BASIC FILE/FOLDER UPLOAD/PROCESSING WITH OPENCV
static void initializeImageFileDialog(QFileDialog &dialog,
                                      QFileDialog::AcceptMode acceptMode) {
  static bool firstDialog = true;

  // upon first dialog, figure out the most convenient
  // path for the file explorer to open up to
  if (firstDialog) {
    firstDialog = false;
    const QStringList picturesLocations =
        QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath()
                                                    : picturesLocations.last());
  }

  // #NOTE: may want to expand acceptable types in future
  // compile list of acceptable mime types for file explorer
  QStringList mimeTypeFilters;
  const QByteArrayList supportedMimeTypes =
      acceptMode == QFileDialog::AcceptOpen
          ? QImageReader::supportedMimeTypes()
          : QImageWriter::supportedMimeTypes();
  for (const QByteArray &mimeTypeName : supportedMimeTypes)
    mimeTypeFilters.append(mimeTypeName);
  mimeTypeFilters.sort();
  dialog.setMimeTypeFilters(mimeTypeFilters);
  dialog.selectMimeTypeFilter("image/jpeg");
  dialog.setAcceptMode(acceptMode);
  if (acceptMode == QFileDialog::AcceptSave)
    dialog.setDefaultSuffix("jpg");
}

// opens file dialog
void eggsizerML::open() {
  QFileDialog dialog(this, tr("Select Images or Folder"));
  dialog.setFileMode(QFileDialog::ExistingFiles);
  dialog.setOption(QFileDialog::ShowDirsOnly, false);

  if (dialog.exec() == QDialog::Accepted) {
    QString selectedPath = dialog.selectedFiles().first();

    QFileInfo fileInfo(selectedPath);
    if (fileInfo.isDir()) {
      QDir directory(selectedPath);
      QStringList filters;
      filters << "*.jpg" << "*.png" << "*.bmp" << "*.tiff";
      imageFiles = directory.entryList(filters, QDir::Files);

      for (int i = 0; i < imageFiles.size(); i++) {
        imageFiles[i] = directory.absoluteFilePath(imageFiles[i]);
      }
    } else {
      imageFiles = dialog.selectedFiles();
    }

    if (!imageFiles.isEmpty()) {
      currentImageIndex = 0;
      loadImageAtIndex(currentImageIndex);
    }
  }
}

void eggsizerML::openFolder() {
  qDebug() << "openFolder() called";

  QString selectedFolder = QFileDialog::getExistingDirectory(
      this, tr("Select a Folder"), QDir::homePath());

  if (selectedFolder.isEmpty()) {
    qDebug() << "No folder selected.";
    return;
  }

  qDebug() << "Selected Folder: " << selectedFolder;

  QDir directory(selectedFolder);
  QStringList filters;
  filters << "*.jpg" << "*.png" << "*.bmp" << "*.tiff";
  imageFiles = directory.entryList(filters, QDir::Files);

  for (int i = 0; i < imageFiles.size(); i++) {
    imageFiles[i] = directory.absoluteFilePath(imageFiles[i]);
  }

  if (!imageFiles.isEmpty()) {
    currentImageIndex = 0;
    loadImageAtIndex(currentImageIndex);
  }
}

void eggsizerML::loadImageAtIndex(int index) {
  if (index < 0 || index >= imageFiles.size())
    return;

  currentImageIndex = index;
  loadFile(imageFiles[index]); // Load the selected image
}

void eggsizerML::handleEggSegmentation(cv::Mat* src, cv::Mat* dst, cv::Mat* orig, float pixToMM) {
    std::vector<double> eggAreas = polyApproxFromEdges(src, dst, orig, pixToMM);

    // Convert the polygon-detection image (polygon approximation) to QImage for display
    QImage imgPoly = ASM::cvMatToQImage(*dst); // This is the polygon approximation image (polyapprox)

    // Convert the blob-detection image to QImage for display
    QImage imgBlob = ASM::cvMatToQImage(blobDst); // This is the blob-detected image (blob)

    // Label the blob image (imgDisp_ur)
    QPainter painterBlob(&imgBlob);
    painterBlob.setPen(Qt::red);  // Use a color for blob labels
    painterBlob.setFont(QFont("Arial", 12));

    std::vector<cv::Point> blobCentroids = getCentroidsFromEdges(src, &blobDst, orig); // Assuming this gets blob centroids
    for (int i = 0; i < blobCentroids.size(); ++i) {
        QString label = QString::number(i + 1); // Label blobs as 1, 2, 3, ...
        QPoint center(blobCentroids[i].x, blobCentroids[i].y);
        painterBlob.drawText(center, label);
    }

    // Display the blob image (imgDisp_ur)
    ui->imgDisp_ur->setPixmap(QPixmap::fromImage(imgBlob));

    // Label the polygon-detection image (imgDisp_lr) (this is already working as expected)
    QPainter painterPoly(&imgPoly);
    painterPoly.setPen(Qt::green);  // Use a different color for polygon labels
    painterPoly.setFont(QFont("Arial", 12));

    std::vector<cv::Point> polyCentroids = getCentroidsFromEdges(src, dst, orig); // Assuming this gets polygon centroids
    for (int i = 0; i < polyCentroids.size(); ++i) {
        QString label = QString::number(i + 1); // Label polygons as 1, 2, 3, ...
        QPoint center(polyCentroids[i].x, polyCentroids[i].y);
        painterPoly.drawText(center, label);
    }

    // Display the polygon approximation image (imgDisp_lr)
    ui->imgDisp_lr->setPixmap(QPixmap::fromImage(imgPoly));
}

QImage MatToQImage(const cv::Mat& mat) {
    QImage img(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
    return img.rgbSwapped();
}

void eggsizerML::onInfoIconHovered() {
    QToolTip::showText(QCursor::pos(), "Version: 1.0.0\nDevelopers: Jackson Fair, James Murrer, Benjamin Davis");
}

// loads file into label element & CV mat for analysis
bool eggsizerML::loadFile(const QString &fileName) {
    // validate file and read in
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage originalImage = reader.read();
    if (originalImage.isNull()) {
        QMessageBox::information(
            this, QGuiApplication::applicationDisplayName(),
            tr("Cannot load %1, %2")
                .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    orig = cv::imread(fileName.toStdString());

    // Run Canny + Otsu pipeline
    autoCanny(&orig, &cannyDst);
    std::vector<double> otsus_areas =
        polyApproxFromEdges(&cannyDst, &polyDst, &orig);
    ui->imgDisp_lr->setPixmap(ASM::cvMatToQPixmap(polyDst));

    // Run Blob detection pipeline
    std::vector<double> blob_areas = detectBlobs(orig, blobDst);
    ui->imgDisp_ur->setPixmap(ASM::cvMatToQPixmap(blobDst));

    // Build eggMeasurement entries
    std::vector<eggMeasurement> measurements;
    int numEggs = std::max(otsus_areas.size(), blob_areas.size());
    for (int i = 0; i < numEggs; ++i) {
        eggMeasurement egg;
        egg.eggLabel = i + 1;
        egg.otsuArea = (i < otsus_areas.size()) ? otsus_areas[i] : -1.0;
        egg.blobArea = (i < blob_areas.size()) ? blob_areas[i] : -1.0;
        egg.computeAvgArea();
        egg.computeWidths();
        egg.computeConfidence();  // for now returns dummy value
        measurements.push_back(egg);
    }

    // Store into a single-image results map
    eggResults results;
    results.imageMeasurements[fileName.toStdString()] = measurements;

    // Display into table
    displayResultsTable(results, fileName.toStdString());

    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->resizeRowsToContents();

    return true;
}

void eggsizerML::displayResultsTable(const eggResults &results, const std::string &imageName) {
    auto it = results.imageMeasurements.find(imageName);
    if (it == results.imageMeasurements.end()) {
        QMessageBox::warning(this, "No Data", "No measurement data for selected image.");
        return;
    }

    const std::vector<eggMeasurement> &measurements = it->second;
    ui->tableWidget->setRowCount(static_cast<int>(measurements.size()));
    ui->tableWidget->setColumnCount(6);  // Adjust the column count to 6 (Remove Label column)

    QStringList headers = {"ID", "Otsu Area", "Blob Area", "Otsu Width", "Blob Width", "Confidence"};
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < measurements.size(); ++i) {
        const eggMeasurement &egg = measurements[i];

        // Display ID, instead of having a separate "Label"
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(egg.eggLabel)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(egg.otsuArea)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(egg.blobArea)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(egg.otsuWidth)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(egg.blobWidth)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(egg.confidenceScore)));
    }
}

void eggsizerML::outputResult(eggResults results, const QString &filename,
                              int outputFormat, int emitImage) {
  // output formats are as follows:
  // 0 = CSV (default)
  // 1 = JSON

  // open file
  QFile file(filename);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Error"),
                         tr("Could not open file for writing"));
    return;
  }
  QTextStream out(&file);
  // CSV output
  // write headers (Image Name, Egg no., Avg. Area, Otsus Area, Blob Area,
  // Confidence)
  out << "Image Name, Egg No., Avg. Area, Otsus Area, Blob Area, Confidence\n";

  // for each pair in results object
  for (auto &pair : results.imageMeasurements) {
    const std::string &imageName = pair.first; // Image Name
    const std::vector<eggMeasurement> &measurements = pair.second;

    // for each egg measurement in the vector
    for (const eggMeasurement &egg : measurements) {
      QString eggNo = QString::number(egg.eggLabel);             // Egg No.
      QString avgArea = QString::number(egg.avgArea, 'f', 4);    // Avg. Area
      QString otsusArea = QString::number(egg.otsuArea, 'f', 2); // Otsus Area
      QString blobArea = QString::number(egg.blobArea, 'f', 2);  // Blob Area
      QString confidenceScore =
          QString::number(egg.confidenceScore, 'f', 2); // Confidence

      if (outputFormat == 0) {                           // CSV output
        out << QString::fromStdString(imageName) << ","; // Image Name
        out << eggNo << ",";                             // Egg No.
        out << avgArea << ",";                           // Avg. Area
        out << otsusArea << ",";                         // Otsus Area
        out << blobArea << ",";                          // Blob Area
        out << confidenceScore << "\n";                  // Certainty
      } else if (outputFormat == 1) {                    // JSON output
        out << "{\n";
        out << "  \"Image Name\": \"" << QString::fromStdString(imageName)
            << "\",\n";
        out << "  \"Egg No.\": " << eggNo << ",\n";
        out << "  \"Avg. Area\": " << avgArea << ",\n";
        out << "  \"Otsus Area\": " << otsusArea << ",\n";
        out << "  \"Blob Area\": " << blobArea << ",\n";
        out << "  \"Confidence\": " << confidenceScore << "\n";
        out << "}\n";
      } else {
        QMessageBox::warning(this, tr("Error"),
                             tr("Invalid output format selected"));
      }
    }
  }

  return;
}

void eggsizerML::showNextImage() {
  if (currentImageIndex < imageFiles.size() - 1) {
    loadImageAtIndex(++currentImageIndex);
  }
}

void eggsizerML::showPreviousImage() {
  if (currentImageIndex > 0) {
    loadImageAtIndex(--currentImageIndex);
  }
}

// < ---------------------------------------- >

// < ---------------------------------------- >
// SLOT CONNECTORS (BASICALLY CALLBACKS)
void eggsizerML::on_fileOpen_btn_clicked() { open(); }
void eggsizerML::on_folderOpen_btn_clicked() {}
// < ---------------------------------------- >

void eggsizerML::on_saveResults_btn_clicked() {
  if (imageFiles.isEmpty()) {
    QMessageBox::warning(this, tr("Error"),
                         tr("No images loaded. Please load images first."));
    return;
  }

  eggResults results;

  for (const QString &filePath : imageFiles) {
    // Load the image
    orig = cv::imread(filePath.toStdString());
    if (orig.empty()) {
      QMessageBox::warning(this, tr("Error"),
                           tr("Failed to load image: %1").arg(filePath));
      continue;
    }

    // Process the image
    cannyDst.release();
    polyDst.release();
    blobDst.release();

    autoCanny(&orig, &cannyDst);
    std::vector<double> otsus_areas =
        polyApproxFromEdges(&cannyDst, &polyDst, &orig);
    std::vector<double> blob_areas = detectBlobs(orig, blobDst);
    int longer_areas = std::max(otsus_areas.size(), blob_areas.size());
    std::string img_name =
        filePath
            .section('/', -1)  // Get the last section after '/'
            .section('\\', -1) // In case of Windows path
            .toStdString();

    // Store results
    for (int i = 0; i < longer_areas; i++) {
      eggMeasurement egg;
      egg.eggLabel = i + 1; // Store the image name as egg label
      egg.otsuArea = otsus_areas.size() > i ? otsus_areas[i] : -1.0;
      egg.blobArea = blob_areas.size() > i ? blob_areas[i] : -1.0;
      egg.computeWidths();  // Compute widths from areas
      egg.computeAvgArea(); // Compute average area

      // Store the egg measurement in results
      results.imageMeasurements[img_name].push_back(egg);
    }
    results.computeConfidence();
  }

  // store results in Downloadsresults.csv
  QString resultsFile = QFileDialog::getSaveFileName(
      this, tr("Save Results"),
      QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) +
          "/results.csv",
      tr("CSV Files (*.csv);;JSON Files (*.json)"));
  outputResult(results, resultsFile, 0,
               0); // 0 for CSV output, 0 for no image emit

  return;
}
