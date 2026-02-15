#include "filelistwidget.h"
#include "operation.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QMenu>
#include <QtConcurrent>
#include <QFuture>
#include <QDebug>

FileListWidget::FileListWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    
    // Initialize the watcher for async preview generation
    previewWatcher = new QFutureWatcher<QString>(this);
    connect(previewWatcher, &QFutureWatcher<QString>::finished,
            this, &FileListWidget::onPreviewsReady);
}

void FileListWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Title
    QLabel *titleLabel = new QLabel(tr("Files to Rename"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // Tree widget
    treeWidget = new QTreeWidget(this);
    treeWidget->setColumnCount(3);
    treeWidget->setHeaderLabels({tr("Original Name"), tr("New Name"), tr("File Path")});
    treeWidget->setAlternatingRowColors(true);
    treeWidget->setSortingEnabled(true);
    treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    
    // Set column widths
    // Use Interactive mode to allow manual column resizing by the user
    // The last section will stretch to fill available space
    treeWidget->header()->setStretchLastSection(true);
    treeWidget->header()->setSectionResizeMode(0, QHeaderView::Interactive);
    treeWidget->header()->setSectionResizeMode(1, QHeaderView::Interactive);
    treeWidget->header()->setSectionResizeMode(2, QHeaderView::Stretch);
    
    // Set reasonable initial column widths
    treeWidget->setColumnWidth(0, 200);  // Original Name column
    treeWidget->setColumnWidth(1, 200);  // New Name column
    // Column 2 (File Path) will stretch to fill remaining space
    
    connect(treeWidget, &QTreeWidget::itemSelectionChanged,
            this, &FileListWidget::onItemSelectionChanged);
    connect(treeWidget, &QTreeWidget::customContextMenuRequested,
            this, &FileListWidget::showContextMenu);
    
    mainLayout->addWidget(treeWidget, 1);
}

void FileListWidget::addFiles(const QStringList &filePaths)
{
    // Disable updates during batch addition to improve performance
    treeWidget->setUpdatesEnabled(false);
    
    // Reserve space in the files list if we know the approximate size
    files.reserve(files.size() + filePaths.size());
    
    for (const QString &filePath : filePaths) {
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            continue;
        }
        
        // Fast duplicate check using QSet (O(1) instead of O(n))
        if (filePathsSet.contains(filePath)) {
            continue;
        }
        
        FileEntry entry;
        entry.fullPath = filePath;
        entry.directory = fileInfo.absolutePath();
        entry.originalName = fileInfo.fileName();
        entry.newName = fileInfo.fileName();
        
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
        item->setText(0, entry.originalName);
        item->setText(1, entry.newName);
        item->setText(2, entry.directory);
        entry.item = item;
        
        files.append(entry);
        filePathsSet.insert(filePath);
    }
    
    // Re-enable updates and trigger a single repaint
    treeWidget->setUpdatesEnabled(true);
    
    emit filesChanged();
}

void FileListWidget::clearFiles()
{
    treeWidget->clear();
    files.clear();
    filePathsSet.clear();
    emit filesChanged();
}

void FileListWidget::updatePreviews(const QList<std::shared_ptr<Operation>> &operations)
{
    // Cancel any pending preview computation
    if (previewWatcher->isRunning()) {
        previewWatcher->cancel();
        previewWatcher->waitForFinished();
    }
    
    // If no files, nothing to do
    if (files.isEmpty()) {
        return;
    }
    
    // Create a lambda that captures operations and applies them to a filename
    // Note: We capture 'operations' by value to ensure it remains valid
    // We don't capture 'this' as applyOperations is now static and thread-safe
    auto applyOpsFunc = [operations](const QString &fileName) -> QString {
        // Extract the file index from a specially formatted string
        // Format: "index|filename"
        int sepIndex = fileName.indexOf('|');
        if (sepIndex >= 0) {
            int fileIndex = fileName.left(sepIndex).toInt();
            QString actualFileName = fileName.mid(sepIndex + 1);
            return FileListWidget::applyOperations(actualFileName, operations, fileIndex);
        }
        return FileListWidget::applyOperations(fileName, operations, 0);
    };
    
    // Extract original filenames for parallel processing, prepending the index
    QList<QString> originalNames;
    originalNames.reserve(files.size());
    for (int i = 0; i < files.size(); ++i) {
        originalNames.append(QString("%1|%2").arg(i).arg(files[i].originalName));
    }
    
    // Start parallel computation of new names
    QFuture<QString> future = QtConcurrent::mapped(originalNames, applyOpsFunc);
    previewWatcher->setFuture(future);
}

int FileListWidget::applyRename(QStringList &errors)
{
    int successCount = 0;
    errors.clear();
    
    for (int i = 0; i < files.size(); ++i) {
        if (files[i].newName == files[i].originalName) {
            continue; // No change, skip
        }
        
        QString oldPath = files[i].fullPath;
        QString newPath = files[i].directory + QDir::separator() + files[i].newName;
        
        // Check if target file already exists
        if (QFile::exists(newPath)) {
            errors.append(tr("Cannot rename '%1': target file '%2' already exists")
                         .arg(files[i].originalName)
                         .arg(files[i].newName));
            continue;
        }
        
        // Try to rename
        QFile file(oldPath);
        if (file.rename(newPath)) {
            files[i].fullPath = newPath;
            files[i].originalName = files[i].newName;
            files[i].item->setText(0, files[i].newName);
            files[i].item->setForeground(1, QBrush(Qt::black));
            QFont font = files[i].item->font(1);
            font.setBold(false);
            files[i].item->setFont(1, font);
            successCount++;
        } else {
            errors.append(tr("Failed to rename '%1': %2")
                         .arg(files[i].originalName)
                         .arg(file.errorString()));
        }
    }
    
    return successCount;
}

void FileListWidget::onItemSelectionChanged()
{
    // Can be used for future enhancements
}

void FileListWidget::onPreviewsReady()
{
    // Get results from the parallel computation
    QList<QString> results = previewWatcher->future().results();
    
    // Validate that the number of results matches the number of files
    // If they don't match, files may have been added/removed during computation
    // In this case, we skip the update as it would be inconsistent
    if (results.size() != files.size()) {
        qWarning() << "Preview results size mismatch: expected" << files.size() 
                   << "but got" << results.size() << "- skipping update";
        return;
    }
    
    // Disable updates during batch update to improve performance
    treeWidget->setUpdatesEnabled(false);
    
    // Update UI with the computed new names (this runs in the main thread)
    for (int i = 0; i < files.size(); ++i) {
        QString newName = results[i];
        files[i].newName = newName;
        files[i].item->setText(1, newName);
        
        // Highlight changes
        if (newName != files[i].originalName) {
            files[i].item->setForeground(1, QBrush(Qt::darkGreen));
            QFont font = files[i].item->font(1);
            font.setBold(true);
            files[i].item->setFont(1, font);
        } else {
            files[i].item->setForeground(1, QBrush(Qt::black));
            QFont font = files[i].item->font(1);
            font.setBold(false);
            files[i].item->setFont(1, font);
        }
    }
    
    // Re-enable updates and trigger a single repaint
    treeWidget->setUpdatesEnabled(true);
}

QString FileListWidget::applyOperations(const QString &fileName, 
                                       const QList<std::shared_ptr<Operation>> &operations,
                                       int fileIndex)
{
    QString result = fileName;
    
    for (const auto &op : operations) {
        if (op) {
            result = op->perform(result, fileIndex);
        }
    }
    
    return result;
}

void FileListWidget::showContextMenu(const QPoint &pos)
{
    // Only show context menu if there are selected items
    QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }
    
    QMenu contextMenu(this);
    QAction *removeAction = contextMenu.addAction(tr("Remove Selected"));
    connect(removeAction, &QAction::triggered, this, &FileListWidget::removeSelectedFiles);
    
    contextMenu.exec(treeWidget->mapToGlobal(pos));
}

void FileListWidget::removeSelectedFiles()
{
    QList<QTreeWidgetItem*> selectedItems = treeWidget->selectedItems();
    if (selectedItems.isEmpty()) {
        return;
    }
    
    // Disable updates during batch removal
    treeWidget->setUpdatesEnabled(false);
    
    // Create a set of items to remove for quick lookup
    QSet<QTreeWidgetItem*> itemsToRemove;
    for (QTreeWidgetItem *item : selectedItems) {
        itemsToRemove.insert(item);
    }
    
    // Remove files from the list in reverse order to maintain indices
    for (int i = files.size() - 1; i >= 0; --i) {
        if (itemsToRemove.contains(files[i].item)) {
            // Update filePathsSet to reflect removal
            filePathsSet.remove(files[i].fullPath);
            
            // Remove the tree widget item
            delete files[i].item;
            
            // Remove from the files list
            files.removeAt(i);
        }
    }
    
    // Re-enable updates
    treeWidget->setUpdatesEnabled(true);
    
    emit filesChanged();
}
