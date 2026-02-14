#include "operationlistwidget.h"
#include "operationcard.h"
#include <QLabel>

OperationListWidget::OperationListWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void OperationListWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    
    // Title
    QLabel *titleLabel = new QLabel(tr("Operations"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);
    
    // Scroll area for operations
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    
    scrollWidget = new QWidget();
    operationsLayout = new QVBoxLayout(scrollWidget);
    operationsLayout->setContentsMargins(5, 5, 5, 5);
    operationsLayout->setSpacing(10);
    operationsLayout->addStretch();
    
    scrollWidget->setLayout(operationsLayout);
    scrollArea->setWidget(scrollWidget);
    mainLayout->addWidget(scrollArea, 1);
    
    // Add operation button
    addButton = new QPushButton(tr("+ Add Operation"), this);
    connect(addButton, &QPushButton::clicked, this, &OperationListWidget::onAddOperation);
    mainLayout->addWidget(addButton);
}

QList<QPair<QString, QString>> OperationListWidget::getOperations() const
{
    QList<QPair<QString, QString>> operations;
    for (OperationCard *card : operationCards) {
        operations.append(QPair<QString, QString>(
            card->getOperationType(),
            card->getOperationValue()
        ));
    }
    return operations;
}

void OperationListWidget::addOperation()
{
    onAddOperation();
}

void OperationListWidget::onAddOperation()
{
    OperationCard *card = new OperationCard(scrollWidget);
    
    connect(card, &OperationCard::operationChanged, 
            this, &OperationListWidget::onOperationChanged);
    connect(card, &OperationCard::removeRequested, 
            this, &OperationListWidget::onRemoveOperation);
    connect(card, &OperationCard::moveUpRequested, 
            this, &OperationListWidget::onMoveOperationUp);
    connect(card, &OperationCard::moveDownRequested, 
            this, &OperationListWidget::onMoveOperationDown);
    
    operationCards.append(card);
    
    // Insert before the stretch
    int index = operationsLayout->count() - 1;
    operationsLayout->insertWidget(index, card);
    
    updateOperationButtons();
    emit operationsChanged();
}

void OperationListWidget::onRemoveOperation()
{
    OperationCard *card = qobject_cast<OperationCard*>(sender());
    if (!card) return;
    
    int index = operationCards.indexOf(card);
    if (index >= 0) {
        operationCards.removeAt(index);
        operationsLayout->removeWidget(card);
        card->deleteLater();
        
        updateOperationButtons();
        emit operationsChanged();
    }
}

void OperationListWidget::onMoveOperationUp()
{
    OperationCard *card = qobject_cast<OperationCard*>(sender());
    if (!card) return;
    
    int index = operationCards.indexOf(card);
    if (index > 0) {
        // Swap in list
        operationCards.swap(index, index - 1);
        
        // Swap in layout
        operationsLayout->removeWidget(card);
        operationsLayout->insertWidget(index - 1, card);
        
        updateOperationButtons();
        emit operationsChanged();
    }
}

void OperationListWidget::onMoveOperationDown()
{
    OperationCard *card = qobject_cast<OperationCard*>(sender());
    if (!card) return;
    
    int index = operationCards.indexOf(card);
    if (index >= 0 && index < operationCards.size() - 1) {
        // Swap in list
        operationCards.swap(index, index + 1);
        
        // Swap in layout
        operationsLayout->removeWidget(card);
        operationsLayout->insertWidget(index + 1, card);
        
        updateOperationButtons();
        emit operationsChanged();
    }
}

void OperationListWidget::onOperationChanged()
{
    emit operationsChanged();
}

void OperationListWidget::updateOperationButtons()
{
    // This could be used to enable/disable move buttons based on position
    // For now, we keep it simple
}
