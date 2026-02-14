#include "operationcard.h"
#include <QGroupBox>

OperationCard::OperationCard(QWidget *parent)
    : QFrame(parent)
{
    // Create debounce timer for text input
    debounceTimer = new QTimer(this);
    debounceTimer->setSingleShot(true);
    debounceTimer->setInterval(300); // 300ms delay after last keystroke
    connect(debounceTimer, &QTimer::timeout, this, &OperationCard::operationChanged);
    
    setupUI();
}

void OperationCard::setupUI()
{
    // Set object name for styling
    setObjectName("operationCard");
    
    // Add margin around the card to separate it from other cards
    setContentsMargins(4, 4, 4, 4);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(8);
    
    // Operation type row
    QHBoxLayout *typeLayout = new QHBoxLayout();
    QLabel *typeLabel = new QLabel(tr("Operation:"), this);
    operationTypeCombo = new QComboBox(this);
    operationTypeCombo->addItem(tr("Replace"), "replace");
    operationTypeCombo->addItem(tr("Add Prefix"), "prefix");
    operationTypeCombo->addItem(tr("Add Suffix"), "suffix");
    operationTypeCombo->addItem(tr("Insert Text"), "insert");
    operationTypeCombo->addItem(tr("Change Extension"), "change_ext");
    
    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(operationTypeCombo, 1);
    mainLayout->addLayout(typeLayout);
    
    // Value/Pattern row
    QHBoxLayout *valueLayout = new QHBoxLayout();
    valueLabel = new QLabel(tr("Pattern:"), this);
    valueEdit = new QLineEdit(this);
    valueEdit->setPlaceholderText(tr("Enter pattern..."));
    valueLayout->addWidget(valueLabel);
    valueLayout->addWidget(valueEdit, 1);
    mainLayout->addLayout(valueLayout);
    
    // Replacement row (for replace operation)
    QHBoxLayout *replacementLayout = new QHBoxLayout();
    replacementLabel = new QLabel(tr("Replace with:"), this);
    replacementEdit = new QLineEdit(this);
    replacementEdit->setPlaceholderText(tr("Enter replacement..."));
    replacementLayout->addWidget(replacementLabel);
    replacementLayout->addWidget(replacementEdit, 1);
    mainLayout->addLayout(replacementLayout);
    
    // Buttons row
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    moveUpButton = new QPushButton(tr("↑"), this);
    moveUpButton->setMaximumWidth(40);
    moveDownButton = new QPushButton(tr("↓"), this);
    moveDownButton->setMaximumWidth(40);
    removeButton = new QPushButton(tr("Remove"), this);
    
    buttonLayout->addWidget(moveUpButton);
    buttonLayout->addWidget(moveDownButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(removeButton);
    mainLayout->addLayout(buttonLayout);
    
    // Connect signals
    connect(operationTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OperationCard::onOperationTypeChanged);
    connect(valueEdit, &QLineEdit::textChanged, this, &OperationCard::onTextChanged);
    connect(replacementEdit, &QLineEdit::textChanged, this, &OperationCard::onTextChanged);
    connect(removeButton, &QPushButton::clicked, this, &OperationCard::removeRequested);
    connect(moveUpButton, &QPushButton::clicked, this, &OperationCard::moveUpRequested);
    connect(moveDownButton, &QPushButton::clicked, this, &OperationCard::moveDownRequested);
    
    updateValueFieldVisibility();
}

void OperationCard::onTextChanged()
{
    // Restart the debounce timer on each keystroke
    // This delays the operationChanged signal until user stops typing
    debounceTimer->start();
}

QString OperationCard::getOperationType() const
{
    return operationTypeCombo->currentData().toString();
}

QString OperationCard::getOperationValue() const
{
    return valueEdit->text();
}

QString OperationCard::getReplacementValue() const
{
    return replacementEdit->text();
}

void OperationCard::setOperationType(const QString &type)
{
    for (int i = 0; i < operationTypeCombo->count(); ++i) {
        if (operationTypeCombo->itemData(i).toString() == type) {
            operationTypeCombo->setCurrentIndex(i);
            break;
        }
    }
}

void OperationCard::setOperationValue(const QString &value)
{
    valueEdit->setText(value);
}

void OperationCard::onOperationTypeChanged(int index)
{
    Q_UNUSED(index);
    updateValueFieldVisibility();
    emit operationChanged();
}

void OperationCard::updateValueFieldVisibility()
{
    QString type = getOperationType();
    
    if (type == "replace") {
        valueLabel->setText(tr("Pattern:"));
        valueLabel->show();
        valueEdit->show();
        valueEdit->setPlaceholderText(tr("Enter regex pattern..."));
        replacementLabel->show();
        replacementEdit->show();
    } else if (type == "prefix") {
        valueLabel->setText(tr("Prefix:"));
        valueLabel->show();
        valueEdit->show();
        valueEdit->setPlaceholderText(tr("Enter prefix text..."));
        replacementLabel->hide();
        replacementEdit->hide();
    } else if (type == "suffix") {
        valueLabel->setText(tr("Suffix:"));
        valueLabel->show();
        valueEdit->show();
        valueEdit->setPlaceholderText(tr("Enter suffix text..."));
        replacementLabel->hide();
        replacementEdit->hide();
    } else if (type == "insert") {
        valueLabel->setText(tr("Position:"));
        valueLabel->show();
        valueEdit->show();
        valueEdit->setPlaceholderText(tr("Enter position (0-based index)..."));
        replacementLabel->setText(tr("Text:"));
        replacementLabel->show();
        replacementEdit->show();
        replacementEdit->setPlaceholderText(tr("Enter text to insert..."));
    } else if (type == "change_ext") {
        valueLabel->setText(tr("New Extension:"));
        valueLabel->show();
        valueEdit->show();
        valueEdit->setPlaceholderText(tr("Enter new extension (e.g., .txt)..."));
        replacementLabel->hide();
        replacementEdit->hide();
    }
}
