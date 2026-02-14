# Usage Guide

## Overview

Regex Rename is a Qt 6-based batch file renaming application with a graphical user interface. It provides a visual way to rename multiple files using various operations with real-time preview.

## User Interface

The application window is divided into three main sections:

### Menu Bar (Top)
- **File Menu**:
  - Add Files... (Ctrl+O): Select files to add to the rename list
  - Clear Files: Remove all files from the list
  - Apply Rename (Ctrl+R): Execute the renaming operations
  - Exit (Ctrl+Q): Close the application

- **Help Menu**:
  - About: Display application information

### Operations Panel (Left Side)
The operations panel displays a list of operation cards. Each card represents a transformation to apply to filenames.

**Operation Types:**
- **Replace**: Use regex patterns to find and replace text in filenames
- **Add Prefix**: Add text to the beginning of the filename
- **Add Suffix**: Add text before the file extension
- **Insert**: Insert text at a specified position (0-based index)
- **Change Extension**: Change the file extension to a new one

**Operation Card Controls:**
- ↑ button: Move operation up in the list (operations are applied in order)
- ↓ button: Move operation down in the list
- Remove button: Delete the operation
- **+ Add Operation** button at the bottom: Create a new operation

### File List Panel (Right Side)
The file list displays all selected files in a table with three columns:
- **File Path**: The directory containing the file
- **Original Name**: The original filename
- **New Name**: The preview of what the filename will become after applying operations

Files with changes are highlighted in bold green text in the "New Name" column.

## Workflow

1. **Add Files**
   - Click "File → Add Files..." or press Ctrl+O
   - Select one or multiple files from the file dialog
   - Files appear in the file list on the right

2. **Create Operations**
   - Click "+ Add Operation" in the left panel
   - Select the operation type from the dropdown
   - Configure the operation parameters:
     - For Replace: Enter regex pattern and replacement text
     - For Prefix: Enter the prefix text
     - For Suffix: Enter the suffix text
     - For Insert: Enter position (0-based index) and text to insert
     - For Change Extension: Enter the new extension (e.g., .txt)

3. **Arrange Operations**
   - Use ↑ and ↓ buttons to reorder operations
   - Operations are applied from top to bottom
   - The preview updates automatically as you change operations

4. **Preview Changes**
   - The "New Name" column shows the result of applying all operations
   - Changes are highlighted in bold green
   - Verify that the preview looks correct

5. **Apply Renaming**
   - Click "File → Apply Rename" or press Ctrl+R
   - The application will rename all files
   - A dialog shows the results (success count and any errors)
   - Successfully renamed files update their "Original Name" to match "New Name"

## Examples

### Example 1: Replace with Regex
**Goal**: Replace all spaces with underscores

1. Add files with spaces in their names
2. Add a Replace operation
3. Pattern: ` ` (space)
4. Replace with: `_`
5. Preview shows spaces replaced with underscores

### Example 2: Add Date Prefix
**Goal**: Add today's date as a prefix

1. Add files
2. Add a Prefix operation
3. Prefix: `2024-02-14_`
4. All files now start with the date

### Example 3: Insert Text at Position
**Goal**: Insert text at a specific position in the filename

1. Add files like "document.txt"
2. Add an Insert operation
3. Position: `3` (0-based index)
4. Text: `_new`
5. Result: "document.txt" → "doc_newument.txt"

### Example 4: Change Extension
**Goal**: Change all .txt files to .md

1. Add .txt files
2. Add a Change Extension operation
3. New Extension: `.md` or `md`
4. Files now have .md extension

### Example 5: Complex Renaming
**Goal**: Replace spaces, add prefix, and change extension

1. Add files like "my document.txt"
2. Add Replace operation: ` ` → `_`
3. Add Prefix operation: `backup_`
4. Add Change Extension operation: `.bak`
5. Result: "my document.txt" → "backup_my_document.bak"

## Tips

- **Preview First**: Always check the preview before applying
- **Operation Order Matters**: Operations are applied sequentially
- **Regex Power**: Use regex patterns for complex replacements
- **Undo**: There is no undo - make sure to preview carefully
- **Conflicts**: If a target filename already exists, that file will be skipped with an error
- **File Selection**: You can add more files at any time

## Safety Features

- Real-time preview of all changes
- Conflict detection (prevents overwriting existing files)
- Error reporting for failed renames
- No accidental modifications without confirmation
