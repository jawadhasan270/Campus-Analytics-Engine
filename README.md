
# Campus Analytics Engine

A multi-file, menu-driven data analytics system for a fictional university campus, built using only fundamental C++ programming constructs (no OOP, no STL algorithms).

**Course:** Programming Fundamentals — BS Artificial Intelligence

---

## Features

- **Student Management** — Add, search, update, and soft-delete students with roll number format validation (`BSAI-YY-XXX`)
- **Course & Enrollment** — Enroll/drop students with prerequisite checks and 21-credit-hour load validation
- **Attendance Tracking** — Mark daily attendance (Present/Absent/Late), compute attendance %, flag students below 75%, undo last session
- **Grades** — Enter quiz/assignment/mid/final marks, compute weighted totals, letter grades, and semester GPA
- **Fee Tracker** — Record payments, calculate late fines (2% per week overdue) using manual date arithmetic, generate receipts
- **Reports** — Merit list, attendance defaulters, fee defaulters, semester result sheets, department summary, and export to file
- **Bonus:** Search-as-you-type for student names (live filtering using `substr`)

---

## Project Structure