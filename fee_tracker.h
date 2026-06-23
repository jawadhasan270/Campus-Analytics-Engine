#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <string>
#include <vector>
using namespace std;

extern const string FEES_FILE;

// Record a payment for a student
void recordPayment(const string& roll, const string& semester, double amount, const string& paidDate);

// Compute late fine: 2% per complete week overdue
double computeLateFine(const string& roll, const string& semester);

// Calculate days between two DD-MM-YYYY dates (manual, no ctime)
int daysBetween(const string& date1, const string& date2);

// Print formatted receipt
void generateReceipt(const string& roll, const string& semester);

// Get all fee defaulters (balance > 0 past due date), sorted by outstanding amount
vector<vector<string>> getDefaulters();

// Validate date format DD-MM-YYYY
bool isValidDate(const string& date);

#endif