# CLI Text Editor in C

A **Command Line Interface (CLI) Text Editor** developed in **C** that provides essential text editing capabilities such as inserting, deleting, searching, replacing, file handling, clipboard operations, and undo/redo functionality. The editor is implemented using modular programming principles, making the code organized, maintainable, and easy to extend.

---

# Features

* Add a new line
* Insert text at the cursor position
* Delete a character
* Delete an entire line
* Search for text
* Replace text
* Copy selected text
* Cut selected text
* Paste copied text
* Undo previous operation
* Redo undone operation
* Move cursor up
* Move cursor down
* Move cursor left
* Move cursor right
* Display current document
* Open an existing text file
* Save the document to a file

---

# Project Structure

```
Text_Editor/
│
├── include/
│   ├── *.h
│
├── src/
│   ├── *.c
│
├── makefile
├── README.md
└── fileSave.txt
```

---

# Technologies Used

* Programming Language : C
* Compiler : GCC
* Operating System : Linux
* Build Tool : Make
* Data Structures :

  * Doubly Linked List
  * Stack (Undo/Redo)
  * Dynamic Memory Allocation

---

# Functional Modules

### Text Editing

* Add Line
* Insert Text
* Delete Character
* Delete Line

### Search & Replace

* Search for text
* Replace existing text

### Clipboard Operations

* Copy
* Cut
* Paste

### Undo & Redo

* Undo previous modifications
* Redo reverted changes

### Cursor Navigation

* Move cursor up
* Move cursor down
* Move cursor left
* Move cursor right

### File Operations

* Open text file
* Save text file

---

# Supported Commands

| Command             | Description                |
| ------------------- | -------------------------- |
| addline <text>      | Add a new line             |
| insert <text>       | Insert text at cursor      |
| delete              | Delete character           |
| deleteline          | Delete current line        |
| undo                | Undo last operation        |
| redo                | Redo last undone operation |
| search <text>       | Search text                |
| replace <old> <new> | Replace text               |
| display             | Display document           |
| copy                | Copy selected text         |
| cut                 | Cut selected text          |
| paste               | Paste copied text          |
| openfile <filename> | Open an existing file      |
| savefile            | Save current document      |
| moveup              | Move cursor up             |
| movedown            | Move cursor down           |
| moveleft            | Move cursor left           |
| moveright           | Move cursor right          |
| help                | Display available commands |
| exit                | Exit the editor            |

---

# Compilation

Compile the project using the Makefile.

```bash
make
```

or

```bash
gcc src/*.c -Iinclude -o text_editor
```

---

# Running the Project

```bash
./text_editor
```

---

# Example

```
> addline Hello World

> addline Welcome to CLI Text Editor

> display

1. Hello World
2. Welcome to CLI Text Editor

> search World

Found at Line 1

> replace World Everyone

> savefile

File saved successfully.
```

---

# Learning Outcomes

* Modular Programming in C
* Dynamic Memory Management
* Linked List Operations
* Stack Implementation
* File Handling
* String Manipulation
* Command Line Interface Development
* Undo/Redo Design
* Software Modularity

---

# Future Enhancements

* Multiple file support
* Syntax highlighting
* Word wrapping
* Line numbering
* Keyboard shortcuts
* Find and Replace with regular expressions
* Auto-save functionality
* Text formatting support

---

# Author

**Varsha S**

B.E. Electronics and Communication Engineering

Embedded Systems Trainee

---

# License

This project is developed for educational and learning purposes.
