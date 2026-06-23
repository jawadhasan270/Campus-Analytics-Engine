#ifndef GRADES_H
#define GRADES_H

#include <string>
#include <vector>
using namespace std;

extern const string GRADES_FILE;

struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

// Enter marks for a student in a course
void enterMarks(const string& roll, const string& courseCode, const string& semester);

// Best 3 of 5 quizzes using loop (no sort)
double bestThreeOfFive(double quizzes[], int n);

// Compute weighted total: quiz*0.10 + asgn*0.10 + mid*0.30 + final*0.50
double computeWeightedTotal(double quiz, double asgn, double mid, double finalMark);

// Map numeric grade to letter grade
string getLetterGrade(double total);

// Credit-weighted GPA across semester
double computeGPA(const string& roll, const string& semester);

// Compute class stats for a course
Stats computeClassStats(const string& courseCode);

// Apply attendance penalty: if attendance < 75%, force grade to F
void applyAttendancePenalty(const string& roll, const string& courseCode);

// Print grade record
void printGrade(const vector<string>& g);

#endif