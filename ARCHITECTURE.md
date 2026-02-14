# Application Architecture

## UI Layout

```
┌─────────────────────────────────────────────────────────────────────────┐
│ Regex Rename - Batch File Renamer                            [_][□][X]  │
├─────────────────────────────────────────────────────────────────────────┤
│ File   Help                                                              │
├─────────────────┬───────────────────────────────────────────────────────┤
│                 │                                                         │
│  Operations     │  Files to Rename                                       │
│                 │                                                         │
│  ┌───────────┐  │  ┌──────────────────────────────────────────────────┐ │
│  │ Operation │  │  │ File Path │ Original Name │ New Name           │ │
│  │ Replace   │  │  ├──────────────────────────────────────────────────┤ │
│  │ Pattern:  │  │  │ /home/... │ document.txt  │ backup_document... │ │
│  │ [_______] │  │  │ /home/... │ image.jpg     │ backup_image...    │ │
│  │ Replace:  │  │  │ /home/... │ notes.md      │ backup_notes...    │ │
│  │ [_______] │  │  └──────────────────────────────────────────────────┘ │
│  │ [↑][↓][X] │  │                                                         │
│  └───────────┘  │                                                         │
│                 │                                                         │
│  ┌───────────┐  │                                                         │
│  │ Operation │  │                                                         │
│  │ Prefix    │  │                                                         │
│  │ Prefix:   │  │                                                         │
│  │ [backup_] │  │                                                         │
│  │ [↑][↓][X] │  │                                                         │
│  └───────────┘  │                                                         │
│                 │                                                         │
│  [+ Add Op...]  │                                                         │
│                 │                                                         │
└─────────────────┴───────────────────────────────────────────────────────┘
```

## Component Architecture

```
MainWindow
├── QMenuBar
│   ├── File Menu
│   │   ├── Add Files...
│   │   ├── Clear Files
│   │   ├── Apply Rename
│   │   └── Exit
│   └── Help Menu
│       └── About
│
└── QSplitter (Horizontal)
    ├── OperationListWidget (Left 30%)
    │   ├── QScrollArea
    │   │   └── QWidget (scroll content)
    │   │       └── QVBoxLayout
    │   │           ├── OperationCard 1
    │   │           │   ├── QComboBox (operation type)
    │   │           │   ├── QLineEdit (pattern/value)
    │   │           │   ├── QLineEdit (replacement)
    │   │           │   └── QPushButton (↑, ↓, Remove)
    │   │           ├── OperationCard 2
    │   │           └── ...
    │   └── QPushButton (+ Add Operation)
    │
    └── FileListWidget (Right 70%)
        └── QTreeWidget (3 columns)
            ├── Column 0: File Path
            ├── Column 1: Original Name
            └── Column 2: New Name (preview)
```

## Data Flow

```
User Actions
    ↓
┌─────────────────────────────────────────────┐
│ 1. Add Files (File → Add Files)            │
│    User selects files from file dialog      │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ 2. Files added to FileListWidget            │
│    - Stored in QList<FileEntry>             │
│    - Displayed in QTreeWidget               │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ 3. Add/Configure Operations                 │
│    - User clicks + Add Operation            │
│    - OperationCard created                  │
│    - User configures operation parameters   │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ 4. Preview Updates (Real-time)              │
│    operationChanged signal                  │
│         ↓                                    │
│    MainWindow::updatePreviews()             │
│         ↓                                    │
│    FileListWidget::updatePreviews()         │
│         ↓                                    │
│    applyOperations() for each file          │
│         ↓                                    │
│    Update "New Name" column                 │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ 5. Apply Rename (File → Apply Rename)       │
│    - Validate no conflicts                  │
│    - QFile::rename() for each file          │
│    - Collect errors                         │
│    - Show result dialog                     │
└─────────────────────────────────────────────┘
```

## Operation Processing

Each operation is applied sequentially to the filename:

```
Original: "my document.txt"
    ↓
Operation 1: Replace " " → "_"
Result: "my_document.txt"
    ↓
Operation 2: Prefix "backup_"
Result: "backup_my_document.txt"
    ↓
Operation 3: Change Extension ".bak"
Result: "backup_my_document.bak"
    ↓
Final Preview: "backup_my_document.bak"
```

## Key Classes

### MainWindow
- **Purpose**: Main application window and controller
- **Responsibilities**: 
  - Create and layout UI components
  - Handle menu actions
  - Coordinate between widgets
  - Trigger preview updates

### OperationCard
- **Purpose**: UI for a single rename operation
- **Responsibilities**:
  - Display operation type selector
  - Show appropriate input fields
  - Emit signals on changes
  - Provide move/remove controls

### OperationListWidget
- **Purpose**: Container for all operations
- **Responsibilities**:
  - Manage list of OperationCards
  - Handle add/remove/reorder
  - Provide operation data to MainWindow

### FileListWidget  
- **Purpose**: Display and manage file list
- **Responsibilities**:
  - Show files in tree widget
  - Apply operations to generate previews
  - Execute actual file renaming
  - Handle errors and conflicts

## Signals & Slots

```
OperationCard::operationChanged
    → OperationListWidget::onOperationChanged
        → OperationListWidget::operationsChanged
            → MainWindow::updatePreviews
                → FileListWidget::updatePreviews

OperationCard::removeRequested
    → OperationListWidget::onRemoveOperation

OperationCard::moveUpRequested
    → OperationListWidget::onMoveOperationUp

FileListWidget::filesChanged
    → MainWindow::updatePreviews
```
