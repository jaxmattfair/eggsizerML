#include "../include/eggsizerml.h"
#include "../include/ui_eggsizerml.h"
#include "../include/asmOpenCV.h"
#include "../include/cannyDetect.h"
#include "../include/blobDetect.h"
#include "../include/measureEdges.h"

// GLOBAL APPLICATION DATA STORAGE
//(keep this to a MINIMUM)
cv::Mat orig; // original image
cv::Mat cannyDst; // canny-detected image
cv::Mat polyDst; // polygonally-approximated image
cv::Mat blobDst; // blob-detected image

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
// BASIC FILE/FOLDER UPLOAD/PROCESSING WITH OPENCV
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

// loads file into label element & CV mat for analysis
bool eggsizerML::loadFile(const QString &fileName="")
{
    // validate file and read in
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage originalImage = reader.read();
    if (originalImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1, %2").arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }

    // display input image
    ui->imgDisp_ul->setPixmap(QPixmap::fromImage(originalImage));

    // Otsu's threshold and display
    orig = cv::imread(fileName.toStdString());
    autoCanny(&orig, &cannyDst);
    ui->imgDisp_ll->setPixmap(ASM::cvMatToQPixmap(cannyDst));

    // polygonally approximate and display
    polyApproxFromEdges(&cannyDst, &polyDst, &orig);
    ui->imgDisp_lr->setPixmap(ASM::cvMatToQPixmap(polyDst));

    // blob detect and display
    detectBlobs(orig, blobDst);
    ui->imgDisp_ur->setPixmap(ASM::cvMatToQPixmap(blobDst));

    return true;
}

// < ---------------------------------------- >


// < ---------------------------------------- >
// SLOT CONNECTORS (BASICALLY CALLBACKS)
void eggsizerML::on_fileOpen_btn_clicked()
{
    open();
}
// < ---------------------------------------- >
