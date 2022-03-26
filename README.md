# parse-elf
`parse-elf` is a simple parser for elf64 binaries written in c++.

# Usage
```
parse-elf binary-file
```

# Current Features
- Print elf headers
- Print Section Header Tables
- Print Symbol Tables
- Print Relocation Tables

# To Do
- add a makefile
- add command line argument parsing for better usability
- prints section types in the section header table wrong for certain types
- segment header tables
