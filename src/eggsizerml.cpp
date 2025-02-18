#include "../include/eggsizerml.h"
#include "../include/ui_eggsizerml.h"
#include "../include/asmOpenCV.h"
#include "../include/cannyDetect.h"

cv::Mat src; // current image, unanalyzed
cv::Mat dst; // current image, analyzed

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

// loads file into label element
bool eggsizerML::loadFile(const QString &fileName="")
{
    extern cv::Mat src;
    // QString testfileName = "/Users/jax/Downloads/data/LT_Eggs/23Y6115_2.jpg";
    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1, %2").arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    ui->imgDisp_1->setPixmap(QPixmap::fromImage(newImage));

    src = cv::imread(fileName.toStdString());
    cv::Mat testImg;
    autoCanny(src, &testImg);
    ui->imgDisp_2->setPixmap(ASM::cvMatToQPixmap(testImg));
    return true;
}

// < ---------------------------------------- >


// < ---------------------------------------- >
// SLOT CONNECTORS
void eggsizerML::on_horizontalSlider_sliderMoved(int position)
{
    extern cv::Mat src;
    if (!src.empty()) {
        float newSigma = ui->horizontalSlider->value() / 100.0f;
        ui->sigma_val_label->setText(QString::number(newSigma));
        autoCanny(src, &dst, newSigma);
        ui->imgDisp_2->setPixmap(ASM::cvMatToQPixmap(dst));
    }
}

void eggsizerML::on_fileOpen_btn_clicked()
{
    open();
}
// < ---------------------------------------- >
