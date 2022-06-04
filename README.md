# Corth
## Exaples:

Hello, World:
```corth
'H' .
'e' .
'l' .
'l' .
'o' .
',' .
' ' .
'W' .
'o' .
'r' .
'l' .
'd' .
'\n' .
```
Corth don't suport strings yet


Print from A to Z:
```corth
'A' cast(int) while dup 'Z' cast(int) 1 + < do
    cast(char) dup .
    cast(int)
    1 +
end drop
```

## Quick Start

### Requirements
    -linux(or wsl)
    -gcc compiler
    -[fasm](https://flatassembler.net/)

Build:
```console
./build.py
```

Run:
```console
./corth your_file.corth
```

Type './build.py -run' to build std.corth
```consonle
./build.py -run
```