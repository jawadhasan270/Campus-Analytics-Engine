#include "course_ops.h"
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
using namespace std;

const string COURSES_FILE    = "courses.txt";
const string ENROLLMENTS_FILE= "enrollments.txt";

const string COURSES_HEADER     = "course_code,course_name,credit_hours,instructor,capacity,enrolled,prerequisite";
const string ENROLLMENTS_HEADER = "enrollment_id,roll_no,course_code,semester,enrollment_date,status";

void printCourse(const vector<string>& c) {
    if (c.size() < 7) { cout << "  [Invalid course record]\n"; return; }
    cout << "  Code    : " << c[0] << "\n";
    cout << "  Title   : " << c[1] << "\n";
    cout << "  Credits : " << c[2] << "\n";
    cout << "  Instr.  : " << c[3] << "\n";
    cout << "  Seats   : " << c[4] << " (enrolled: " << c[5] << ")\n";
    cout << "  Prereq  : " << c[6] << "\n";
}

void printEnrollResult(EnrollResult r) {
    switch(r) {
        case ENROLL_SUCCESS:          cout << "  [Success] Student enrolled successfully.\n"; break;
        case ENROLL_STUDENT_INACTIVE: cout << "  [Error] Student is inactive.\n"; break;
        case ENROLL_COURSE_NOT_FOUND: cout << "  [Error] Course not found.\n"; break;
        case ENROLL_NO_SEATS:         cout << "  [Error] No seats available in this course.\n"; break;
        case ENROLL_ALREADY_ENROLLED: cout << "  [Error] Student is already enrolled in this course.\n"; break;
        case ENROLL_CREDIT_OVERLOAD:  cout << "  [Error] Credit load exceeds 21 hours.\n"; break;
        case ENROLL_PREREQ_NOT_MET:   cout << "  [Error] Prerequisite course not passed.\n"; break;
    }
}

// Update enrolled count in courses.txt by delta (+1 or -1)
void updateEnrolledCount(const string& courseCode, int delta) {
    vector<vector<string>> rows = readTXT(COURSES_FILE);
    for (int i = 0; i < (int)rows.size(); i++) {
        if (!rows[i].empty() && rows[i][0] == courseCode && rows[i].size() >= 7) {
            int count = 0;
            for (int j = 0; j < (int)rows[i][5].size(); j++) {
                if (isdigit(rows[i][5][j])) count = count * 10 + (rows[i][5][j] - '0');
            }
            count += delta;
            if (count < 0) count = 0;
            if (count == 0) { rows[i][5] = "0"; }
            else {
                string s = "";
                int tmp = count;
                while (tmp > 0) { s = char('0' + tmp % 10) + s; tmp /= 10; }
                rows[i][5] = s;
            }
            break;
        }
    }
    writeTXT(COURSES_FILE, COURSES_HEADER, rows);
}

int getCreditLoad(const string& roll, const string& semester) {
    vector<vector<string>> enrolls = readTXT(ENROLLMENTS_FILE);
    vector<vector<string>> courses = readTXT(COURSES_FILE);
    int total = 0;
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i].size() < 6) continue;
        if (enrolls[i][1] != roll || enrolls[i][3] != semester) continue;
        if (enrolls[i][5] != "active") continue;
        string code = enrolls[i][2];
        for (int j = 0; j < (int)courses.size(); j++) {
            if (courses[j].size() >= 3 && courses[j][0] == code) {
                int cr = 0;
                for (int k = 0; k < (int)courses[j][2].size(); k++) {
                    if (isdigit(courses[j][2][k])) cr = cr * 10 + (courses[j][2][k] - '0');
                }
                total += cr;
                break;
            }
        }
    }
    return total;
}

bool checkPrerequisite(const string& roll, const string& courseCode) {
    vector<string> courseRow = findRow(COURSES_FILE, 0, courseCode);
    if (courseRow.size() < 7) return false;
    string prereq = courseRow[6];
    if (prereq == "NONE" || prereq.empty()) return true;

    vector<vector<string>> grades = readTXT("grades.txt");
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() < 3) continue;
        if (grades[i][0] == roll && grades[i][1] == prereq) {
            for (int j = 2; j < (int)grades[i].size(); j++) {
                string g = grades[i][j];
                if (g == "A" || g == "B+" || g == "B" || g == "C+" || g == "C" || g == "D") return true;
                if (g == "F") return false;
            }
        }
    }
    return false;
}

EnrollResult enrollStudent(const string& roll, const string& courseCode, const string& semester) {
    vector<string> stu = searchByRoll(roll);
    if (stu.empty() || stu.size() < 6 || stu[5] != "active") return ENROLL_STUDENT_INACTIVE;

    vector<string> crs = findRow(COURSES_FILE, 0, courseCode);
    if (crs.empty()) return ENROLL_COURSE_NOT_FOUND;

    int capacity = 0, enrolled = 0;
    for (int k = 0; k < (int)crs[4].size(); k++) if (isdigit(crs[4][k])) capacity = capacity*10+(crs[4][k]-'0');
    for (int k = 0; k < (int)crs[5].size(); k++) if (isdigit(crs[5][k])) enrolled = enrolled*10+(crs[5][k]-'0');
    if (enrolled >= capacity) return ENROLL_NO_SEATS;

    vector<vector<string>> enrolls = readTXT(ENROLLMENTS_FILE);
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i].size() < 6) continue;
        if (enrolls[i][1] == roll && enrolls[i][2] == courseCode && enrolls[i][3] == semester && enrolls[i][5] == "active")
            return ENROLL_ALREADY_ENROLLED;
    }

    int credits = 0;
    for (int k = 0; k < (int)crs[2].size(); k++) if (isdigit(crs[2][k])) credits = credits*10+(crs[2][k]-'0');
    if (getCreditLoad(roll, semester) + credits > 21) return ENROLL_CREDIT_OVERLOAD;

    if (!checkPrerequisite(roll, courseCode)) return ENROLL_PREREQ_NOT_MET;

    string eid = "E";
    int num = (int)enrolls.size() + 1;
    string numStr = "";
    if (num == 0) numStr = "0000";
    else { int tmp = num; while (tmp > 0) { numStr = char('0'+tmp%10) + numStr; tmp /= 10; } }
    while ((int)numStr.size() < 4) numStr = "0" + numStr;
    eid += numStr;

    vector<string> newRow = {eid, roll, courseCode, semester, "01-01-2024", "active"};
    appendTXT(ENROLLMENTS_FILE, newRow);
    updateEnrolledCount(courseCode, 1);
    return ENROLL_SUCCESS;
}

bool dropCourse(const string& roll, const string& courseCode, const string& semester) {
    vector<vector<string>> att = readTXT("attendance_log.txt");
    for (int i = 0; i < (int)att.size(); i++) {
        if (att[i].size() < 5) continue;
        // columns: log_id=0, roll_no=1, course_code=2
        if (att[i][1] == roll && att[i][2] == courseCode) return false;
    }
    vector<vector<string>> enrolls = readTXT(ENROLLMENTS_FILE);
    bool found = false;
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i].size() < 6) continue;
        if (enrolls[i][1] == roll && enrolls[i][2] == courseCode && enrolls[i][3] == semester && enrolls[i][5] == "active") {
            enrolls[i][5] = "dropped";
            found = true;
        }
    }
    if (!found) return false;
    writeTXT(ENROLLMENTS_FILE, ENROLLMENTS_HEADER, enrolls);
    updateEnrolledCount(courseCode, -1);
    return true;
}

vector<vector<string>> listEnrolledStudents(const string& courseCode) {
    vector<vector<string>> enrolls = readTXT(ENROLLMENTS_FILE);
    vector<vector<string>> result;
    for (int i = 0; i < (int)enrolls.size(); i++) {
        if (enrolls[i].size() < 6) continue;
        if (enrolls[i][2] == courseCode && enrolls[i][5] == "active") {
            result.push_back(enrolls[i]);
        }
    }
    return result;
}