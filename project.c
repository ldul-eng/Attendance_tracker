#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define Max_records 5000
#define Max_absences 5
#define Max_students 10
#define Name_len 50
#define Date_len 11
#define Present 'P'
#define Late 'L'
#define Absent 'A'

typedef struct{
	int id;
	char name[Name_len];
} Student;

typedef struct{
	int studentId;
	char date[Date_len];
	char status;
} Record;
typedef struct{
	Student students[Max_students];
	int studentCount;
	Record records[Max_records];
	int recordCount;
} Database;

static int askYesNo(const char *prompt);

static void computeStats(const Database *db, int idx, int *present, int*late, int *absent);

/* statusName
 * Turns a status letter into a word for display.
 * Input:  s - 'P', 'L', or 'A'
 * Returns: "Present", "Late", "Absent", or "?" if the letter is unknown.
 */
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

/* removeStudentAt
 * Deletes the student at position idx from the roster AND deletes all of
 * that student's attendance records. Records are compacted toward the
 * front of the array; students after idx are shifted left by one.
 * Helper only, called by dropExcessAbsences.
 */
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

/* dropExcessAbsences
 * Lists every student with Max_absences or more absences (only 'A' counts,
 * not 'L'). If there are any, asks for confirmation (y/n) and, if yes,
 * removes them and their records. The removal loop runs BACKWARDS so that
 * shifting students left never causes one to be skipped.
 */
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

/* readLine
 * Reads one line from the keyboard using fgets and removes the '\n'.
 * If the line was longer than the buffer, the extra characters are
 * discarded so they don't leak into the next prompt.
 * Input:  buf - where the text is stored, size - size of buf
 * Output: buf holds the line, or "" if input ended (EOF).
 */
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

/* askYesNo
 * Displays a prompt and keeps asking until the user enters y or n
 * (upper or lower case, exactly one character).
 * Input:  prompt - the question to display
 * Returns: 1 for yes, 0 for no.
 */
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

/* readStatus
 * Keeps asking until the user enters P, L, A, or Q (upper or lower case).
 * Returns: 'P', 'L', 'A', or 'Q' (Q means stop entering attendance).
 */
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

/* findRecord
 * Searches the records for one matching a student ID and a date.
 * Input:  db - the database, studentId - ID to look for, date - "YYYY-MM-DD"
 * Returns: the index of the matching record, or -1 if none exists.
 */
static int findRecord(const Database *db, int studentId, const char *date) {
    for (int i = 0; i < db->recordCount; i++) {
        if (db->records[i].studentId == studentId &&
            strcmp(db->records[i].date, date) == 0) {
            return i;
        }
    }
    return -1;
}

/* ---------- Dates ---------- */

/* isLeap
 * Checks whether a year is a leap year: divisible by 4, except century
 * years, which must be divisible by 400 (1900 is not leap, 2000 is).
 * Returns: 1 if leap, 0 if not.
 */
static int isLeap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* Valid only if it is exactly YYYY-MM-DD and a real calendar date. */
/* isValidDate
 * Checks that a string is exactly YYYY-MM-DD and is a real calendar date
 * (month 1-12, day within that month, February 29 only in leap years).
 * Returns: 1 if valid, 0 if not.
 */
static int isValidDate(const char *date) {
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

/* Keeps asking until the user types a valid date; result goes in 'date' (needs Date_len chars). */
/* readDate
 * Keeps asking until the user enters a valid date, then copies it into
 * the caller's string. Reads into a larger temporary buffer first so that
 * extra characters (like "2024-02-29abc") are rejected, not cut off.
 * Output: date - must have room for Date_len characters.
 */
static void readDate(char *date) {
    char buf[64];                            /* bigger than Date_len on purpose */
    while (1) {
        printf("Date (YYYY-MM-DD): ");
        readLine(buf, sizeof buf);
        if (isValidDate(buf)) {
            strcpy(date, buf);               /* safe: isValidDate guarantees length 10 */
            return;
        }
        printf("Invalid date. Example: 2024-02-29\n");
    }
}

/* ---------- Record attendance ---------- */

/* recordAttendance
 * Asks for a date, then asks for each student's status in roster order.
 * If a record already exists for that student and date, it shows the old
 * status as [now: ...] and updates it; otherwise it adds a new record.
 * Entering Q stops early and keeps everything entered so far.
 * Stops with a message if the record array is full (Max_records).
 */
static void recordAttendance(Database *db) {
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

/* computeStats
 * Counts one student's Present, Late, and Absent records.
 * Input:  db - the database, idx - the student's position in the roster
 * Output: *present, *late, *absent - the counts, returned through pointers
 *         (set to 0 first, then added up).
 */
static void computeStats(const Database *db, int idx, int *present, int *late, int *absent){
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
