# parse-elf
`parse-elf` is a simple parser for elf64 binaries written in c++.

# usage
```
parser-elf binary-file
```

# current features
- Print elf headers
- Print Section Header Tables
- Print Symbol Tables
- Print Relocation Tables

# to do
- add a makefile
- add command line argument parsing for better usability
- prints section types in the section header table wrong for certain types
- segment header tables
