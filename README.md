# Corth
## Exaples:

Hello, World:
```corth
func main -- do
    "Hello, World!\n" .
end
```


Print from A to Z:
```corth
func main -- do
    'A' cast(int) while dup 'Z' cast(int) <= do
        cast(char) dup .
        cast(int)
        1 +
    end drop
end
```

## Quick Start

### Requirements
-linux(or wsl)<br>
-gcc<br>
-python3<br>
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
