# regex-rename

Tool to rename files by replacing parts of the original filename based on regex patterns.

## Description

A Qt 6 based batch file renaming application with a graphical user interface. The application allows you to:
- Select multiple files for batch renaming
- Define operations (replace, prefix, suffix, extension changes) to transform filenames
- Preview the changes before applying them
- Apply the renaming operations safely

## Features

- **Visual Operations**: Add, remove, and reorder operations using a card-based interface
- **Real-time Preview**: See how your files will be renamed before applying changes
- **Auto-Numbering**: Use tags like `<0>`, `<00:5>`, or `<000:14>` for sequential file numbering
- **Multiple Operations**: Support for:
  - Replace (using regex patterns)
  - Add Prefix
  - Add Suffix
  - Insert (text at specified position)
  - Change Extension
  - Change Case (lowercase, uppercase, title case)
- **Split View**: Operations panel on the left, file list on the right
- **Safe Renaming**: Checks for conflicts and provides error feedback

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

1. **Add Files**: Use File → Add Files to select files you want to rename
2. **Add Operations**: Click "+ Add Operation" to add renaming operations
3. **Configure Operations**: Select operation type and configure parameters
4. **Preview**: See the new filenames in the "New Name" column
5. **Apply**: Use File → Apply Rename (or Ctrl+R) to execute the renaming

## License

See LICENSE file for details.
