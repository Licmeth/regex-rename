#ifndef OPERATIONLISTWIDGET_H
#define OPERATIONLISTWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QList>

class OperationCard;

class OperationListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OperationListWidget(QWidget *parent = nullptr);
    
    QList<QPair<QString, QString>> getOperations() const;
    void addOperation();

signals:
    void operationsChanged();

private slots:
    void onAddOperation();
    void onRemoveOperation();
    void onMoveOperationUp();
    void onMoveOperationDown();
    void onOperationChanged();

private:
    void setupUI();
    void updateOperationButtons();

    QVBoxLayout *operationsLayout;
    QScrollArea *scrollArea;
    QWidget *scrollWidget;
    QPushButton *addButton;
    QList<OperationCard*> operationCards;
};

#endif // OPERATIONLISTWIDGET_H
