#include<stdio.h> 
#include<string.h>
#include<stdlib.h>
#define MAX_STUDENTS 10 


    
//create structure for student
typedef struct 
{
    char name[50];
    int ID;
} Student; 
//prototypes
int isDuplicateID(Student students[], int count, int id);
void addStudent(Student students[], int count, Student *s);
void displayStudents(Student students[], int count);
int loadStudents(Student students[]);
void saveStudents(Student students[], int count);
void removeStudent(Student students[], int *count);
void editStudent(Student students[], int count);


//function to add/create student, name and ID
void addStudent(Student students[], int count, Student *s) 
{
    int i; 

    printf("Enter Student Name: \n");
    while(getchar() != '\n');
    fgets(s->name, sizeof(s->name), stdin);

    do 
{	
    printf("Enter a 5-digit Student ID:\n");
    if (scanf("%d", &s->ID) != 1)
    {
	printf("invalid input.\n");
	return;
    }
        if (s->ID < 10000 || s->ID > 99999)
	{
	    printf("Error ID must be exactly 5 digits.\n");
	}
	else if (isDuplicateID(students, count, s->ID))
        {
            printf("Error: ID already exists.\n");
        }	   

        printf("ID entered is: %d", s->ID);
	printf("\ncount = %d\n", count); 
	printf("duplicate = %d\n", isDuplicateID(students, count, s->ID));
}   while (s->ID < 10000 || s->ID > 99999 || isDuplicateID(students, count,s->ID));
} 
         
//function to display roster of students
void displayStudents(Student students[], int count) 
{
    int i; 

    printf("\n------Student Roster------\n");
    
    for (i = 0; i < count; i++)
    {
	printf("Name: %s  ", students[i].name);
	printf("  ID: %d\n", students[i].ID);
    }
}

//function to check that there are no duplicate IDs entered for students
int isDuplicateID(Student students[], int count, int id)
{
    int i;
    
    for(i = 0; i < count; i++)
    {	if(students[i].ID == id)
	{
	    return 1;
        }
    }
    return 0 ;
}

//access students in the csv file and load them onto display
int loadStudents(Student students[])
{
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

    while (count < MAX_STUDENTS && fscanf(fp, " %49[^,],%d", students[count].name, &students[count].ID) ==2)

    {
     printf("loaded: %s %d\n", students[count].name, students[count].ID);


        count++;
    }

    fclose(fp);

    return count;
}

//function to save and update csv file     
void saveStudents(Student students[], int count)
{
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
        fprintf(fp, "%s,%d\n", students[i].name, students[i].ID);
    }

    fclose(fp);

    printf("roster saved successfully.\n");
}

//function to remove student from roster
void removeStudent(Student students[], int *count)
{
    int id;
    int i,j;

    printf("enter ID of student to remove: \n");
    scanf("%d", &id);

    for(i = 0; i < *count; i++)
    {
        if (students[i].ID == id)
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


void editStudent(Student students[], int count)
{
    int id; 
    int i;
    int newID;
    int choice;
   
    printf("enter ID of student to edit: ");
    scanf("%d", &id);

    for(i = 0; i < count; i++)
    {
        if(students[i].ID == id)
        {
            printf("\nCurrent Information:\n");
            printf("Name: %s\n", students[i].name);
            printf("ID: %d\n", students[i].ID);

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

                students[i].ID = newID;
            }

            printf("Student updated successfully.\n");
            return;
        }
    }

    printf("Student not found.\n");
}	    


int main(void)
{
    Student students[MAX_STUDENTS];
        int studentCount;
	int choice;

	
    studentCount = loadStudents(students);

    printf("DEBUG: loaded %d students\n", studentCount);
    displayStudents(students, studentCount);    
        do
	{
            printf("\n------Menu------\n");
                printf("1. Display Students\n");
                printf("2. Add Student\n");
                printf("3. Remove Student\n");
		printf("4. Edit Student\n");
                printf("5. Save and exit.\n");
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
	            printf("Saving %d students...\n", studentCount);

		    saveStudents(students, studentCount);
		    printf("roster is saved.\n");

                    break;

		    default:
		    printf("invalid choice.\n");
		} 
        }

	while(choice !=5);
                    
	

return 0;

}
