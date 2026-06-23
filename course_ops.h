#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <string>
#include <vector>
using namespace std;

extern const string COURSES_FILE;
extern const string ENROLLMENTS_FILE;

// Enrollment result codes
enum EnrollResult {
    ENROLL_SUCCESS,
    ENROLL_STUDENT_INACTIVE,
    ENROLL_COURSE_NOT_FOUND,
    ENROLL_NO_SEATS,
    ENROLL_ALREADY_ENROLLED,
    ENROLL_CREDIT_OVERLOAD,
    ENROLL_PREREQ_NOT_MET
};

// Enroll a student in a course (full validation)
EnrollResult enrollStudent(const string& roll, const string& courseCode, const string& semester);

// Drop a course (only if no attendance rows exist for this student+course+semester)
bool dropCourse(const string& roll, const string& courseCode, const string& semester);

// Get total credit hours for a student in a semester
int getCreditLoad(const string& roll, const string& semester);

// Check if student has passed the prerequisite course
bool checkPrerequisite(const string& roll, const string& courseCode);

// List all actively enrolled students in a course
vector<vector<string>> listEnrolledStudents(const string& courseCode);

// Print EnrollResult message
void printEnrollResult(EnrollResult r);

// Print a course row
void printCourse(const vector<string>& c);

// Update enrolled count in courses.txt
void updateEnrolledCount(const string& courseCode, int delta);

#endif