#include "mainwindow.h"
#include "operationlistwidget.h"
#include "filelistwidget.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Regex Rename - Batch File Renamer");
    
    // Create debounce timer for preview updates
    // This prevents rapid successive updates during resize/splitter movement
    previewDebounceTimer = new QTimer(this);
    previewDebounceTimer->setSingleShot(true);
    previewDebounceTimer->setInterval(150); // 150ms delay
    connect(previewDebounceTimer, &QTimer::timeout, this, &MainWindow::performPreviewUpdate);
    
    setupMenuBar();
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    
    QAction *addFilesAction = new QAction(tr("&Add Files..."), this);
    addFilesAction->setShortcut(QKeySequence::Open);
    connect(addFilesAction, &QAction::triggered, this, &MainWindow::onAddFiles);
    fileMenu->addAction(addFilesAction);
    
    QAction *clearFilesAction = new QAction(tr("&Clear Files"), this);
    connect(clearFilesAction, &QAction::triggered, this, &MainWindow::onClearFiles);
    fileMenu->addAction(clearFilesAction);
    
    fileMenu->addSeparator();
    
    QAction *applyAction = new QAction(tr("&Apply Rename"), this);
    applyAction->setShortcut(QKeySequence(tr("Ctrl+R")));
    connect(applyAction, &QAction::triggered, this, &MainWindow::onApplyRename);
    fileMenu->addAction(applyAction);
    
    fileMenu->addSeparator();
    
    QAction *exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    fileMenu->addAction(exitAction);
    
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    
    QAction *aboutAction = new QAction(tr("&About"), this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupUI()
{
    // Create main splitter
    splitter = new QSplitter(Qt::Horizontal, this);
    
    // Create operation list on the left
    operationList = new OperationListWidget(this);
    splitter->addWidget(operationList);
    
    // Create file list on the right
    fileList = new FileListWidget(this);
    splitter->addWidget(fileList);
    
    // Set initial splitter sizes (30% left, 70% right)
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 7);
    
    setCentralWidget(splitter);
    
    // Connect operations changes to preview updates
    // Use the debounced update request method
    connect(operationList, &OperationListWidget::operationsChanged, 
            this, &MainWindow::onUpdatePreviewsRequested);
    connect(fileList, &FileListWidget::filesChanged,
            this, &MainWindow::onUpdatePreviewsRequested);
}

void MainWindow::onAddFiles()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Select Files to Rename"),
        QString(),
        tr("All Files (*)")
    );
    
    if (!fileNames.isEmpty()) {
        fileList->addFiles(fileNames);
    }
}

void MainWindow::onClearFiles()
{
    fileList->clearFiles();
}

void MainWindow::onApplyRename()
{
    QStringList errors;
    int successCount = fileList->applyRename(errors);
    
    QString message;
    if (errors.isEmpty()) {
        message = tr("Successfully renamed %1 file(s).").arg(successCount);
        QMessageBox::information(this, tr("Rename Complete"), message);
    } else {
        message = tr("Renamed %1 file(s).\n\nErrors:\n%2")
                      .arg(successCount)
                      .arg(errors.join("\n"));
        QMessageBox::warning(this, tr("Rename Complete with Errors"), message);
    }
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, tr("About Regex Rename"),
        tr("<h3>Regex Rename v1.0</h3>"
           "<p>A batch file renaming tool with preview functionality.</p>"
           "<p>Use operations to define how files should be renamed, "
           "preview the changes, and apply them when ready.</p>"));
}

void MainWindow::onUpdatePreviewsRequested()
{
    // Restart the debounce timer on each request
    // This delays the actual preview update until requests stop coming
    previewDebounceTimer->start();
}

void MainWindow::performPreviewUpdate()
{
    // This is the actual update that happens after debouncing
    updatePreviews();
}

void MainWindow::updatePreviews()
{
    QList<std::shared_ptr<Operation>> operations = operationList->getOperations();
    fileList->updatePreviews(operations);
}
