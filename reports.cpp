#include "reports.h"
#include "filehandler.h"
#include "student_ops.h"
#include "grades.h"
#include "attendance.h"
#include "fee_tracker.h"
#include "course_ops.h"
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

static double strToDoublR(const string& s) {
    double intP = 0, fracP = 0, fracD = 1;
    bool inFrac = false;
    for (int i = 0; i < (int)s.size(); i++) {
        if (s[i] == '.') inFrac = true;
        else if (isdigit(s[i])) {
            if (!inFrac) intP = intP * 10 + (s[i] - '0');
            else { fracP = fracP * 10 + (s[i] - '0'); fracD *= 10; }
        }
    }
    return intP + fracP / fracD;
}

void printMeritList() {
    vector<vector<string>> students = listActiveStudents();

    for (int i = 0; i < (int)students.size() - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < (int)students.size(); j++) {
            double cgpaJ   = strToDoublR(students[j][4]);
            double cgpaMax = strToDoublR(students[maxIdx][4]);
            if (cgpaJ > cgpaMax) maxIdx = j;
        }
        if (maxIdx != i) {
            vector<string> tmp = students[i];
            students[i] = students[maxIdx];
            students[maxIdx] = tmp;
        }
    }

    cout << "\n" << string(65, '=') << "\n";
    cout << setw(35) << "         MERIT LIST\n";
    cout << string(65, '=') << "\n";
    cout << left
         << setw(6)  << "Rank"
         << setw(15) << "Roll No"
         << setw(25) << "Name"
         << setw(8)  << "CGPA" << "\n";
    cout << string(65, '-') << "\n";

    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i].size() < 5) continue;
        cout << left
             << setw(6)  << (i + 1)
             << setw(15) << students[i][0]
             << setw(25) << students[i][1]
             << setw(8)  << students[i][4] << "\n";
    }
    cout << string(65, '=') << "\n";
}

void printAttendanceDefaulters() {
    vector<vector<string>> courses = readTXT(COURSES_FILE);

    cout << "\n" << string(65, '=') << "\n";
    cout << setw(35) << "    ATTENDANCE DEFAULTERS (<75%)\n";
    cout << string(65, '=') << "\n";
    cout << left
         << setw(15) << "Roll No"
         << setw(10) << "Course"
         << setw(12) << "Attendance" << "\n";
    cout << string(65, '-') << "\n";

    bool anyFound = false;
    for (int i = 0; i < (int)courses.size(); i++) {
        if (courses[i].empty()) continue;
        string code = courses[i][0];
        vector<vector<string>> shortage = getShortageList(code);
        for (int j = 0; j < (int)shortage.size(); j++) {
            if (shortage[j].size() < 3) continue;
            cout << left
                 << setw(15) << shortage[j][0]
                 << setw(10) << shortage[j][1]
                 << setw(12) << (shortage[j][2] + "%") << "\n";
            anyFound = true;
        }
    }
    if (!anyFound) cout << "  No attendance defaulters found.\n";
    cout << string(65, '=') << "\n";
}

void printFeeDefaulters() {
    vector<vector<string>> defaulters = getDefaulters();

    cout << "\n" << string(70, '=') << "\n";
    cout << setw(35) << "         FEE DEFAULTERS\n";
    cout << string(70, '=') << "\n";
    cout << left
         << setw(15) << "Roll No"
         << setw(10) << "Semester"
         << setw(15) << "Outstanding"
         << setw(12) << "Due Date" << "\n";
    cout << string(70, '-') << "\n";

    if (defaulters.empty()) {
        cout << "  No fee defaulters found.\n";
    }
    for (int i = 0; i < (int)defaulters.size(); i++) {
        if (defaulters[i].size() < 6) continue;
        cout << left
             << setw(15) << defaulters[i][0]
             << setw(10) << defaulters[i][1]
             << setw(15) << ("Rs. " + defaulters[i][4])
             << setw(12) << defaulters[i][5] << "\n";
    }
    cout << string(70, '=') << "\n";
}

void printSemesterResult(const string& roll, const string& semester) {
    vector<string> stu = searchByRoll(roll);
    string name = (stu.size() >= 2) ? stu[1] : "Unknown";

    vector<vector<string>> grades = readTXT(GRADES_FILE);

    cout << "\n" << string(75, '*') << "\n";
    cout << "  SEMESTER RESULT SHEET\n";
    cout << "  Student: " << name << "  |  Roll: " << roll << "  |  Semester: " << semester << "\n";
    cout << string(75, '*') << "\n";
    cout << left
         << setw(10) << "Course"
         << setw(10) << "Quiz%"
         << setw(10) << "Asgn%"
         << setw(8)  << "Mid"
         << setw(8)  << "Final"
         << setw(10) << "Total%"
         << setw(8)  << "Grade"
         << setw(10) << "Attend%" << "\n";
    cout << string(75, '-') << "\n";

    bool any = false;
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() < 9) continue;
        if (grades[i][0] != roll || grades[i][2] != semester) continue;

        double attPct = getAttendancePct(roll, grades[i][1]);
        int aInt = (int)attPct;
        string attStr = "";
        if (aInt == 0) attStr = "0";
        else { int t = aInt; while (t > 0) { attStr = char('0'+t%10)+attStr; t /= 10; } }
        attStr += "%";

        cout << left
             << setw(10) << grades[i][1]
             << setw(10) << grades[i][3]
             << setw(10) << grades[i][4]
             << setw(8)  << grades[i][5]
             << setw(8)  << grades[i][6]
             << setw(10) << grades[i][7]
             << setw(8)  << grades[i][8]
             << setw(10) << attStr << "\n";
        any = true;
    }
    if (!any) cout << "  No grade records found for this semester.\n";

    double gpa = computeGPA(roll, semester);
    cout << string(75, '-') << "\n";
    cout << "  Semester GPA: " << fixed << setprecision(2) << gpa << "\n";
    cout << string(75, '*') << "\n";
}

void printDepartmentSummary() {
    vector<vector<string>> students = readTXT(STUDENTS_FILE);

    string depts[50];
    int deptCount[50];
    double deptCGPA[50];
    int deptActive[50];
    int numDepts = 0;

    for (int i = 0; i < (int)students.size(); i++) {
        if (students[i].size() < 6) continue;
        if (students[i][5] != "active") continue;
        string dept = students[i][2];
        double cgpa = strToDoublR(students[i][4]);

        bool found = false;
        for (int j = 0; j < numDepts; j++) {
            if (depts[j] == dept) {
                deptCount[j]++;
                deptCGPA[j] += cgpa;
                if (cgpa >= 2.0) deptActive[j]++;
                found = true;
                break;
            }
        }
        if (!found && numDepts < 50) {
            depts[numDepts] = dept;
            deptCount[numDepts] = 1;
            deptCGPA[numDepts] = cgpa;
            deptActive[numDepts] = (cgpa >= 2.0) ? 1 : 0;
            numDepts++;
        }
    }

    cout << "\n" << string(70, '=') << "\n";
    cout << setw(35) << "     DEPARTMENT SUMMARY\n";
    cout << string(70, '=') << "\n";
    cout << left
         << setw(30) << "Department"
         << setw(10) << "Students"
         << setw(12) << "Avg CGPA"
         << setw(12) << "Pass Rate" << "\n";
    cout << string(70, '-') << "\n";

    for (int i = 0; i < numDepts; i++) {
        double avg = (deptCount[i] > 0) ? deptCGPA[i] / deptCount[i] : 0.0;
        double passRate = (deptCount[i] > 0) ? (double)deptActive[i] / deptCount[i] * 100.0 : 0.0;

        int prInt = (int)passRate;
        string prStr = "";
        if (prInt == 0) prStr = "0%";
        else { int t = prInt; while (t > 0) { prStr = char('0'+t%10)+prStr; t /= 10; } prStr += "%"; }

        cout << left
             << setw(30) << depts[i]
             << setw(10) << deptCount[i]
             << setw(12) << fixed << setprecision(2) << avg
             << setw(12) << prStr << "\n";
    }
    cout << string(70, '=') << "\n";
}

void exportReportToFile(int reportChoice, const string& filename) {
    ofstream fout(filename.c_str());
    if (!fout.is_open()) {
        cout << "  [Error] Cannot create file: " << filename << "\n";
        return;
    }
    streambuf* coutBuf = cout.rdbuf();
    cout.rdbuf(fout.rdbuf());

    switch (reportChoice) {
        case 1: printMeritList(); break;
        case 2: printAttendanceDefaulters(); break;
        case 3: printFeeDefaulters(); break;
        case 4: printDepartmentSummary(); break;
        default: cout << "Invalid report choice.\n"; break;
    }

    cout.rdbuf(coutBuf);
    fout.close();
    cout << "  [Success] Report exported to " << filename << "\n";
}