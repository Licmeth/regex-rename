# Tag Numbering Feature

## Overview

The tag numbering feature allows you to include auto-incrementing numbers in the prefix, suffix, and replace text fields when renaming files. This is useful for batch renaming files with sequential numbers.

## Tag Format

Tags use the format: `<zeros:start>`

- **zeros**: One or more zeros that define the formatting (number of leading zeros)
- **:start** (optional): Colon followed by the starting number

If `:start` is omitted, numbering starts from 1 by default.

## Examples

### Basic Tags

1. **`<0>`** - Single digit, starts at 1
   - File 0: `1`
   - File 1: `2`
   - File 2: `3`

2. **`<00>`** - Two digits with leading zeros, starts at 1
   - File 0: `01`
   - File 1: `02`
   - File 9: `10`

3. **`<000>`** - Three digits with leading zeros, starts at 1
   - File 0: `001`
   - File 1: `002`
   - File 99: `100`

### Tags with Custom Start Number

1. **`<0:5>`** - Single digit, starts at 5
   - File 0: `5`
   - File 1: `6`
   - File 2: `7`

2. **`<00:14>`** - Two digits with leading zeros, starts at 14
   - File 0: `14`
   - File 1: `15`
   - File 2: `16`

3. **`<000:1>`** - Three digits with leading zeros, starts at 1
   - File 0: `001`
   - File 1: `002`
   - File 11: `012`

## Usage in Operations

### Prefix Operation

Add a numbered prefix to files:

**Input files:**
- photo.jpg
- photo.jpg (duplicate)
- photo.jpg (another)

**Prefix:** `IMG_<000:1>_`

**Output:**
- IMG_001_photo.jpg
- IMG_002_photo.jpg
- IMG_003_photo.jpg

### Suffix Operation

Add a numbered suffix before the extension:

**Input files:**
- document.pdf
- document.pdf (copy)
- document.pdf (final)

**Suffix:** `_v<00>`

**Output:**
- document_v01.pdf
- document_v02.pdf
- document_v03.pdf

### Replace Operation

Replace text with a numbered pattern:

**Input files:**
- temp.txt
- temp.txt (2)
- temp.txt (3)

**Pattern:** `temp`
**Replacement:** `file<00:10>`

**Output:**
- file10.txt
- file11.txt
- file12.txt

## Multiple Tags

You can use multiple tags in the same operation:

**Prefix:** `<0>-<00:5>_`

**For file 0:** `1-05_filename.txt`
**For file 1:** `2-06_filename.txt`

## Important Notes

1. File indexing is **0-based** internally, but the start number determines the first visible number
2. Tags work in all text fields: prefix, suffix, and replace text
3. The format is determined by the number of zeros before the colon
4. If the number exceeds the format width, it will use more digits (e.g., `<00>` with file 100 will show `100`, not `00`)
