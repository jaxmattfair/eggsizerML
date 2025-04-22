#include "../include/eggsizerml.h"
#include "../include/asmOpenCV.h"
#include "../include/blobDetect.h"
#include "../include/cannyDetect.h"
#include "../include/measureEdges.h"
#include "../include/ui_eggsizerml.h"
#include <opencv2/core/mat.hpp>

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

  ui->imgDisp_ul->setMinimumWidth(fixedImageWidth);
  ui->imgDisp_ul->setMaximumWidth(fixedImageWidth);

  ui->imgDisp_ur->setMinimumWidth(fixedImageWidth);
  ui->imgDisp_ur->setMaximumWidth(fixedImageWidth);

  ui->imgDisp_ll->setMinimumWidth(fixedImageWidth);
  ui->imgDisp_ll->setMaximumWidth(fixedImageWidth);

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

  // display input image
  ui->imgDisp_ul->setPixmap(QPixmap::fromImage(originalImage));

  // Otsu's threshold and display
  orig = cv::imread(fileName.toStdString());
  autoCanny(&orig, &cannyDst);
  ui->imgDisp_ll->setPixmap(ASM::cvMatToQPixmap(cannyDst));

  // polygonally approximate and display
  EdgeMeasureResults edgeResults = polyApproxFromEdges(&cannyDst, &polyDst, &orig);
  std::vector<double> otsus_areas = edgeResults.areas;
  ui->imgDisp_lr->setPixmap(ASM::cvMatToQPixmap(polyDst));

  ui->tableWidget->resizeColumnsToContents();
  ui->tableWidget->resizeRowsToContents();

  // blob detect and display
  std::vector<double> blob_areas, blob_widths;
  BlobResults blobResults = detectBlobs(orig, blobDst);
  blob_areas = blobResults.areas;
  blob_widths = blobResults.widths;
  ui->imgDisp_ur->setPixmap(ASM::cvMatToQPixmap(blobDst));

  // display all areas to table (first column egg no, second otsus area, third
  // blob area)
  int numRows = std::max({otsus_areas.size(), blob_areas.size(), blob_widths.size()});
  ui->tableWidget->setRowCount(numRows);
  for (int i = 0; i < numRows; i++) {
      QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(i + 1));
      ui->tableWidget->setItem(i, 0, item1);

      if (i < otsus_areas.size()) {
          QTableWidgetItem *item2 =
              new QTableWidgetItem(QString::number(otsus_areas[i]));
          ui->tableWidget->setItem(i, 1, item2);
      }

      if (i < blob_areas.size()) {
          QTableWidgetItem *item3 =
              new QTableWidgetItem(QString::number(blob_areas[i]));
          ui->tableWidget->setItem(i, 2, item3);
      }

      if (i < blob_widths.size()) {
          QTableWidgetItem *item4 =
              new QTableWidgetItem(QString::number(blob_widths[i], 'f', 2));
          ui->tableWidget->setItem(i, 3, item4);
      }
  }

  return true;
}

void eggsizerML::outputResult(
    const QString &filename, std::vector<std::string> &imageNames,
    const std::vector<int> &eggNumbers, const std::vector<double> &avgAreas,
    const std::vector<double> &otsusAreas, const std::vector<double> &blobAreas,
    const std::vector<double> &certainties, int outputFormat, int emitImage) {
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
  // Certainty)
  out << "Image Name, Egg No., Avg. Area, Otsus Area, Blob Area, Certainty\n";

  // The results object is a vector of vectors:
  // image names vector
  // egg numbers vector (numbers split per image)
  // average areas vector (per egg, averaged between Otsus and Blob areas)
  // otsus areas vector (per egg)
  // blob areas vector (per egg)
  // a vector of certainties
  size_t longest_vec_size =
      std::max({imageNames.size(), eggNumbers.size(), avgAreas.size(),
                otsusAreas.size(), blobAreas.size(), certainties.size()});

  if (longest_vec_size == 0) {
    QMessageBox::warning(this, tr("Error"),
                         tr("No data to output. Results are empty."));
    file.close();
    return;
  }

  for (size_t i = 0; i < longest_vec_size; i++) {
    // collect results or default to "N/A" if out of range
    QString imageName =
        imageNames.size() > i
            ? QString::fromStdString(imageNames[i]) // Image Name
            : QString("N/A"); // Default to "N/A" if no image name
    QString eggNo = eggNumbers.size() > i
                        ? QString::number(eggNumbers[i]) // Egg No.
                        : QString("N/A");
    QString avgArea = avgAreas.size() > i
                          ? QString::number(avgAreas[i], 'f', 4) // Avg. Area
                          : QString("N/A");
    QString otsusArea =
        otsusAreas.size() > i
            ? QString::number(otsusAreas[i], 'f', 2) // Otsus Area
            : QString("N/A");
    QString blobArea = blobAreas.size() > i
                           ? QString::number(blobAreas[i], 'f', 2) // Blob Area
                           : QString("N/A");
    QString certainty =
        certainties.size() > i
            ? QString::number(certainties[i], 'f', 2) // Certainty
            : QString("N/A");

    if (outputFormat == 0) { // CSV output
      // Image Name, Egg No., Avg. Area, Otsus Area, Blob Area, Certainty
      out << imageName << ",";      // Image Name
      out << eggNo << ",";          // Egg No.
      out << avgArea << ",";        // Avg. Area
      out << otsusArea << ",";      // Otsus Area
      out << blobArea << ",";       // Blob Area
      out << certainty << "\n";     // Certainty
    } else if (outputFormat == 1) { // JSON output
      out << "{\n";
      out << "  \"Image Name\": \"" << imageName << "\",\n";
      out << "  \"Egg No.\": " << eggNo << ",\n";
      out << "  \"Avg. Area\": " << avgArea << ",\n";
      out << "  \"Otsus Area\": " << otsusArea << ",\n";
      out << "  \"Blob Area\": " << blobArea << ",\n";
      out << "  \"Certainty\": " << certainty << "\n";
      out << "}\n";
    } else {
      QMessageBox::warning(this, tr("Error"),
                           tr("Invalid output format selected"));
    }
  }
  file.close();
  QMessageBox::information(
      this, tr("Success"),
      tr("Results saved successfully to %1").arg(filename));

  if (emitImage) {
    for (size_t i = 0; i < imageNames.size(); i++) {
      QString imagePath = QString::fromStdString(imageNames[i]);
      QString outputImageName =
          QFileInfo(imagePath).completeBaseName() + "_analyze.png";

      if (!blobDst.empty()) {
        cv::imwrite(outputImageName.toStdString(), blobDst);
      } else {
        QMessageBox::warning(
            this, tr("Error"),
            tr("Failed to save blob image: %1").arg(outputImageName));
      }
      outputImageName = QFileInfo(imagePath).completeBaseName() + "_otsus.png";
      if (!polyDst.empty()) {
        cv::imwrite(outputImageName.toStdString(), polyDst);
      } else {
        QMessageBox::warning(
            this, tr("Error"),
            tr("Failed to save otsus image: %1").arg(outputImageName));
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

  // for all currently selected images, run analysis and concatenate results
  // vectors
  std::vector<std::string> imageNames;
  std::vector<int> eggNumbers;
  std::vector<double> avgAreas;
  std::vector<double> otsusAreas;
  std::vector<double> blobAreas;
  std::vector<double> certainties;

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
    EdgeMeasureResults edgeResults = polyApproxFromEdges(&cannyDst, &polyDst, &orig);
    std::vector<double> otsus_areas = edgeResults.areas;
    BlobResults blobResults = detectBlobs(orig, blobDst);
    std::vector<double> blob_areas = blobResults.areas;
    int longer_areas = std::max(otsus_areas.size(), blob_areas.size());

    // Store results
    for (int i = 0; i < longer_areas; i++) {
      // strip all but the filename
      imageNames.push_back(
          filePath
              .section('/', -1)  // Get the last section after '/'
              .section('\\', -1) // In case of Windows path
              .toStdString());   // Convert to std::string
      eggNumbers.push_back(i + 1);
      double tempOtsusArea = otsus_areas.size() > i ? otsus_areas[i] : -1.0;
      double tempBlobArea = blob_areas.size() > i ? blob_areas[i] : -1.0;
      double avgArea =
          (tempOtsusArea >= 0 && tempBlobArea >= 0)
              ? (tempOtsusArea + tempBlobArea) / 2.0
              : (tempOtsusArea >= 0 ? tempOtsusArea : tempBlobArea);
      avgAreas.push_back(avgArea);
      // Certainty calculation (dummy example)
      double certainty = 0.0;
      if (!otsus_areas.empty()) {
        certainty = 1.0; // Placeholder for actual certainty calculation
        certainties.push_back(certainty);
      } else {
        certainties.push_back(0.0); // No eggs detected
      }
    }

    otsusAreas.insert(otsusAreas.end(), otsus_areas.begin(), otsus_areas.end());
    blobAreas.insert(blobAreas.end(), blob_areas.begin(), blob_areas.end());
  }

  // store results in Downloadsresults.csv
  QString resultsFile = QFileDialog::getSaveFileName(
      this, tr("Save Results"),
      QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) +
          "/results.csv",
      tr("CSV Files (*.csv);;JSON Files (*.json)"));
  outputResult(resultsFile, imageNames, eggNumbers, avgAreas, otsusAreas,
               blobAreas, certainties, 0, 1);

  return;
}
