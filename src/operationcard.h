#ifndef OPERATIONCARD_H
#define OPERATIONCARD_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QTimer>

class OperationCard : public QFrame
{
    Q_OBJECT

public:
    explicit OperationCard(QWidget *parent = nullptr);
    
    QString getOperationType() const;
    QString getOperationValue() const;
    void setOperationType(const QString &type);
    void setOperationValue(const QString &value);

signals:
    void operationChanged();
    void removeRequested();
    void moveUpRequested();
    void moveDownRequested();

private slots:
    void onOperationTypeChanged(int index);
    void onTextChanged();

private:
    void setupUI();
    void updateValueFieldVisibility();

    QComboBox *operationTypeCombo;
    QLineEdit *valueEdit;
    QLineEdit *replacementEdit;
    QLabel *valueLabel;
    QLabel *replacementLabel;
    QPushButton *removeButton;
    QPushButton *moveUpButton;
    QPushButton *moveDownButton;
    QTimer *debounceTimer;
};

#endif // OPERATIONCARD_H
