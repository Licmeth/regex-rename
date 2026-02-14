#ifndef FILELISTWIDGET_H
#define FILELISTWIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QStringList>
#include <QFileInfo>
#include <QPair>
#include <QList>

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

private:
    void setupUI();
    QString applyOperations(const QString &fileName, 
                           const QList<QPair<QString, QString>> &operations);

    QTreeWidget *treeWidget;
    QList<FileEntry> files;
};

#endif // FILELISTWIDGET_H
