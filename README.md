# 📁 Simple FAT-Style File System

## 🛠️ Description

This project implements a simplified file system inspired by the FAT (File Allocation Table) design. It runs on top of a virtual disk stored as a binary file, simulating core file system operations such as creating, deleting, and listing files.

It was created for an educational assignment in a systems or operating systems course, focusing on how file systems interact with disks at a low level.

---

## 📦 Features

- Create and delete files
- List existing files
- Mount and unmount a file system
- Persistent disk structure saved to a binary file
- Superblock and simple root directory management
- Custom implementation of low-level read/write to disk sectors
