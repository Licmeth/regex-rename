# regex-rename

Tool to rename files by replacing parts of the original filename based on regex patterns.

## Description

A Qt 6 based batch file renaming application with a graphical user interface. The application allows you to:
- Select multiple files for batch renaming
- Define operations (replace, prefix, suffix, extension changes) to transform filenames
- Preview the changes before applying them
- Apply the renaming operations safely

## Requirements

- CMake 3.16 or higher
- Qt 6 (Core, Gui, Widgets modules)
- C++17 compatible compiler

## Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Running

After building, run the application:

```bash
./regex-rename
```

## Usage

### Quick Start

1. **Add Files**: Click File → Add Files (Ctrl+O) to select files for renaming
2. **Add Operations**: Click "+ Add Operation" to create transformation operations
3. **Configure**: Choose operation type and set parameters:
   - **Replace**: Use regex patterns to find and replace text
   - **Prefix/Suffix**: Add text to the beginning or end of filenames
   - **Insert**: Add text at specific position
   - **Change Extension**: Modify file extensions
   - **Change Case**: Convert to lowercase, uppercase, or title case
4. **Reorder**: Use ↑↓ buttons to arrange operation order (applied top to bottom)
5. **Preview**: View results in the "New Name" column (changes shown in bold green)
6. **Apply**: Execute renaming with File → Apply Rename (Ctrl+R)

### Auto-Numbering

Use tags in operations for sequential numbering:
- `<0>` - Single digit: 1, 2, 3...
- `<00>` - Two digits: 01, 02, 03...
- `<000:14>` - Three digits starting at 14: 014, 015, 016...

### Examples

**Replace spaces with underscores:**
- Add Replace operation: pattern ` `, replace with `_`
- "my document.txt" → "my_document.txt"

**Add numbered prefix:**
- Add Prefix operation: `photo_<00:1>_`
- "image.jpg" → "photo_01_image.jpg"

**Combine operations:**
1. Replace: ` ` → `_`
2. Prefix: `backup_`
3. Change Extension: `.bak`
- "my document.txt" → "backup_my_document.bak"

### Tips

- Operations preserve file extensions (except Change Extension)
- Always preview before applying (no undo available)
- Files are processed in order; conflicts are skipped

## License

See LICENSE file for details.
