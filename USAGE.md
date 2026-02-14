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
- **Replace**: Use regex patterns to find and replace text in filenames (excluding the extension). Supports auto-numbering tags.
- **Add Prefix**: Add text to the beginning of the filename. Supports auto-numbering tags.
- **Add Suffix**: Add text before the file extension. Supports auto-numbering tags.
- **Insert**: Insert text at a specified position (0-based index, excluding the extension). Supports auto-numbering tags.
- **Change Extension**: Change the file extension to a new one
- **Change Case**: Convert the filename to lowercase, uppercase, or title case

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
     - For Change Case: Select the case type (lowercase, uppercase, or title case)

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

1. Add files with spaces in their names (e.g., "my document.txt")
2. Add a Replace operation
3. Pattern: ` ` (space)
4. Replace with: `_`
5. Preview shows spaces replaced with underscores (e.g., "my_document.txt")
6. Note: The extension (.txt) is preserved and not affected by the replacement

### Example 2: Add Date Prefix
**Goal**: Add today's date as a prefix

1. Add files (e.g., "report.pdf")
2. Add a Prefix operation
3. Prefix: `2024-02-14_`
4. Result: "report.pdf" → "2024-02-14_report.pdf"

### Example 3: Insert Text at Position
**Goal**: Insert a separator at a specific position in the filename

1. Add files like "backup20240214.txt"
2. Add an Insert operation
3. Position: `6` (0-based index, after "backup")
4. Text: `_`
5. Result: "backup20240214.txt" → "backup_20240214.txt"
6. Note: The position index is relative to the basename only (excluding .txt extension)

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

### Example 6: Auto-Numbering with Tags
**Goal**: Add sequential numbers to files

1. Add multiple files (e.g., "photo.jpg", "photo (2).jpg", "photo (3).jpg")
2. Add a Prefix operation
3. Prefix: `IMG_<000:1>_`
4. Result:
   - "photo.jpg" → "IMG_001_photo.jpg"
   - "photo (2).jpg" → "IMG_002_photo (2).jpg"
   - "photo (3).jpg" → "IMG_003_photo (3).jpg"

### Example 7: Change Case
**Goal**: Standardize filename casing

**Lowercase:**
1. Add files with mixed case names (e.g., "MyDocument.txt", "REPORT.pdf")
2. Add a Change Case operation
3. Case Type: Lowercase
4. Result:
   - "MyDocument.txt" → "mydocument.txt"
   - "REPORT.pdf" → "report.pdf"

**Uppercase:**
1. Add files (e.g., "notes.txt", "readme.md")
2. Add a Change Case operation
3. Case Type: Uppercase
4. Result:
   - "notes.txt" → "NOTES.txt"
   - "readme.md" → "README.md"

**Title Case:**
1. Add files (e.g., "my_document_file.txt", "test report.doc")
2. Add a Change Case operation
3. Case Type: Title Case
4. Result:
   - "my_document_file.txt" → "My_Document_File.txt"
   - "test report.doc" → "Test Report.doc"
5. Note: Title case capitalizes the first letter of each word, where words are separated by spaces, underscores, or other non-letter characters

## Auto-Numbering Tags

Tags allow you to include auto-incrementing numbers in prefix, suffix, replace text, and insert operations. Tag format: `<zeros:start>`

**Format Components:**
- **zeros**: One or more zeros defining the number format (leading zeros)
- **:start** (optional): Starting number (defaults to 1 if omitted)

**Common Examples:**
- `<0>` - Single digit starting at 1: 1, 2, 3...
- `<00>` - Two digits with leading zeros: 01, 02, 03...
- `<000>` - Three digits: 001, 002, 003...
- `<0:5>` - Single digit starting at 5: 5, 6, 7...
- `<00:14>` - Two digits starting at 14: 14, 15, 16...

**Multiple Tags:**
You can use multiple tags in the same operation. Example: `<0>-<00:5>_` produces `1-05_`, `2-06_`, etc.

**Notes:**
- Tags work in prefix, suffix, replace text, and insert text fields
- File indexing is 0-based, but the start number determines the first visible number
- Numbers exceeding the format width will use additional digits (e.g., `<00>` at file 100 shows `100`)

## Tips

- **Preview First**: Always check the preview before applying
- **Operation Order Matters**: Operations are applied sequentially
- **Regex Power**: Use regex patterns for complex replacements
- **Auto-Numbering**: Use tags like `<0>`, `<00:5>`, or `<000:14>` for sequential numbering (see Auto-Numbering Tags section above)
- **Extension Preservation**: All operations except "Change Extension" preserve the file extension - they only modify the basename (including "Change Case" which only affects the filename, not the extension)
- **Undo**: There is no undo - make sure to preview carefully
- **Conflicts**: If a target filename already exists, that file will be skipped with an error
- **File Selection**: You can add more files at any time

## Safety Features

- Real-time preview of all changes
- Conflict detection (prevents overwriting existing files)
- Error reporting for failed renames
- No accidental modifications without confirmation
