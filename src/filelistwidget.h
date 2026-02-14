#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QFileInfo>
#include <QPair>
#include <QList>
#include <QSet>
#include <QFutureWatcher>
#include <memory>

class Operation;

struct FileEntry {
    QString fullPath;
    QString directory;
    QString originalName;
    QString newName;
    QTreeWidgetItem *item;
};

class FileListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FileListWidget(QWidget *parent = nullptr);
    
    void addFiles(const QStringList &filePaths);
    void clearFiles();
    void updatePreviews(const QList<std::shared_ptr<Operation>> &operations);
    int applyRename(QStringList &errors);

signals:
    void filesChanged();

private slots:
    void onItemSelectionChanged();
    void onPreviewsReady();

private:
    void setupUI();
    static QString applyOperations(const QString &fileName, 
                                   const QList<std::shared_ptr<Operation>> &operations,
                                   int fileIndex = 0);
    
    QTreeWidget *treeWidget;
    QList<FileEntry> files;
    QSet<QString> filePathsSet; // For fast duplicate checking
    QFutureWatcher<QString> *previewWatcher;
};

#endif // FILELISTWIDGET_H
