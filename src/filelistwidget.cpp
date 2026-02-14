#include "filelistwidget.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QFile>
#include <QDir>
#include <QRegularExpression>
#include <QtConcurrent>
#include <QFuture>

FileListWidget::FileListWidget(QWidget *parent)
    : QWidget(parent), previewWatcher(nullptr)
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
    treeWidget->setHeaderLabels({tr("File Path"), tr("Original Name"), tr("New Name")});
    treeWidget->setAlternatingRowColors(true);
    treeWidget->setSortingEnabled(false);
    treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    // Set column widths
    treeWidget->header()->setStretchLastSection(false);
    treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    
    connect(treeWidget, &QTreeWidget::itemSelectionChanged,
            this, &FileListWidget::onItemSelectionChanged);
    
    mainLayout->addWidget(treeWidget, 1);
}

void FileListWidget::addFiles(const QStringList &filePaths)
{
    for (const QString &filePath : filePaths) {
        QFileInfo fileInfo(filePath);
        if (!fileInfo.exists() || !fileInfo.isFile()) {
            continue;
        }
        
        // Check if file already exists in list
        bool exists = false;
        for (const FileEntry &entry : files) {
            if (entry.fullPath == filePath) {
                exists = true;
                break;
            }
        }
        
        if (exists) continue;
        
        FileEntry entry;
        entry.fullPath = filePath;
        entry.directory = fileInfo.absolutePath();
        entry.originalName = fileInfo.fileName();
        entry.newName = fileInfo.fileName();
        
        QTreeWidgetItem *item = new QTreeWidgetItem(treeWidget);
        item->setText(0, entry.directory);
        item->setText(1, entry.originalName);
        item->setText(2, entry.newName);
        entry.item = item;
        
        files.append(entry);
    }
    
    emit filesChanged();
}

void FileListWidget::clearFiles()
{
    treeWidget->clear();
    files.clear();
    emit filesChanged();
}

void FileListWidget::updatePreviews(const QList<QPair<QString, QString>> &operations)
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
        return FileListWidget::applyOperations(fileName, operations);
    };
    
    // Extract original filenames for parallel processing
    QList<QString> originalNames;
    originalNames.reserve(files.size());
    for (const auto &file : files) {
        originalNames.append(file.originalName);
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
            files[i].item->setText(1, files[i].newName);
            files[i].item->setForeground(2, QBrush(Qt::black));
            QFont font = files[i].item->font(2);
            font.setBold(false);
            files[i].item->setFont(2, font);
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
        return;
    }
    
    // Update UI with the computed new names (this runs in the main thread)
    for (int i = 0; i < files.size(); ++i) {
        QString newName = results[i];
        files[i].newName = newName;
        files[i].item->setText(2, newName);
        
        // Highlight changes
        if (newName != files[i].originalName) {
            files[i].item->setForeground(2, QBrush(Qt::darkGreen));
            QFont font = files[i].item->font(2);
            font.setBold(true);
            files[i].item->setFont(2, font);
        } else {
            files[i].item->setForeground(2, QBrush(Qt::black));
            QFont font = files[i].item->font(2);
            font.setBold(false);
            files[i].item->setFont(2, font);
        }
    }
}

QString FileListWidget::applyOperations(const QString &fileName, 
                                       const QList<QPair<QString, QString>> &operations)
{
    QString result = fileName;
    
    for (const auto &op : operations) {
        QString type = op.first;
        QString value = op.second;
        
        if (type == "replace") {
            // Split value into pattern and replacement
            QStringList parts = value.split("|");
            if (parts.size() >= 2) {
                QString pattern = parts[0];
                QString replacement = parts[1];
                
                QRegularExpression regex(pattern);
                if (regex.isValid()) {
                    result = result.replace(regex, replacement);
                }
            }
        } else if (type == "prefix") {
            result = value + result;
        } else if (type == "suffix") {
            // Add suffix before extension
            // Note: dotIndex > 0 ensures we don't treat dotfiles (like .bashrc) as having extensions
            int dotIndex = result.lastIndexOf('.');
            if (dotIndex > 0) {
                result = result.left(dotIndex) + value + result.mid(dotIndex);
            } else {
                result = result + value;
            }
        } else if (type == "remove_ext") {
            // Remove extension, but preserve dotfiles (like .bashrc)
            int dotIndex = result.lastIndexOf('.');
            if (dotIndex > 0) {
                result = result.left(dotIndex);
            }
        } else if (type == "change_ext") {
            // Change extension, but preserve dotfiles (like .bashrc)
            int dotIndex = result.lastIndexOf('.');
            if (dotIndex > 0) {
                result = result.left(dotIndex);
            }
            if (!value.isEmpty()) {
                if (!value.startsWith('.')) {
                    result += '.';
                }
                result += value;
            }
        }
    }
    
    return result;
}
