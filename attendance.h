#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <stdio.h>

#define Max_records 5000
#define Max_absences 5
#define MAX_STUDENTS 10   // Unified from Max_students / MAX_STUDENTS
#define Name_len 50
#define Date_len 11
#define Present 'P'
#define Late 'L'
#define Absent 'A'

typedef struct {
    int id;
    char name[Name_len];
} Student;

typedef struct {
    int studentId;
    char date[Date_len];
    char status;
} Record;

typedef struct {
    Student students[MAX_STUDENTS];
    int studentCount;
    Record records[Max_records];
    int recordCount;
} Database;

/* Function Prototypes */
int isDuplicateID(Student students[], int count, int id);
void addStudent(Student students[], int count, Student *s);
void displayStudents(Student students[], int count);
int loadStudents(Student students[]);
void saveStudents(Student students[], int count);
void removeStudent(Student students[], int *count);
void editStudent(Student students[], int count);
void Student_summary(Database *db, int droppedCount);
void search_students(FILE *file);
void dropExcessAbsences(Database *db);
void recordAttendance(Database *db);
void Student_summary(Database *db, int droppedCount);
void search_students(FILE *file);
void displayAttendanceHistory(const Database *db);

#endif // ATTENDANCE_H
