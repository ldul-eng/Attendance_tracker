#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_STUDENTS 10
#define Max_records 5000
#define Max_absences 5
#define Max_students 10
#define Name_len 50
#define Date_len 11
#define Present 'P'
#define Late 'L'
#define Absent 'A'

typedef struct
{
    char name[Name_len];
    int id;
} Student;

typedef struct{
    int studentId;
    char date[Date_len];
    char status;
} Record;
typedef struct{
    Student students[MAX_STUDENTS];
    int studentCount;
    Record records[Max_records];
    int recordCount;
} Database;

//functions-- prototypes
static int askYesNo(const char *prompt);
static void computeStats(const Database *db, int idx, int *present, int*late, int *absent);
static const char *statusName(char s);
static void removeStudentAt(Database *db, int idx);
static void dropExcessAbsences(Database *db);
static void readLine(char *buf, int size);
static char readStatus(void);
static int findRecord(const Database *db, int studentId, const char *date);
static int isLeap(int year);
static int isValidDate(const char *date);
static void readDate(char *date);
static void recordAttendance(Database *db);
int isDuplicateID(Student students[], int count, int id);
void addStudent(Student students[], int count, Student *s);
void displayStudents(Student students[], int count);
int loadStudents(Student students[]);
void saveStudents(Student students[], int count);
void removeStudent(Student students[], int *count);
void editStudent(Student students[], int count);
void Student_summary(Database *db, int droppedCount);
void search_students(FILE *file);


int main(void) {
// import csv file
    FILE *file;
    file = fopen("Attendance_tracker.csv","r");
    if (file == NULL){
        printf("\nError\n");
        return 1;
    }
    char line[1024];
    char name[100];
    char id[32];
    
    fgets(line,sizeof(line),file);//skips first row (header)
    
    printf("%-25s %s\n","Name","ID");
    printf("-----------------------------------\n");
    
    while(fgets(line,sizeof(line),file)!=NULL){
        if(sscanf(line,",%99[^,],%31[^,]",name,id)==2){
            printf("%-25s %s\n",name,id);
        }
    }
    fclose(file);
    printf("\n");
    
    Student students[MAX_STUDENTS];
    int studentCount = loadStudents(students);
    int choice = 0;
    Database db = {0};
    db.studentCount = studentCount;
    int droppedCount = 0;

    printf("DEBUG: loaded %d students\n", studentCount);
    displayStudents(students, studentCount);
        do
    {
    printf("\n------Menu------\n");
    printf("1. Display Students\n");
    printf("2. Add Student\n");
    printf("3. Remove Student\n");
    printf("4. Edit Student\n");
    printf("5. Attendance Summary\n");
    printf("6. Save and exit.\n");
    printf("Enter your choice:\n");
    scanf("%d", &choice);
    switch(choice)
        {
        case 1:
        printf("debug: studentCount = %d\n", studentCount);
        displayStudents(students, studentCount);
                
        break;
                    
        case 2:
        if(studentCount < MAX_STUDENTS)
        {
        addStudent(students, studentCount, &students[studentCount]);
        studentCount++;
        }
        else
        {
        printf("roster is FULL.\n");
        }

        break;
                
        case 3:
        removeStudent(students, &studentCount);
        printf("after removal student count = %d\n", studentCount);
        displayStudents(students, studentCount);
            
        break;
                
        case 4:
        editStudent(students, studentCount);
                
        break;
        case 5:
        db.studentCount = studentCount;
                for (int i = 0; i < studentCount; i++)
                db.students[i] = students[i];
                Student_summary(&db, droppedCount);
        break;

        case 6:
        printf("Saving %d students...\n", studentCount);

        saveStudents(students, studentCount);
        printf("roster is saved.\n");

        break;

        default:
        printf("invalid choice.\n");
        }
        }

    while(choice !=6);
                    
    

return 0;

    
// call functions
    search_students(file);
    return 0;
}
// functions
static int askYesNo(const char *prompt){
    char buf[16];
    while (1){
        printf("%s (y/n): ", prompt);
        readLine(buf, sizeof buf);
        if(buf[0] !='\0' && buf[1] == '\0'){
            char c = toupper((unsigned char)buf[0]);
            if(c=='Y') return 1;
            if(c=='N') return 0;
        }
        printf("Please enter y or n.\n");
    }
}
static void computeStats(const Database *db, int idx, int *present, int*late, int *absent){
    *present = 0;
    *late = 0;
    *absent = 0;
    int id = db->students[idx].id;

    for(int i=0; i < db->recordCount; i++){
        if(db->records[i].studentId != id) continue;
        if(db->records[i].status==Present) (*present)++;
        else if(db->records[i].status == Late) (*late)++;
        else if(db->records[i].status == Absent) (*absent)++;
    }
}
static const char *statusName(char s){
    switch(s) {
        case Present:
            return "Present";
        case Late:
            return "Late";
        case Absent:
            return "Absent";
        default:
            return "?";
    }
}
static void removeStudentAt(Database *db, int idx){
    int id = db->students[idx].id;

    int keep = 0;
    for(int i=0; i<db->recordCount; i++){
        if (db->records[i].studentId != id){
            db->records[keep] = db->records[i];
            keep++;
        }
    }
    db->recordCount=keep;
    for(int i=idx; i < db->studentCount - 1; i++){
        db->students[i] = db->students[i+1];
        }
    db->studentCount--;
}
static void dropExcessAbsences(Database *db){
    int p, l, a;
    int found = 0;
    
    printf("Students with more than %d absences:\n", Max_absences);
    for(int i = 0; i < db->studentCount; i++){
        computeStats(db, i, &p, &l, &a);
        if(a>Max_absences){
            printf(" %d %s (%d absences)\n", db->students[i].id, db->students[i].name, a);
            found++;
        }
    }

    if(found==0){
        printf(" None.\n");
        return;
    }

    if(!askYesNo("Drop these students and delete their records?")){
        printf("No changes made.\n");
        return;
    }

    for(int i = db->studentCount - 1; i>=0; i--){
        computeStats(db, i, &p, &l, &a);
        if(a>Max_absences){
            removeStudentAt(db, i);
        }
    }
    printf("Dropped %d student(s).\n", found);
}
static void readLine(char *buf, int size){
    if(fgets(buf, size, stdin) == NULL){
    buf[0]='\0';
    return;
}
int len = strlen(buf);
if(len>0 && buf[len-1]=='\n'){
    buf[len-1]='\0';
} else{
    int c;
    while((c=getchar())!='\n' && c != EOF){}
}
}
static char readStatus(void){
    char buf[16];
    while(1){
        printf("Status (P/L/A, Q to stop): ");
        readLine(buf, sizeof buf);
        if(buf[0] != '\0' && buf[1] == '\0'){
            char c = toupper((unsigned char)buf[0]);
            if(c == Present || c == Late || c == Absent || c == 'Q') return c;
        }
        printf("Invalid. Enter P, L, A, or Q.\n");
    }
}
static int findRecord(const Database *db, int studentId, const char *date){
    for (int i = 0; i < db->recordCount; i++) {
        if (db->records[i].studentId == studentId &&
            strcmp(db->records[i].date, date) == 0) {
            return i;
        }
    }
    return -1;
}
static int isLeap(int year){
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
static int isValidDate(const char *date){
    if (strlen(date) != 10) return 0;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') return 0;
        } else {
            if (!isdigit((unsigned char)date[i])) return 0;
        }
}
    int year  = (date[0]-'0')*1000 + (date[1]-'0')*100 + (date[2]-'0')*10 + (date[3]-'0');
    int month = (date[5]-'0')*10 + (date[6]-'0');
    int day   = (date[8]-'0')*10 + (date[9]-'0');

    if (month < 1 || month > 12) return 0;

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDay = daysInMonth[month - 1];
    if (month == 2 && isLeap(year)) maxDay = 29;

    return day >= 1 && day <= maxDay;
}
static void readDate(char *date){
    char buf[64];
    while (1) {
        printf("Date (YYYY-MM-DD): ");
        readLine(buf, sizeof buf);
        if (isValidDate(buf)) {
            strcpy(date, buf);
            return;
        }
        printf("Invalid date. Example: 2024-02-29\n");
    }
}
static void recordAttendance(Database *db){
    if (db->studentCount == 0) {
        printf("No students on the roster.\n");
        return;
    }

    char date[Date_len];
    readDate(date);

    for (int i = 0; i < db->studentCount; i++) {
        int rec = findRecord(db, db->students[i].id, date);

        printf("\n%d  %s", db->students[i].id, db->students[i].name);
        if (rec != -1) {
            printf("  [now: %s]", statusName(db->records[rec].status));
        }
        printf("\n");

        char status = readStatus();
        if (status == 'Q') break;            /* stop, keep what's already entered */

        if (rec != -1) {                     /* update existing record */
            db->records[rec].status = status;
        } else {                             /* append a new one */
            if (db->recordCount >= Max_records) {
                printf("Record storage is full.\n");
                return;
            }
            Record *r = &db->records[db->recordCount];
            r->studentId = db->students[i].id;
            strcpy(r->date, date);
            r->status = status;
            db->recordCount++;
        }
    }
}
int isDuplicateID(Student students[], int count, int id){
    int i;
    
    for(i = 0; i < count; i++)
    {    if(students[i].id == id)
    {
        return 1;
        }
    }
    return 0 ;
}
void addStudent(Student students[], int count, Student *s){
    int i;

    printf("Enter Student Name: \n");
    while(getchar() != '\n');
    fgets(s->name, sizeof(s->name), stdin);

    do
{
    printf("Enter a 5-digit Student ID:\n");
    if (scanf("%d", &s->id) != 1)
    {
    printf("invalid input.\n");
    return;
    }
        if (s->id < 10000 || s->id > 99999)
    {
        printf("Error ID must be exactly 5 digits.\n");
    }
    else if (isDuplicateID(students, count, s->id))
        {
            printf("Error: ID already exists.\n");
        }

        printf("ID entered is: %d", s->id);
    printf("\ncount = %d\n", count);
    printf("duplicate = %d\n", isDuplicateID(students, count, s->id));
}   while (s->id < 10000 || s->id > 99999 || isDuplicateID(students, count,s->id));
}
void displayStudents(Student students[], int count){
    int i;

    printf("\n------Student Roster------\n");
    
    for (i = 0; i < count; i++)
    {
    printf("Name: %s  ", students[i].name);
    printf("  ID: %d\n", students[i].id);
    }
}
int loadStudents(Student students[]){
    FILE *fp;
    int count = 0;
    char header[100];

    fp = fopen("Attendance_tracker.csv", "r");

    if (fp == NULL)
    {
        printf("Could not open file.\n");
        return 0;
    }

    fgets(header, sizeof(header), fp); /* skip header */

    while (count < MAX_STUDENTS && fscanf(fp, " %49[^,],%d", students[count].name, &students[count].id) ==2)

    {
     printf("loaded: %s %d\n", students[count].name, students[count].id);


        count++;
    }

    fclose(fp);

    return count;

}
void saveStudents(Student students[], int count){
    FILE *fp;
    int i;

    fp = fopen("Attendance_tracker.csv", "w");

    if(fp == NULL)
    {
        printf("error saving file.\n");
    return ;
    }

    fprintf(fp, "name\n,ID\n");

    for(i = 0; i < count; i++)
    {
        fprintf(fp, "%s,%d\n", students[i].name, students[i].id);
    }

    fclose(fp);

    printf("roster saved successfully.\n");
}
void removeStudent(Student students[], int *count){
    int id;
    int i,j;

    printf("enter ID of student to remove: \n");
    scanf("%d", &id);

    for(i = 0; i < *count; i++)
    {
        if (students[i].id == id)
        {
            
            for(j = i; j < *count - 1; j++)
        {    students[j] = students[j + 1];
        }
        (*count)--;

        printf("student removed.\n");
            return;
    }
    }
    printf("student not found.\n");

}
void editStudent(Student students[], int count){
    int id;
    int i;
    int newID;
    int choice;
   
    printf("enter ID of student to edit: ");
    scanf("%d", &id);

    for(i = 0; i < count; i++)
    {
        if(students[i].id == id)
        {
            printf("\nCurrent Information:\n");
            printf("Name: %s\n", students[i].name);
            printf("ID: %d\n", students[i].id);

            printf("\n1. Edit Name\n");
            printf("2. Edit ID\n");
            printf("3. Edit Both\n");
            printf("Choice: ");
            scanf("%d", &choice);

            if(choice == 1 || choice == 3)
            {
                while(getchar() != '\n');

                printf("Enter new name: ");
                fgets(students[i].name,sizeof(students[i].name),stdin);

                students[i].name[strcspn(students[i].name, "\n")] = '\0';
            }

            if(choice == 2 || choice == 3)
            {
                do
                {
                    printf("Enter new 5-digit ID: ");
                    scanf("%d", &newID);

                    if(newID < 10000 || newID > 99999)
                    {
                        printf("Error: ID must be 5 digits.\n");
                    }
                    else if(newID != id && isDuplicateID(students, count, newID))
                    {
                        printf("Error: ID already exists.\n");
                    }

                } while(newID < 10000 || newID > 99999 || isDuplicateID(students, count, (newID)));

                students[i].id = newID;
            }

            printf("Student updated successfully.\n");
            return;
        }
    }

    printf("Student not found.\n");

}
void search_students(FILE *file){
    char line[1024];
    char name[100];
    char id[32];
    char search[100];

    printf("\nEnter Student Name or ID number: ");
    if(fgets(search,sizeof(search),stdin)== NULL){
        return;
    }
   // search[strcspn(search,'\r\n')] = '\0';
    if(search[0] == '\0'){
        printf("\nInvalid input, please try again\n");
        return;
    }
    if(fgets(line,sizeof(line),file) ==NULL){
        printf("\n--Error--\n");
        return;
    }
    while(fgets(line,sizeof(line),file)!= NULL){
        if(sscanf(line, ",%99[^,],%31[^,\r\n]",name,id) == 2){
            if(strcmp(search,name) == 0 || strcmp(search,id) == 0){
                printf("\nStudent:\n");
                printf("Name: %s\n",name);
                printf("ID: %s\n",id);
                return;
            }
        }
    }
    printf("\nError. Student not found\n");
}
void Student_summary(Database *db, int droppedCount)
{
    int totalPresent = 0;
    int totalRecords = 0;
    
    int bestPresent = 0;
    int bestTotal = 0;
    int bestID = 0;
    char bestName[Name_len] = "None";
    
    for (int i = 0; i < db->studentCount; i++)
    {
        int present = 0;
        int total = 0;
        
        for (int j = 0; j < db->recordCount; j++)
        {
            if (db->records[j].studentId == db->students[i].id)
            {
                total++;
                
                if (db->records[j].status == Present ||
                    db->records[j].status == Late)
                {
                    present++;
                }
            }
        }
        
        totalPresent += present;
        totalRecords += total;
        
        if (total > 0 &&
            (bestTotal == 0 ||
             (double)present / total >
             (double)bestPresent / bestTotal))
        {
            bestPresent = present;
            bestTotal = total;
            
            bestID = db->students[i].id;
            strcpy(bestName, db->students[i].name);
        }
    }
    
    printf("\n--- Student Summary ---\n");
    
    if (totalRecords > 0)
    {
        printf("Average Attendance: %.2f%%\n",
               100.0 * totalPresent / totalRecords);
    }
    else
    {
        printf("Average Attendance: No records\n");
    }
    
    if (bestTotal > 0)
    {
        printf("Best Attendance: %s (ID: %d) - %.2f%%\n",
               bestName, bestID,
               100.0 * bestPresent / bestTotal);
    }
    else
    {
        printf("Best Attendance: No records\n");
    }
    
    printf("Students Dropped: %d\n", droppedCount);
}
