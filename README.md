# Mini OS Process Manager

🧠 A lightweight multithreaded process manager written in C, designed to simulate the core control flow of an operating system process lifecycle: `create`, `start`, `pause`, `resume`, and `exit`.

> 📦 Built entirely from scratch using only POSIX threads (pthreads).  
> 🧵 Simulates process & UI thread orchestration with shared control over synchronization primitives.

---

## Features

- ✅ Process creation with dynamic UI-thread binding  
- ✅ Thread lifecycle control (`start`, `pause`, `resume`, `exit`)  
- ✅ Custom thread argument structs for flexible data injection  
- ✅ Expandable UI thread system (`MAX_UI_BUFFER` size)  
- ✅ POSIX-compliant with mutex and condition variable synchronization  

---

## Architecture

```
[Main Thread]  <-->  [UI Threads x N]
     |
     |-- Uses shared mutex + cond var
     |-- Controlled by Process state: EMPTY, WORK, PAUSE, EXIT
```

Each `Process` manages:

- Its own **main thread**
- A dynamic array of **UI threads**
- Shared **state** across threads
- Internal **mutex/cond synchronization**
- Named ID (`char* name`) and **PID** (self-pointer)

---

## Getting Started

### 🔧 Requirements

- POSIX-compatible compiler (e.g., `gcc`)
- OS: Linux / macOS / Windows (MinGW-w64 or WSL)

### ⚙️ Compile

```bash
gcc -g main.c process/process.c process/processList/testProcess.c -o main -lpthread
./main
```

> 💡 Replace the file list as needed, depending on your file structure.

---

## File Structure

```
MiniOS-ProcessManager/
├── main.c
├── process/
│   ├── process.h        # Public process interface
│   ├── private.h        # Internal struct & enum definitions
│   ├── process.c        # Implementation of Process logic
│   └── processList/
│       └── testProcess.c
│       └── testProcess.h
└── README.md
```

---

## License

MIT License

Copyright (c) 2025-07-24 toucan8187@gmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.