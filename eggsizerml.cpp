#include "eggsizerml.h"
#include "./ui_eggsizerml.h"

eggsizerML::eggsizerML(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::eggsizerML)
{
    ui->setupUi(this);
}

eggsizerML::~eggsizerML()
{
    delete ui;
}

// < ---------------------------------------- >
// UTILITIES
QPixmap matToPixmap(cv::Mat src)
{
    // assume the image is grayscale, then
    // swap to color according to channels
    QImage::Format format = QImage::Format_Grayscale8;
    int bpp=src.channels();
    if (bpp==3) format = QImage::Format_RGB888;

    QImage img(src.cols, src.rows, format);
    uchar *sptr, *dptr;
    int linesize=src.cols*bpp;

    // scan the matrix into the image
    for (int y=0; y<src.rows; y++) {
        sptr=src.ptr(y);
        dptr=img.scanLine(y);
        memcpy(dptr, sptr, linesize);
    }

    if (bpp==3) return QPixmap::fromImage(img.rgbSwapped());
    return QPixmap::fromImage(img);
}
// < ---------------------------------------- >

// < ---------------------------------------- >
// BASIC IMAGE UPLOAD/PROCESSING WITH OPENCV
static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    // upon first dialog, figure out the most convenient
    // path for the file explorer to open up to
    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    // #NOTE: may want to expand acceptable types in future
    // compile list of acceptable mime types for file explorer
    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen ?
                                                  QImageReader::supportedMimeTypes() :
                                                  QImageWriter::supportedMimeTypes();
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
void eggsizerML::open()
{
    QFileDialog dialog(this, tr("Select Image"));

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().constFirst())) {}
}

// loads file into label element
bool eggsizerML::loadFile(const QString &fileName)
{
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1, %2").arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    ui->imgDisp_1->setPixmap(QPixmap::fromImage(newImage));

    cv::Mat image = cv::imread(fileName.toStdString());
    cv::Mat grayImg;
    cv::cvtColor(image, grayImg, cv::COLOR_RGBA2GRAY);
    ui->imgDisp_2->setPixmap(matToPixmap(grayImg));
    // cv::namedWindow("TestCV2");
    // cv::imshow("TestCV2", grayImg);
    return true;
}

// button connect
void eggsizerML::on_fileOpen_btn_clicked()
{
    open();
}
// < ---------------------------------------- >
