# Linux File System Explorer Using System Calls

## 📌 Project Overview

**Linux File System Explorer** is a command-line based file management application developed in **C** for the Linux operating system. The project demonstrates how an operating system interacts with files and directories through **Linux system calls and POSIX APIs**.

Instead of relying entirely on high-level file-management utilities, the application directly uses operating-system interfaces to perform common file-system operations.

The project provides an interactive menu through which users can explore and manage files and directories.

---

## 🎯 Objectives

The main objectives of this project are:

* Understand Linux file-system operations.
* Demonstrate the use of Linux system calls.
* Perform file operations using C.
* Navigate through directories.
* Create and delete files/directories.
* Read and write file contents.
* Display file metadata.
* Understand file descriptors.
* Demonstrate how user applications interact with the Linux kernel.

---

## 🛠️ Technologies Used

| Technology             | Purpose                         |
| ---------------------- | ------------------------------- |
| **C**                  | Core programming language       |
| **Linux / Ubuntu**     | Operating-system environment    |
| **GCC**                | C compiler                      |
| **Linux System Calls** | File-system operations          |
| **POSIX APIs**         | Directory and file handling     |
| **VS Code**            | Development environment         |
| **WSL / Ubuntu**       | Windows-based Linux environment |

---

## 📂 Main Features

The File System Explorer provides the following operations:

1. **List Directory**
2. **Change Directory**
3. **Go to Parent Directory**
4. **Display File Information**
5. **Read File**
6. **Create File**
7. **Create Directory**
8. **Delete File**
9. **Delete Directory**
10. **Write/Append to File**
11. **Exit**

The exact menu may vary depending on the final implementation.

---

## ⚙️ System Calls / APIs Used

Important Linux/POSIX interfaces used in the project include:

### File Operations

```c
open()
read()
write()
close()
```

These are used for opening, reading, writing, and closing files.

### File Information

```c
stat()
```

Used to obtain information such as:

* File size
* Permissions
* File type
* Owner information
* Modification time

### Directory Operations

```c
opendir()
readdir()
closedir()
```

These are POSIX directory APIs used to traverse directory contents.

### Directory/File Management

```c
mkdir()
unlink()
rmdir()
chdir()
getcwd()
```

These allow the application to:

* Create directories
* Delete files
* Delete directories
* Change the current directory
* Obtain the current working directory

---

## 🔄 How the Project Works

The application starts with a menu-driven interface.

```text
              START
                │
                ▼
       Display Current Directory
                │
                ▼
          Display Menu
                │
                ▼
       User Selects Operation
                │
       ┌────────┼─────────┐
       │        │         │
       ▼        ▼         ▼
   Directory   File     Metadata
   Operations Operations Operations
       │        │         │
       └────────┼─────────┘
                ▼
       Execute Linux/POSIX API
                │
                ▼
        Display Result
                │
                ▼
          Return to Menu
                │
                ▼
              EXIT
```

---

## ▶️ How to Run

### 1. Open Ubuntu / WSL

Navigate to the project directory:

```bash
cd path/to/Linux-File-System-Explorer
```

### 2. Compile the program

If the source file is `main.c`:

```bash
gcc main.c -o file_explorer
```

For multiple source files:

```bash
gcc *.c -o file_explorer
```

### 3. Run

```bash
./file_explorer
```

You should see the interactive file-system explorer menu.

---

## 🖥️ Example Menu

```text
============================================
       LINUX FILE SYSTEM EXPLORER
============================================

Current Directory:
/home/jay

1. List Directory
2. Change Directory
3. Go to Parent Directory
4. File Information
5. Read File
6. Create File
7. Create Directory
8. Delete File
9. Delete Directory
10. Write to File
11. Exit

Enter your choice:
```

---

## 🧪 Example Operations

### List Directory

The program reads the current directory and displays its contents.

```text
Files and Directories:

1. Documents
2. Downloads
3. project
4. main.c
5. README.md
```

### Create File

```text
Enter file name: test.txt

File created successfully.
```

### Read File

```text
Enter file name: test.txt

----- File Content -----

Hello Linux!
This file was created using the File System Explorer.

------------------------
```

### File Information

```text
File: test.txt

Size       : 72 bytes
Permissions: rw-r--r--
Type       : Regular File
```

---

## 🧠 Operating System Concepts Demonstrated

This project demonstrates several important Operating System concepts:

### 1. System Calls

The application communicates with the Linux operating system using system-call interfaces such as:

```c
open()
read()
write()
close()
stat()
mkdir()
unlink()
```

### 2. File Descriptors

When a file is opened using `open()`, Linux returns a **file descriptor**.

Example:

```c
int fd = open("test.txt", O_RDONLY);
```

The returned `fd` identifies the opened file to subsequent operations.

### 3. File Management

The project demonstrates how Linux manages:

* Files
* Directories
* Permissions
* Metadata
* File descriptors

### 4. Directory Management

Directory traversal and navigation demonstrate how applications interact with the Linux file-system hierarchy.

### 5. Kernel Interaction

The project provides a practical demonstration of how a user-level program requests file-system services from the Linux kernel.

---

## 📁 Suggested Project Structure

```text
Linux-File-System-Explorer/
│
├── main.c
├── file_operations.c
├── directory_operations.c
├── file_operations.h
├── directory_operations.h
├── README.md
└── screenshots/
```

If your implementation currently has everything in one C file, the structure can simply be:

```text
Linux-File-System-Explorer/
│
├── main.c
└── README.md
```

---

## 🔐 Error Handling

The application should check whether file-system operations succeed.

For example:

```c
if (fd == -1) {
    perror("Error opening file");
}
```

This allows the program to display meaningful Linux error messages when:

* A file does not exist.
* Permission is denied.
* A directory cannot be created.
* A file cannot be deleted.
* An invalid path is entered.

---

## 🚀 Future Enhancements

Possible future improvements include:

* Graphical web dashboard.
* File search functionality.
* File copy and move operations.
* File permission modification.
* File sorting.
* Disk-space monitoring.
* Hidden-file support.
* Multiple-tab directory navigation.
* File preview.
* User authentication.
* Real-time file-system monitoring using Linux facilities such as `inotify`.

---

## 👨‍💻 Author

**Sunkara Jayanth**
**Roll Number:** 2520030421
**B.Tech – Computer Science and Engineering**
**K L (Deemed to be) University**

---

## 📜 License

This project was developed for **academic/educational purposes** to demonstrate Linux operating-system concepts, file management, and system-call programming.
