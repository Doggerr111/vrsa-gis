#include "vectordatasetform.h"
#include "ui_vectordatasetform.h"

VectorDatasetForm::VectorDatasetForm(QWidget *parent) :
    BaseDatasetForm(parent),
    ui(new Ui::VectorDatasetForm)
{
    ui->setupUi(this);

    ui->labelPath->setTextFormat(Qt::RichText);
    ui->labelPath->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
    ui->labelPath->setCursor(Qt::PointingHandCursor);

    QFont font = ui->labelPath->font();
    font.setUnderline(true);
    ui->labelPath->setFont(font);

    ui->labelPath->setStyleSheet(
                "QLabel { color: #0066cc; }"
                "QLabel:hover { color: #ff6600; text-decoration: underline; }"
                );

    ui->labelPath->installEventFilter(this);
}

VectorDatasetForm::~VectorDatasetForm()
{
    delete ui;
}

void VectorDatasetForm::setTreeItem(QTreeWidgetItem *item)
{
    loadFromItem(item);
    ui->labelPath->setText(mFilePath);
    auto &pM = vrsa::services::ProjectManager::instance();
    auto str = mFilePath.toStdString();

    QWidget *parentWidget = ui->frame_7;
    QVBoxLayout *parentLayout = qobject_cast<QVBoxLayout*>(parentWidget->layout());

    if (!parentLayout)
        return;

    auto names = pM.getLayerNames(str);
    for (int i=0; i<names.size(); ++i)
    {
        if (i==0)
        {
            ui->labelLayer->setText(QString::fromStdString(names[i]));
            continue;
        }
        QFrame *newFrame = new QFrame(parentWidget);
        newFrame->setFrameShape(QFrame::StyledPanel);
        newFrame->setFrameShadow(QFrame::Raised);
        newFrame->setMinimumHeight(30);

        QHBoxLayout *frameLayout = new QHBoxLayout(newFrame);
        frameLayout->setContentsMargins(5, 5, 5, 5);

        QLabel *newLabel = new QLabel(newFrame);
        newLabel->setText(QString::fromStdString(names[i]));
        newLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        frameLayout->addWidget(newLabel);
        parentLayout->addWidget(newFrame);
    }
    parentLayout->addStretch();

}

bool VectorDatasetForm::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->labelPath && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            openFileLocation();
            return true; // Событие обработано
        }
    }
    return QDialog::eventFilter(obj, event);
}

void VectorDatasetForm::openFileLocation() {
    QString filePath = ui->labelPath->text();

    if (filePath.isEmpty())
        return;


    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists()) {
        QMessageBox::warning(this, "Ошибка",
            QString("Файл не существует:\n%1").arg(filePath));
        return;
    }

    QString folderPath = fileInfo.absolutePath();

    // Открываем папку в проводнике/файловом менеджере
    #ifdef Q_OS_WIN
        // Windows: открыть папку и выделить файл
        QStringList args;
        args << "/select," << QDir::toNativeSeparators(filePath);
        QProcess::startDetached("explorer", args);

    #elif defined(Q_OS_MAC)
        // macOS: открыть в Finder и выделить файл
        QStringList args;
        args << "-e" << "tell application \"Finder\"";
        args << "-e" << "activate";
        args << "-e" << "select POSIX file \"" + filePath + "\"";
        args << "-e" << "end tell";
        QProcess::startDetached("osascript", args);

    #else
        // Linux: просто открыть папку
        QDesktopServices::openUrl(QUrl::fromLocalFile(folderPath));
    #endif
}



