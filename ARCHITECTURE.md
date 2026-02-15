# Application Architecture

## Overview

Regex Rename is a Qt 6-based batch file renaming application with a modern, card-based UI. It uses a split-pane layout with operations on the left and file previews on the right, providing real-time feedback with debounced updates and async preview generation.

## Technology Stack

- **Framework**: Qt 6 (Core, Gui, Widgets, Concurrent)
- **Language**: C++17
- **Build System**: CMake 3.16+
- **Styling**: External QSS (Qt Style Sheet) with system palette integration  
- **Async**: QtConcurrent for preview generation
- **Resource System**: Qt Resource Collection (.qrc) for embedding stylesheets

## UI Layout

```
┌─────────────────────────────────────────────────────────────────────────┐
│ Regex Rename - Batch File Renamer                            [_][□][X]  │
├─────────────────────────────────────────────────────────────────────────┤
│ File   Help                                                             │
├─────────────────┬───────────────────────────────────────────────────────┤
│                 │                                                       │
│  Operations     │  Files to Rename                                      │
│                 │                                                       │
│  ┌───────────┐  │  ┌──────────────────────────────────────────────────┐ │
│  │ Operation │  │  │ Original Name │ New Name           │ File Path   │ │
│  │ Replace   │  │  ├──────────────────────────────────────────────────┤ │
│  │ Pattern:  │  │  │ document.txt  │ backup_document... │ /home/...   │ |
│  │ [_______] │  │  │ image.jpg     │ backup_image...    │ /home/...   │ │
│  │ Replace:  │  │  │ notes.md      │ backup_notes...    │ /home/...   │ │
│  │ [_______] │  │  └──────────────────────────────────────────────────┘ │
│  │ [↑][↓][X] │  │                                                       │
│  └───────────┘  │                                                       │
│                 │                                                       │
│  ┌───────────┐  │                                                       │
│  │ Operation │  │                                                       │
│  │ Prefix    │  │                                                       │
│  │ Prefix:   │  │                                                       │
│  │ [backup_] │  │                                                       │
│  │ [↑][↓][X] │  │                                                       │
│  └───────────┘  │                                                       │
│                 │                                                       │
│  [+ Add Op...]  │                                                       │
│                 │                                                       │
└─────────────────┴───────────────────────────────────────────────────────┘
```

## Component Architecture

```
MainWindow
├── Global Stylesheet (loaded from resource/style.qss)
├── QMenuBar
│   ├── File Menu
│   │   ├── Add Files... (Ctrl+O)
│   │   ├── Clear Files
│   │   ├── Apply Rename (Ctrl+R)
│   │   └── Exit (Ctrl+Q)
│   └── Help Menu
│       └── About
│
└── QSplitter (Horizontal)
    ├── OperationListWidget (Left 30%)
    │   ├── QLabel (Title: "Operations")
    │   ├── QScrollArea
    │   │   └── QWidget (scroll content)
    │   │       └── QVBoxLayout
    │   │           ├── OperationCard 1 (QFrame#operationCard)
    │   │           │   ├── QComboBox (operation type)
    │   │           │   ├── QComboBox (case type - visible for change_case)
    │   │           │   ├── QLineEdit (pattern/value)
    │   │           │   ├── QLineEdit (replacement - conditional)
    │   │           │   ├── QPushButton (↑, ↓)
    │   │           │   ├── QPushButton (Remove)
    │   │           │   └── QTimer (debounce - 300ms delay)
    │   │           ├── OperationCard 2
    │   │           └── ...
    │   └── QPushButton (+ Add Operation)
    │
    └── FileListWidget (Right 70%)
        ├── QLabel (Title: "Files to Rename")
        ├── QTreeWidget (3 columns)
        │   ├── Column 0: File Path (Interactive resize)
        │   ├── Column 1: Original Name (Interactive resize)
        │   └── Column 2: New Name (Stretch, bold green for changes)
        └── QFutureWatcher<QString> (async preview generation)
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
│    - Fast duplicate check using QSet        │
│    - Displayed in QTreeWidget               │
│    - Emit filesChanged signal               │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ 3. Add/Configure Operations                 │
│    - User clicks + Add Operation            │
│    - OperationCard created with debounce    │
│    - User configures operation parameters   │
│    - Text changes start 300ms timer         │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ 4. Preview Updates (Debounced)              │
│    User types → QTimer restarts (300ms)     │
│         ↓                                   │
│    Timer timeout → operationChanged signal  │
│         ↓                                   │
│    MainWindow::updatePreviews()             │
│         ↓                                   │
│    FileListWidget::updatePreviews()         │
│         ↓                                   │
│    QtConcurrent::run (async processing)     │
│         ↓                                   │
│    applyOperations() for each file          │
│         ↓                                   │
│    QFutureWatcher::finished signal          │
│         ↓                                   │
│    Update "New Name" column (bold green)    │
└─────────────────────────────────────────────┘
    ↓
┌─────────────────────────────────────────────┐
│ 5. Apply Rename (File → Apply Rename)       │
│    - Validate no conflicts                  │
│    - QFile::rename() for each file          │
│    - Collect errors (file exists, etc.)     │
│    - Show result dialog with statistics     │
│    - Update original names for success      │
└─────────────────────────────────────────────┘
```

## Operation Processing

Each operation is applied sequentially to the filename (basename only, except Change Extension and Change Case):

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
Operation 4: Change Case (uppercase)
Result: "BACKUP_MY_DOCUMENT.bak"
    ↓
Final Preview: "BACKUP_MY_DOCUMENT.bak"
```

### Auto-Numbering Tags

Operations support auto-numbering tags with format `<zeros:start>`:

```
Files: file1.txt, file2.txt, file3.txt
Operation: Prefix "IMG_<000:1>_"

Processing:
- file1.txt (index 0) → IMG_001_file1.txt
- file2.txt (index 1) → IMG_002_file2.txt  
- file3.txt (index 2) → IMG_003_file3.txt

Tag Examples:
- <0>      → 1, 2, 3...
- <00>     → 01, 02, 03...
- <000:14> → 014, 015, 016...
```

### Operation Types

1. **Replace**: Regex pattern matching on basename (excludes extension)
2. **Prefix**: Add text to start of basename, supports tags
3. **Suffix**: Add text before extension, supports tags
4. **Insert**: Insert text at position (0-based index), supports tags
5. **Change Extension**: Replace file extension completely
6. **Change Case**: Transform basename to lowercase/UPPERCASE/Title Case
7. **New Name**: Replace entire basename with new name, supports tags, preserves extension

## Key Classes

### MainWindow
- **Purpose**: Main application window and controller
- **Responsibilities**: 
  - Create and layout UI components
  - Handle menu actions (Add Files, Clear, Apply, About)
  - Coordinate between OperationListWidget and FileListWidget
  - Trigger preview updates via updatePreviews()
  - Load global stylesheet from resource file
- **Key Methods**:
  - `setupMenuBar()`: Configure File and Help menus with shortcuts
  - `setupUI()`: Create splitter with operation and file list widgets
  - `updatePreviews()`: Retrieve operations and trigger file list updates
  - `onApplyRename()`: Execute rename operations and show results

### OperationCard (QFrame)
- **Purpose**: UI for a single rename operation with modern styling
- **Responsibilities**:
  - Display operation type selector with 6 operation types
  - Show appropriate input fields based on operation type
  - Debounce text input changes (300ms delay)
  - Emit signals on changes, moves, and removal
  - Style with #operationCard object name for CSS targeting
- **Key Features**:
  - **Debounce Timer**: Delays operationChanged signal until 300ms after last keystroke
  - **Dynamic UI**: Fields show/hide based on operation type
  - **Case Type Combo**: Additional dropdown for change_case operation
  - **Hover Effects**: Visual feedback via QSS styling
- **Signals**:
  - `operationChanged()`: Emitted after debounce timeout
  - `removeRequested()`, `moveUpRequested()`, `moveDownRequested()`

### OperationListWidget
- **Purpose**: Container for all operation cards
- **Responsibilities**:
  - Manage list of OperationCards in scrollable area
  - Handle add/remove/reorder operations
  - Provide operation data to MainWindow via getOperations()
  - Propagate operationChanged signals
- **Data Structure**: `QList<OperationCard*> operationCards`
- **Key Methods**:
  - `onAddOperation()`: Create new OperationCard and insert before stretch
  - `onMoveOperationUp/Down()`: Swap cards and update layout
  - `getOperations()`: Convert cards to Operation objects

### FileListWidget
- **Purpose**: Display and manage file list with async previews
- **Responsibilities**:
  - Show files in tree widget with 3 columns
  - Apply operations asynchronously to generate previews
  - Execute actual file renaming operations
  - Handle errors, conflicts, and duplicate detection
  - Maintain file directory structure for display
- **Key Features**:
  - **Async Preview**: Uses QtConcurrent and QFutureWatcher
  - **Fast Duplicate Check**: QSet<QString> for O(1) duplicate detection
  - **Visual Feedback**: Bold green text for changed names
  - **Column Management**: Interactive resize with last column stretch
- **Data Structures**:
  - `QList<FileEntry> files`: Ordered list of files
  - `QSet<QString> filePathsSet`: Fast duplicate lookup
  - `QFutureWatcher<QString> *previewWatcher`: Async result handler
- **Key Methods**:
  - `addFiles()`: Batch add with duplicate check and batch updates disabled
  - `updatePreviews()`: Launch async QtConcurrent::run for preview generation
  - `applyOperations()`: Static method to apply operation sequence
  - `applyRename()`: Execute QFile::rename() and collect errors

### Operation Classes (Abstract Hierarchy)

**Base Class: Operation**
- Pure virtual `perform(fileName, fileIndex)` method
- Static `replaceTags()` for auto-numbering support
- Virtual `getType()` for operation identification

**Concrete Operations:**
1. **ReplaceOperation**: Regex find/replace on basename
2. **PrefixOperation**: Prepend text with tag support
3. **SuffixOperation**: Append before extension with tag support
4. **InsertOperation**: Insert at position with tag support
5. **ChangeExtensionOperation**: Replace file extension
6. **ChangeCaseOperation**: Transform case (Lowercase/Uppercase/TitleCase)
7. **NewNameOperation**: Replace entire basename with new name, preserves extension, tag support

## Signals & Slots

### Input Debouncing Flow
```
User types in OperationCard::valueEdit
    → QLineEdit::textChanged
        → OperationCard::onTextChanged()
            → QTimer::start() [restarts 300ms timer]
                [... user continues typing, timer keeps restarting ...]
            → QTimer::timeout (after 300ms of no typing)
                → OperationCard::operationChanged

OperationCard::operationChanged
    → OperationListWidget::onOperationChanged
        → OperationListWidget::operationsChanged
            → MainWindow::updatePreviews
                → FileListWidget::updatePreviews
                    → QtConcurrent::run(applyOperations)
                        → QFutureWatcher::finished
                            → FileListWidget::onPreviewsReady
                                → Update tree widget display
```

### Other Signal Chains
```
OperationCard::removeRequested
    → OperationListWidget::onRemoveOperation

OperationCard::moveUpRequested  
    → OperationListWidget::onMoveOperationUp

OperationCard::moveDownRequested
    → OperationListWidget::onMoveOperationDown

FileListWidget::filesChanged
    → MainWindow::updatePreviews

QComboBox::currentIndexChanged (operation type)
    → OperationCard::onOperationTypeChanged
        → OperationCard::operationChanged (immediate, no debounce)
```

## Styling System

### Architecture
- **Global Application**: Stylesheet loaded in `main.cpp` via `QApplication::setStyleSheet()`
- **Resource File**: `resource/style.qss` embedded via `resources.qrc`
- **Theme Integration**: Uses `palette()` function for KDE/system colors
- **Selectors**: Object names (#operationCard), property selectors ([text="Remove"])

### Style Features
- Modern card-based design with rounded corners (8px radius)
- Hover effects on operation cards and buttons
- System palette integration for highlights and selections
- Consistent color scheme using Bootstrap-inspired palette
- Custom scrollbar styling (12px width, rounded handles)
- Button variants by text content (Add=green, Remove=red, arrows=gray)

### Key Color Roles
```css
palette(highlight)        /* KDE selection/highlight color */
palette(window)           /* Window background */
palette(base)             /* Input field background */
palette(text)             /* Primary text color */
palette(button)           /* Button background */
palette(highlighted-text) /* Selected text color */
```

## Performance Optimizations

1. **Debounced Updates**: 300ms delay prevents excessive preview recalculations during typing
2. **Async Preview Generation**: QtConcurrent offloads preview calculation from UI thread
3. **Fast Duplicate Detection**: QSet provides O(1) lookup for duplicate files
4. **Batch UI Updates**: `setUpdatesEnabled(false)` during bulk file additions
5. **Resource Embedding**: QRC compiles stylesheet into binary (no runtime file I/O)

## File Structure

```
regex-rename/
├── CMakeLists.txt           # Build configuration
├── README.md                 # User documentation
├── ARCHITECTURE.md           # This file
├── resources.qrc             # Qt resource collection
├── resource/
│   └── style.qss            # Global stylesheet
└── src/
    ├── main.cpp              # Entry point, loads stylesheet
    ├── mainwindow.{h,cpp}    # Main window controller
    ├── operationcard.{h,cpp} # Operation card UI with debounce
    ├── operationlistwidget.{h,cpp}  # Operation list container
    ├── filelistwidget.{h,cpp}       # File list with async preview
    └── operation.{h,cpp}     # Operation class hierarchy
```
