The Low Level File System is a file system designed to be small and to be used as a read only ram file system.

# Version 1

### Header:
 - 4 bytes signature (always "LLFS")
 - 1 byte version (always 1)
 - 8 bytes number of entries
 - 4 bytes padding

### Entry:
  - 16 bytes filename
  - 8 bytes file size in bytes
  - 1 byte padding
  - File content

### Example LLFS structure:
  - Header:
    - Signature: LLFS
    - Version: 1
    - Number of entries: 1
    - Padding: 0000
  - Entry:
    - File name: Test.txt
    - File size: 13 bytes
    - Padding: 0
    - Content: A is a letter

# Version 2

### Header:
 - 4 bytes signature (always "LLFS")
 - 1 byte version (always 2)
 - 8 bytes number of entries

### Entry header:
 - 1 byte type (1 for file, 2 for directory, 3 metadata)

### Entry type 1: (normal file)
 - 1 byte file name size
 - file name with the size declared above
 - 1 byte parent directory name size
 - parent directory name with the size declared above
 - 1 byte attributes
 - 8 bytes file size
 - File contents

### Entry type 2: (metadata)
 - 1 byte creation date year
 - 1 byte creation date month
 - 1 byte creation date day

### Example LLFS structure:
  - Header:
    - Signature: LLFS
    - Version: 2
    - Number of entries: 3
    - Padding: 0000
  - Entry:
    - Type: 2
    - Year: 2020
    - Month: 2
    - Day: 10
  - Entry:
    - Type: 1
    - File name size: 8
    - File name: Test.txt
    - Parent directory name size: 3
    - Parent directory name: Dir
    - Attributes: 0
    - File size: 13 bytes
    - Content: A is a letter

### Entry type 3 attributes:
Each bit in the attributes byte describes an attribute.

Bit 0 - System (used to describe if the file is system protected)
Bit 1 - Hidden (used to hide the file)
Bit 2 - In use (set by the operating system when the file is in use)

Bits 3-7 are not used.