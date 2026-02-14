#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QFileInfo>
#include <QPair>
#include <QList>
#include <QFutureWatcher>
#include <QVector>

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
    void updatePreviews(const QList<QPair<QString, QString>> &operations);
    int applyRename(QStringList &errors);

signals:
    void filesChanged();

private slots:
    void onItemSelectionChanged();
    void onPreviewsReady();

private:
    void setupUI();
    QString applyOperations(const QString &fileName, 
                           const QList<QPair<QString, QString>> &operations);
    
    QTreeWidget *treeWidget;
    QList<FileEntry> files;
    QFutureWatcher<QString> *previewWatcher;
};

#endif // FILELISTWIDGET_H
