# Solution Diagram: Disable Sorting for "New Name" Column

```
User Action Flow:
==================

┌─────────────────────────────────────────────────────────────────┐
│  File List with 3 Columns                                        │
│  ┌──────────────┬──────────────┬──────────────┐                 │
│  │ Original Name│  New Name    │  File Path   │ <- Headers       │
│  │  (sortable)  │(non-sortable)│  (sortable)  │                 │
│  ├──────────────┼──────────────┼──────────────┤                 │
│  │ file1.txt    │ renamed1.txt │ /home/user   │                 │
│  │ file2.txt    │ renamed2.txt │ /home/user   │                 │
│  │ file3.txt    │ renamed3.txt │ /home/user   │                 │
│  └──────────────┴──────────────┴──────────────┘                 │
└─────────────────────────────────────────────────────────────────┘

Scenario 1: Click "Original Name" (Column 0)
────────────────────────────────────────────────
User clicks header → sortIndicatorChanged(0, Ascending)
                  → logicalIndex != 1
                  → Update tracking: lastSortColumn = 0, lastSortOrder = Ascending
                  → Table sorts by column 0 ✓

Scenario 2: Click "File Path" (Column 2)
────────────────────────────────────────────────
User clicks header → sortIndicatorChanged(2, Ascending)
                  → logicalIndex != 1
                  → Update tracking: lastSortColumn = 2, lastSortOrder = Ascending
                  → Table sorts by column 2 ✓

Scenario 3: Click "New Name" (Column 1) - BLOCKED
────────────────────────────────────────────────
User clicks header → sortIndicatorChanged(1, Ascending)
                  → logicalIndex == 1 (detected!)
                  → Capture current state: column = 2, order = Ascending
                  → QueuedConnection invokes: sortByColumn(2, Ascending)
                  → Table reverts to previous sort (column 2) ✓
                  → "New Name" column appears non-sortable ✓

Implementation Details:
─────────────────────────────────────────────────
┌────────────────────────────────────────────────────────────────┐
│ FileListWidget Class                                           │
│                                                                │
│ Private Members:                                               │
│   - int lastSortColumn = 0         // Track last valid column │
│   - Qt::SortOrder lastSortOrder = Ascending  // Track order   │
│                                                                │
│ Signal Handler:                                                │
│   sortIndicatorChanged(index, order) →                        │
│     if (index == 1) {                                         │
│       // Capture values to avoid race conditions              │
│       int col = lastSortColumn;                               │
│       Qt::SortOrder ord = lastSortOrder;                      │
│       // Restore previous sort asynchronously                 │
│       QMetaObject::invokeMethod(..., Qt::QueuedConnection);   │
│     } else {                                                   │
│       // Update tracking for non-column-1 sorts               │
│       lastSortColumn = index;                                 │
│       lastSortOrder = order;                                  │
│     }                                                          │
└────────────────────────────────────────────────────────────────┘

Why This Works:
─────────────────────────────────────────────────
✓ Minimal changes - no subclassing or major refactoring
✓ Race condition safe - values captured before async invocation
✓ User experience - column 1 simply doesn't respond to clicks
✓ Other columns work normally - no side effects
✓ Maintains last sort state - table doesn't "jump" unexpectedly
```
