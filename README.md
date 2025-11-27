# Student Grade Manager – Multithreading Study

![Language: C](https://img.shields.io/badge/Language-C-555555.svg)
![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)
![Status: Complete](https://img.shields.io/badge/Status-Complete-green.svg)
![Repo](https://img.shields.io/badge/Repo-Student--Grade--Manager-blue.svg)

**Author:** Marwane Mohammedi

**University:** University Of Belhadj Bouchaib Ain Temouchent

**Repository:** [https://github.com/iiiMart1/Student-Grade-Manager](https://github.com/iiiMart1/Student-Grade-Manager)

---

## 📖 Overview

A C-based **Student Grade Management System** with multithreading support for sorting and average calculations. Features include:

- Add, display, and manage student records
- Calculate averages and statistics
- Sort by average or alphabetical order (single-threaded & multithreaded)
- Search students by name
- Performance evaluation for multithreaded algorithms

This project demonstrates the **impact of multithreading** on small and medium datasets.

---

## ⚙️ Features

- Add students with grades (TD, Exam, Total)
- Display all students with averages
- Calculate class averages
- Sort students by average or name
- Search students by name (single-threaded or multithreaded)
- View statistics (top/bottom N students, grade distribution)

---

## 🧮 Algorithms Used

### Merge Sort

- **Principle:** Divide-and-conquer sorting algorithm
- **Time Complexity:** O(n log n) (best, average, worst)
- **Multithreading:** Subarrays sorted concurrently

### Quick Sort

- **Principle:** Partition-based recursive sort
- **Time Complexity:** Best/Average: O(n log n), Worst: O(n²)
- **Multithreading:** Left/right partitions sorted concurrently

### Name Sort (Quick Sort)

- **Principle:** Alphabetical sorting using string comparison
- **Time Complexity:** Best/Average: O(n log n), Worst: O(n²)
- **Multithreading:** Partition threads like Quick Sort

---

## ⚡ Performance Evaluation

| Test Case | Dataset Size | Algorithm    | Threads | Time (ms) | Improvement |
| --------- | ------------ | ------------ | ------- | --------- | ----------- |
| 1         | 100          | Merge Sort   | 1       | 2342.75   |             |
| 2         | 100          | Merge Sort   | multi   | 2291.76   | 1.02x       |
| 3         | 100          | Quick Sort   | 1       | 2328.33   |             |
| 4         | 100          | Quick Sort   | multi   | 2255.02   | 1.03x       |
| 5         | 100          | Sort-by-name | 1       | 2357.93   |             |
| 6         | 100          | Sort-by-name | multi   | 2206.97   | 1.07x       |
| 7         | 1,000        | Merge Sort   | 1       | 22805.27  |             |
| 8         | 1,000        | Merge Sort   | multi   | 22658.44  | 1.01x       |
| 9         | 1,000        | Quick Sort   | 1       | 22965.92  |             |
| 10        | 1,000        | Quick Sort   | multi   | 22943.25  | 1.01x       |
| 11        | 1,000        | Sort-by-name | 1       | 22615.37  |             |
| 12        | 1,000        | Sort-by-name | multi   | 22749.36  | 0.99x       |

> **Observation:** Small datasets show minor improvements due to thread overhead. Larger datasets see more benefit.

---

## 🛠️ Compilation

### Linux / macOS

```bash
gcc -pthread -o student_manager main.c students.c
```

### Windows (MinGW)

```bash
gcc -o Student_Grade_Manager src/main.c src/students.c -Iinclude -lpthread
```

> Ensure `pthread` support is installed.

---

## 🚀 Usage

```bash
./student_manager   # Linux/macOS
student_manager.exe # Windows
```

**Menu Options:**

1. Add Student
2. Display Students
3. Calculate Averages
4. Sort Students
5. Find Student by Name
6. Find Student by Name (Multithreaded)
7. Create Test Data
8. Statistics
9. Exit

---

## 📊 Conclusion

- Multithreading improves performance for CPU-intensive tasks but can be slower for small datasets.
- Merge Sort and Quick Sort maintain O(n log n) behavior.
- Alphabetical sorting benefits slightly more due to string comparison overhead.
- Dataset size and CPU characteristics are key factors in multithreading efficiency.

---

## 📸 Screenshots

Check ScreenShots Folder

---

## 📝 License

MIT License
