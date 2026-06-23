#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

#include "filehandler.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"

void banner(const string& title) {
    cout << "\n" << string(55, '=') << "\n";
    cout << "  " << title << "\n";
    cout << string(55, '=') << "\n";
}

void menuStudents() {
    int choice;
    do {
        banner("STUDENT MANAGEMENT");
        cout << "  1. Add Student\n";
        cout << "  2. Search by Roll Number\n";
        cout << "  3. Search by Name\n";
        cout << "  4. Update Student Info\n";
        cout << "  5. Soft Delete Student\n";
        cout << "  6. List All Active Students\n";
        cout << "  7. Search As You Type (Bonus)\n";
        cout << "  0. Back\n";
        cout << "\n  Choice: ";
        cin >> choice;

        if (choice == 1) {
            addStudent();
        } else if (choice == 2) {
            string roll;
            cout << "  Enter Roll No: "; cin >> roll;
            vector<string> s = searchByRoll(roll);
            if (s.empty()) cout << "  [Error] Student not found.\n";
            else printStudent(s);
        } else if (choice == 3) {
            string name;
            cin.ignore();
            cout << "  Enter Name (partial): "; getline(cin, name);
            vector<vector<string>> results = searchByName(name);
            if (results.empty()) cout << "  No students found.\n";
            for (int i = 0; i < (int)results.size(); i++) {
                cout << "\n  --- Result " << (i+1) << " ---\n";
                printStudent(results[i]);
            }
        } else if (choice == 4) {
            string roll, field, val;
            cout << "  Enter Roll No: "; cin >> roll;
            cout << "  Field (name/department/semester/cgpa/status): "; cin >> field;
            cin.ignore();
            cout << "  New value: "; getline(cin, val);
            updateStudent(roll, field, val);
        } else if (choice == 5) {
            string roll;
            cout << "  Enter Roll No: "; cin >> roll;
            softDelete(roll);
        } else if (choice == 6) {
            vector<vector<string>> list = listActiveStudents();
            cout << "\n" << string(60, '-') << "\n";
            cout << left << setw(15) << "Roll No" << setw(25) << "Name" << setw(6) << "CGPA" << "\n";
            cout << string(60, '-') << "\n";
            for (int i = 0; i < (int)list.size(); i++) {
                if (list[i].size() < 5) continue;
                cout << left << setw(15) << list[i][0] << setw(25) << list[i][1] << setw(6) << list[i][4] << "\n";
            }
            cout << "  Total: " << list.size() << "\n";
        } else if (choice == 7) {
            cin.ignore();
            string query = "";
            cout << "\n  [Search As You Type] Empty input = quit.\n";
            while (true) {
                cout << "  Query [" << query << "] + char: ";
                string ch; getline(cin, ch);
                if (ch.empty()) break;
                query += ch;
                vector<vector<string>> res = searchByName(query);
                cout << "\n  --- Matching '" << query << "' ---\n";
                if (res.empty()) { cout << "  (no matches)\n"; continue; }
                cout << left << setw(15) << "Roll No" << setw(25) << "Name" << "\n";
                for (int i = 0; i < (int)res.size(); i++)
                    if (res[i].size() >= 2)
                        cout << setw(15) << res[i][0] << setw(25) << res[i][1] << "\n";
            }
        }
    } while (choice != 0);
}

void menuCourses() {
    int choice;
    do {
        banner("COURSE & ENROLLMENT");
        cout << "  1. Enroll Student in Course\n";
        cout << "  2. Drop a Course\n";
        cout << "  3. View Credit Load\n";
        cout << "  4. List Enrolled Students\n";
        cout << "  0. Back\n";
        cout << "\n  Choice: ";
        cin >> choice;

        if (choice == 1) {
            string roll, code, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Course Code: "; cin >> code;
            cout << "  Semester: "; cin >> sem;
            EnrollResult r = enrollStudent(roll, code, sem);
            printEnrollResult(r);
        } else if (choice == 2) {
            string roll, code, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Course Code: "; cin >> code;
            cout << "  Semester: "; cin >> sem;
            bool ok = dropCourse(roll, code, sem);
            if (ok) cout << "  [Success] Course dropped.\n";
            else cout << "  [Error] Cannot drop — attendance exists or not enrolled.\n";
        } else if (choice == 3) {
            string roll, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Semester: "; cin >> sem;
            cout << "  Credit Load: " << getCreditLoad(roll, sem) << " hrs\n";
        } else if (choice == 4) {
            string code;
            cout << "  Course Code: "; cin >> code;
            vector<vector<string>> list = listEnrolledStudents(code);
            cout << "\n  Enrolled in " << code << ":\n" << string(30,'-') << "\n";
            for (int i = 0; i < (int)list.size(); i++)
                if (list[i].size() >= 2) cout << "  " << list[i][1] << "\n";
            cout << "  Total: " << list.size() << "\n";
        }
    } while (choice != 0);
}

void menuAttendance() {
    int choice;
    do {
        banner("ATTENDANCE");
        cout << "  1. Mark Attendance\n";
        cout << "  2. View Attendance %\n";
        cout << "  3. Shortage List\n";
        cout << "  4. Undo Last Session\n";
        cout << "  5. Print Daily Sheet\n";
        cout << "  0. Back\n";
        cout << "\n  Choice: ";
        cin >> choice;

        if (choice == 1) {
            string code, date, sem;
            cout << "  Course Code: "; cin >> code;
            cout << "  Date (DD-MM-YYYY): "; cin >> date;
            cout << "  Semester: "; cin >> sem;
            markAttendance(code, date, sem);
        } else if (choice == 2) {
            string roll, code;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Course Code: "; cin >> code;
            double pct = getAttendancePct(roll, code);
            cout << fixed << "  Attendance: " << pct << "%";
            if (pct < 75.0) cout << "  *** SHORTAGE ***";
            cout << "\n";
        } else if (choice == 3) {
            string code;
            cout << "  Course Code: "; cin >> code;
            vector<vector<string>> list = getShortageList(code);
            cout << "\n  Shortage List (<75%) for " << code << ":\n";
            if (list.empty()) cout << "  None.\n";
            for (int i = 0; i < (int)list.size(); i++)
                if (list[i].size() >= 3)
                    cout << "  " << list[i][0] << " -> " << list[i][2] << "%\n";
        } else if (choice == 4) {
            string code, date;
            cout << "  Course Code: "; cin >> code;
            cout << "  Date (DD-MM-YYYY): "; cin >> date;
            undoLastSession(code, date);
        } else if (choice == 5) {
            string code, date;
            cout << "  Course Code: "; cin >> code;
            cout << "  Date (DD-MM-YYYY): "; cin >> date;
            printDailySheet(code, date);
        }
    } while (choice != 0);
}

void menuGrades() {
    int choice;
    do {
        banner("GRADES");
        cout << "  1. Enter Marks\n";
        cout << "  2. View Grade\n";
        cout << "  3. Semester GPA\n";
        cout << "  4. Class Statistics\n";
        cout << "  5. Apply Attendance Penalty\n";
        cout << "  0. Back\n";
        cout << "\n  Choice: ";
        cin >> choice;

        if (choice == 1) {
            string roll, code, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Course Code: "; cin >> code;
            cout << "  Semester: "; cin >> sem;
            enterMarks(roll, code, sem);
        } else if (choice == 2) {
            string roll, code;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Course Code: "; cin >> code;
            vector<vector<string>> rows = readTXT(GRADES_FILE);
            bool found = false;
            for (int i = 0; i < (int)rows.size(); i++) {
                if (rows[i].size() >= 3 && rows[i][0] == roll && rows[i][1] == code) {
                    printGrade(rows[i]); found = true; break;
                }
            }
            if (!found) cout << "  [Error] Grade record not found.\n";
        } else if (choice == 3) {
            string roll, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Semester: "; cin >> sem;
            cout << fixed << setprecision(2) << "  Semester GPA: " << computeGPA(roll, sem) << "\n";
        } else if (choice == 4) {
            string code;
            cout << "  Course Code: "; cin >> code;
            Stats s = computeClassStats(code);
            cout << fixed << setprecision(2);
            cout << "  Highest: " << s.highest << "\n  Lowest: " << s.lowest
                 << "\n  Mean: " << s.mean << "\n  Median: " << s.median << "\n";
        } else if (choice == 5) {
            string roll, code;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Course Code: "; cin >> code;
            applyAttendancePenalty(roll, code);
        }
    } while (choice != 0);
}

void menuFees() {
    int choice;
    do {
        banner("FEE TRACKER");
        cout << "  1. Record Payment\n";
        cout << "  2. Compute Late Fine\n";
        cout << "  3. Generate Receipt\n";
        cout << "  4. View Fee Defaulters\n";
        cout << "  0. Back\n";
        cout << "\n  Choice: ";
        cin >> choice;

        if (choice == 1) {
            string roll, sem, paidDate;
            double amount;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Semester: "; cin >> sem;
            cout << "  Amount (Rs.): "; cin >> amount;
            cout << "  Payment Date (DD-MM-YYYY): "; cin >> paidDate;
            recordPayment(roll, sem, amount, paidDate);
        } else if (choice == 2) {
            string roll, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Semester: "; cin >> sem;
            cout << fixed << setprecision(2) << "  Late Fine: Rs." << computeLateFine(roll, sem) << "\n";
        } else if (choice == 3) {
            string roll, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Semester: "; cin >> sem;
            generateReceipt(roll, sem);
        } else if (choice == 4) {
            printFeeDefaulters();
        }
    } while (choice != 0);
}

void menuReports() {
    int choice;
    do {
        banner("REPORTS");
        cout << "  1. Merit List\n";
        cout << "  2. Attendance Defaulters\n";
        cout << "  3. Fee Defaulters\n";
        cout << "  4. Semester Result Sheet\n";
        cout << "  5. Department Summary\n";
        cout << "  6. Export Report to File\n";
        cout << "  0. Back\n";
        cout << "\n  Choice: ";
        cin >> choice;

        if (choice == 1) { printMeritList(); }
        else if (choice == 2) { printAttendanceDefaulters(); }
        else if (choice == 3) { printFeeDefaulters(); }
        else if (choice == 4) {
            string roll, sem;
            cout << "  Roll No: "; cin >> roll;
            cout << "  Semester: "; cin >> sem;
            printSemesterResult(roll, sem);
        } else if (choice == 5) { printDepartmentSummary(); }
        else if (choice == 6) {
            int rc; string fname;
            cout << "  Report (1=Merit 2=AttDefault 3=FeeDefault 4=DeptSummary): "; cin >> rc;
            cout << "  Filename: "; cin >> fname;
            exportReportToFile(rc, fname);
        }
    } while (choice != 0);
}

int main() {
    cout << "\n";
    cout << "  ╔══════════════════════════════════════════╗\n";
    cout << "  ║    CAMPUS ANALYTICS ENGINE v1.0          ║\n";
    cout << "  ║    BS Artificial Intelligence            ║\n";
    cout << "  ╚══════════════════════════════════════════╝\n";

    int choice;
    do {
        banner("MAIN MENU");
        cout << "  1. Student Management\n";
        cout << "  2. Course & Enrollment\n";
        cout << "  3. Attendance\n";
        cout << "  4. Grades\n";
        cout << "  5. Fee Tracker\n";
        cout << "  6. Reports\n";
        cout << "  0. Exit\n";
        cout << "\n  Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: menuStudents();   break;
            case 2: menuCourses();    break;
            case 3: menuAttendance(); break;
            case 4: menuGrades();     break;
            case 5: menuFees();       break;
            case 6: menuReports();    break;
            case 0: cout << "\n  Goodbye!\n\n"; break;
            default: cout << "  [Error] Invalid choice.\n";
        }
    } while (choice != 0);

    return 0;
}