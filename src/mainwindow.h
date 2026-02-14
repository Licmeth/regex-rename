#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSplitter>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTimer>

class OperationListWidget;
class FileListWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddFiles();
    void onClearFiles();
    void onApplyRename();
    void onAbout();
    void onUpdatePreviewsRequested();
    void performPreviewUpdate();

private:
    void setupMenuBar();
    void setupUI();
    void updatePreviews();

    QSplitter *splitter;
    OperationListWidget *operationList;
    FileListWidget *fileList;
    QTimer *previewDebounceTimer;
};

#endif // MAINWINDOW_H
