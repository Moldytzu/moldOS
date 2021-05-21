The Low Level File System is a file system designed to be small and to be used as a read only ram file system.

### Version 1

Header:
    4 byte signature (always "LLFS")
    1 byte version (llfs version)
    64-bit unsigned int number of entries
    4 byte padding

Entry:
    16 byte filename
    64-bit unsigned int file size in bytes
    1 byte padding
    File content

Examples:
    Header:
        Signature: LLFS
        Version: 1
        Number of entries: 5
        Padding: 0000
    Entry:
        File name: Test.txt
        File size: 13 bytes
        Padding: 0
        Content: A is a letter