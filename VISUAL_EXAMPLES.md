# Tag Numbering Feature - Visual Examples

## How It Works in the UI

### Example 1: Adding Sequential Numbers to Photo Names

**Setup:**
1. Add multiple photo files to the application
2. Add a "Prefix" operation
3. Enter `<000:1>_` in the prefix field

**Visual Result in File List:**

```
┌─────────────────────────────────────────────────────────────────────────┐
│ File Path           │ Original Name    │ New Name                       │
├─────────────────────────────────────────────────────────────────────────┤
│ /home/user/photos   │ DSC_1234.jpg     │ 001_DSC_1234.jpg              │
│ /home/user/photos   │ DSC_1235.jpg     │ 002_DSC_1235.jpg              │
│ /home/user/photos   │ DSC_1236.jpg     │ 003_DSC_1236.jpg              │
│ /home/user/photos   │ DSC_1237.jpg     │ 004_DSC_1237.jpg              │
│ /home/user/photos   │ DSC_1238.jpg     │ 005_DSC_1238.jpg              │
└─────────────────────────────────────────────────────────────────────────┘
```

The "New Name" column shows in **bold green** to indicate changes will be made.

---

### Example 2: Multiple Files with Different Starting Number

**Setup:**
1. Add document files
2. Add a "Suffix" operation
3. Enter `_rev<00:5>` in the suffix field

**Visual Result:**

```
┌─────────────────────────────────────────────────────────────────────────┐
│ File Path           │ Original Name    │ New Name                       │
├─────────────────────────────────────────────────────────────────────────┤
│ /home/user/docs     │ contract.pdf     │ contract_rev05.pdf            │
│ /home/user/docs     │ agreement.pdf    │ agreement_rev06.pdf           │
│ /home/user/docs     │ terms.pdf        │ terms_rev07.pdf               │
└─────────────────────────────────────────────────────────────────────────┘
```

---

### Example 3: Combining Tags with Text

**Setup:**
1. Add video files
2. Add a "Prefix" operation
3. Enter `Episode_<00:1>_` in the prefix field

**Visual Result:**

```
┌─────────────────────────────────────────────────────────────────────────┐
│ File Path           │ Original Name    │ New Name                       │
├─────────────────────────────────────────────────────────────────────────┤
│ /home/user/videos   │ intro.mp4        │ Episode_01_intro.mp4          │
│ /home/user/videos   │ part1.mp4        │ Episode_02_part1.mp4          │
│ /home/user/videos   │ part2.mp4        │ Episode_03_part2.mp4          │
│ /home/user/videos   │ finale.mp4       │ Episode_04_finale.mp4         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

### Example 4: Replace Operation with Tags

**Setup:**
1. Add temp files
2. Add a "Replace" operation
3. Pattern: `temp`
4. Replacement: `file_<000:100>`

**Visual Result:**

```
┌─────────────────────────────────────────────────────────────────────────┐
│ File Path           │ Original Name    │ New Name                       │
├─────────────────────────────────────────────────────────────────────────┤
│ /home/user/temp     │ temp_data.txt    │ file_100_data.txt             │
│ /home/user/temp     │ temp_log.txt     │ file_101_log.txt              │
│ /home/user/temp     │ temp_config.txt  │ file_102_config.txt           │
└─────────────────────────────────────────────────────────────────────────┘
```

---

### Example 5: Multiple Tags in One Operation

**Setup:**
1. Add image files
2. Add a "Prefix" operation
3. Enter `<0>-<00:10>_` in the prefix field

**Visual Result:**

```
┌─────────────────────────────────────────────────────────────────────────┐
│ File Path           │ Original Name    │ New Name                       │
├─────────────────────────────────────────────────────────────────────────┤
│ /home/user/images   │ sketch.png       │ 1-10_sketch.png               │
│ /home/user/images   │ drawing.png      │ 2-11_drawing.png              │
│ /home/user/images   │ artwork.png      │ 3-12_artwork.png              │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## Operation Card Example

The operation card in the left panel would look like this:

```
┌──────────────────────────────────────────┐
│ [Replace ▼]                      [↑] [↓] │
│                                           │
│ Pattern:                                  │
│ ┌─────────────────────────────────────┐   │
│ │ temp                                │   │
│ └─────────────────────────────────────┘   │
│                                           │
│ Replace with:                             │
│ ┌─────────────────────────────────────┐   │
│ │ file<000:1>                        │   │
│ └─────────────────────────────────────┘   │
│                                           │
│                             [Remove]      │
└──────────────────────────────────────────┘
```

As you type `<000:1>` in the "Replace with" field, the preview in the file list updates in real-time to show the numbered replacements.

---

## Tag Format Quick Reference

| Tag Format | Description | Example Output (files 0-2) |
|------------|-------------|----------------------------|
| `<0>` | Single digit, start at 1 | 1, 2, 3 |
| `<00>` | Two digits, start at 1 | 01, 02, 03 |
| `<000>` | Three digits, start at 1 | 001, 002, 003 |
| `<0:5>` | Single digit, start at 5 | 5, 6, 7 |
| `<00:10>` | Two digits, start at 10 | 10, 11, 12 |
| `<000:100>` | Three digits, start at 100 | 100, 101, 102 |

---

## Notes

- **Real-time Preview**: As you type the tag in any text field, the "New Name" column updates immediately
- **Bold Green**: Changed filenames appear in bold green text for easy identification
- **No Extension Change**: Tags in prefix/suffix don't affect file extensions
- **Multiple Operations**: You can combine tags with other operations (e.g., replace + prefix + suffix)
- **Order Matters**: The order of operations affects the final result - use ↑/↓ to reorder
