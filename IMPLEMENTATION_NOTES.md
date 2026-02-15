# Implementation Notes: Disable Sorting for "New Name" Column

## Problem Statement
The file list in the application has sorting enabled for all columns. The requirement is to disable sorting **only** for the "New Name" column while keeping it enabled for other columns ("Original Name" and "File Path").

## Solution

### Approach
Instead of completely disabling the sort indicator for column 1, we intercept sort attempts and immediately restore the previous sort state. This provides a better user experience as:
1. The column header remains consistent with other columns visually
2. Clicking the "New Name" header simply doesn't change the sort
3. Other columns continue to work normally

### Implementation Details

#### Files Modified
- `src/filelistwidget.h`: Added two member variables to track the last valid sort state
- `src/filelistwidget.cpp`: Added initialization and signal handling logic

#### Changes in `filelistwidget.h`
Added two private member variables:
```cpp
// Track last valid sort column (for preventing sort on column 1)
int lastSortColumn;
Qt::SortOrder lastSortOrder;
```

#### Changes in `filelistwidget.cpp`

1. **Constructor Initialization**:
   - Initialize `lastSortColumn` to 0 (Original Name column)
   - Initialize `lastSortOrder` to `Qt::AscendingOrder`

2. **Signal Handler in `setupUI()`**:
   - Connect to `QHeaderView::sortIndicatorChanged` signal
   - When column 1 is attempted to be sorted:
     - Use `QMetaObject::invokeMethod` with `Qt::QueuedConnection` to restore previous sort
     - This avoids recursive signal issues
   - When any other column is sorted:
     - Update the tracking variables with the new sort state

### Behavior

- **Initial State**: Table is sorted by column 0 (Original Name) in ascending order
- **User sorts by "Original Name" or "File Path"**: Normal sorting behavior, tracking variables updated
- **User clicks "New Name" header**: The sort is immediately reverted to the last valid sort state
- **Result**: The "New Name" column appears non-sortable while other columns remain sortable

### Rationale for This Approach

Alternative approaches considered:
1. **Making column 1 header non-clickable**: Would require more invasive changes to QHeaderView
2. **Custom QTreeWidget subclass**: Overkill for this single feature
3. **Removing sort indicator for column 1**: Would make the UI inconsistent

The chosen approach is minimal, clean, and doesn't require subclassing or major architectural changes.

### Testing

Due to the Qt6 dependency not being available in the build environment, manual testing would be required:
1. Build the application with Qt6 installed
2. Add some files to the list
3. Click on "Original Name" column header - verify sorting works
4. Click on "File Path" column header - verify sorting works  
5. Click on "New Name" column header - verify sorting does NOT change
6. Verify that after clicking "New Name", the previous sort is maintained

### Edge Cases Handled

- First click on "New Name" before any other sorting: Defaults to column 0 ascending
- Multiple rapid clicks on "New Name": QueuedConnection ensures proper handling
- Switching between different column sorts then clicking "New Name": Last valid sort is restored
