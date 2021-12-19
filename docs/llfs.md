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

# Version 1A
This version comes with the following changes:

### Header:
 - 4 bytes signature (always "LLFS")
 - 1 byte version (always 2)
 - 8 bytes number of entries
 - 4 bytes padding

### Entry:
  - 368 bytes filename
  - 8 bytes file size in bytes
  - 1 byte padding
  - File content