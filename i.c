#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COURSES 10
#define MAX_EMPLOYEES 50

char admin[] = "admin";
int adminpass = 123;

/* Screen and utility functions */
void clearScreen();
void pauseSystem();

/* File handling functions */
void loadCourses();
void saveCourses();
int checkEmployeeLogin(char *name, char *pass);

/* Employee management */
void addNewEmployee();
void viewEmployeeList();
void removeEmployee();

/* Course management */
void addNewCourse();
void viewAllCourse();
void deleteACourse();

/* Student management */
void EnrollStudent();
void viewWaitlistedStudent();

/* Interface functions */
void mainInterface();
void adminInterface();
void employeeInterface();
void guestInterface();
// process Waitlist
void processWaitlist();

typedef struct Employee
{
    char name[20];
    char id[15];
    char pass[8];
    char course[25];
    char dept[20];
} Employee;

typedef struct Student
{
    char name[20];
    char id[15];
    char dept[20];
    char course[25];
} Student;

typedef struct Course
{
    char name[25];
    char courseFaculty[25];
    int maxEnrollment;
    int currentEnrollment;
} Course;

typedef struct WaitlistedStudent
{
    char name[20];
    char id[15];
    char course[50];
    char dept[20];
} WaitlistedStudent;

Course courses[MAX_COURSES];
int courseCount = 0;

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseSystem()
{
    printf("\nPress Enter to continue...");
    while (getchar() != '\n')
        ;
    getchar();
}

int checkEmployeeLogin(char *name, char *pass)
{
    FILE *file = fopen("login_info.txt", "r");
    if (file == NULL)
        return 0;

    char file_name[20], file_pass[8];
    while (fscanf(file, "%s %s", file_name, file_pass) == 2)
    {
        if (strcmp(name, file_name) == 0 && strcmp(pass, file_pass) == 0)
        {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void loadCourses()
{
    FILE *file = fopen("courses.dat", "rb");
    if (file)
    {
        courseCount = fread(courses, sizeof(Course), MAX_COURSES, file);
        fclose(file);
    }
}

void saveCourses()
{
    FILE *file = fopen("courses.dat", "wb");
    if (file)
    {
        fwrite(courses, sizeof(Course), courseCount, file);
        fclose(file);
    }
}

void addNewEmployee()
{
    Employee e1;
    printf("\nEnter Name: ");
    scanf("%19s", e1.name);
    printf("Enter ID: ");
    scanf("%14s", e1.id);
    printf("Enter Password: ");
    scanf("%7s", e1.pass);
    printf("Enter Course: ");
    scanf("%24s", e1.course);
    printf("Enter Department: ");
    scanf("%19s", e1.dept);

    FILE *file = fopen("employees.txt", "a");
    if (file)
    {
        fprintf(file, "%s %s %s %s\n", e1.name, e1.id, e1.course, e1.dept);
        fclose(file);
    }

    FILE *loginFile = fopen("login_info.txt", "a");
    if (loginFile)
    {
        fprintf(loginFile, "%s %s\n", e1.name, e1.pass);
        fclose(loginFile);
    }

    printf("\nEmployee added successfully!");
}

void viewEmployeeList()
{
    FILE *file = fopen("employees.txt", "r");
    if (file == NULL)
    {
        printf("\nNo employees found!");
        return;
    }

    printf("\n|--------------------- Employee List ---------------------|\n");
    printf("| %-20s | %-15s | %-15s |\n", "Name", "ID", "Department");
    printf("|------------------------------------------------------|\n");

    Employee e1;
    while (fscanf(file, "%s %s %s %s", e1.name, e1.id, e1.course, e1.dept) == 4)
    {
        printf("| %-20s | %-15s | %-15s |\n", e1.name, e1.id, e1.dept);
    }
    printf("|------------------------------------------------------|\n");
    fclose(file);
}

void removeEmployee()
{
    char empID[20];
    printf("\nEnter Employee ID to remove: ");
    scanf("%19s", empID);

    FILE *file = fopen("employees.txt", "r");
    if (file == NULL)
    {
        printf("\nNo employees found!");
        return;
    }

    FILE *tempFile = fopen("temp.txt", "w");
    if (tempFile == NULL)
    {
        fclose(file);
        return;
    }

    Employee e1;
    int found = 0;
    while (fscanf(file, "%s %s %s %s", e1.name, e1.id, e1.course, e1.dept) == 4)
    {
        if (strcmp(e1.id, empID) != 0)
        {
            fprintf(tempFile, "%s %s %s %s\n", e1.name, e1.id, e1.course, e1.dept);
        }
        else
        {
            found = 1;
        }
    }

    fclose(file);
    fclose(tempFile);

    if (found)
    {
        remove("employees.txt");
        rename("temp.txt", "employees.txt");
        printf("\nEmployee removed successfully!");
    }
    else
    {
        remove("temp.txt");
        printf("\nEmployee not found!");
    }
}

void addNewCourse()
{
    if (courseCount >= MAX_COURSES)
    {
        printf("\nMaximum courses reached!");
        return;
    }

    printf("\nEnter course name: ");
    scanf("%24s", courses[courseCount].name);
    printf("Enter faculty name: ");
    scanf("%24s", courses[courseCount].courseFaculty);
    printf("Enter maximum enrollment: ");
    scanf("%d", &courses[courseCount].maxEnrollment);
    courses[courseCount].currentEnrollment = 0;
    courseCount++;
    saveCourses();
    printf("\nCourse added successfully!");
}

void viewAllCourse()
{
    printf("\n|------------------ Course List ------------------|\n");
    printf("| %-20s | %-15s | %-6s |\n", "Course Name", "Faculty", "Slots");
    printf("|---------------------------------------------|\n");

    for (int i = 0; i < courseCount; i++)
    {
        printf("| %-20s | %-15s | %3d/%-3d |\n", courses[i].name, courses[i].courseFaculty, courses[i].currentEnrollment, courses[i].maxEnrollment);
    }
    printf("|---------------------------------------------|\n");
}

void deleteACourse()
{
    char courseName[25];
    printf("\nEnter course name to delete: ");
    scanf("%24s", courseName);

    int found = 0;
    for (int i = 0; i < courseCount; i++)
    {
        if (strcmp(courses[i].name, courseName) == 0)
        {
            for (int j = i; j < courseCount - 1; j++)
            {
                courses[j] = courses[j + 1];
            }
            courseCount--;
            found = 1;
            break;
        }
    }

    if (found)
    {
        saveCourses();
        printf("\nCourse deleted successfully!");
    }
    else
    {
        printf("\nCourse not found!");
    }
}

void EnrollStudent()
{
    Student s;
    printf("\nEnter student name: ");
    scanf("%19s", s.name);
    printf("Enter student ID: ");
    scanf("%14s", s.id);
    printf("Enter department: ");
    scanf("%19s", s.dept);

    viewAllCourse();
    printf("\nEnter course to enroll in: ");
    scanf("%24s", s.course);

    for (int i = 0; i < courseCount; i++)
    {
        if (strcmp(courses[i].name, s.course) == 0)
        {
            if (courses[i].currentEnrollment < courses[i].maxEnrollment)
            {
                FILE *file = fopen("students.txt", "a");
                if (file)
                {
                    fprintf(file, "%s %s %s %s\n", s.name, s.id, s.dept, s.course);
                    fclose(file);
                }
                courses[i].currentEnrollment++;
                saveCourses();
                printf("\nEnrollment successful!");
                return;
            }
            else
            {
                FILE *file = fopen("waitlist.txt", "a");
                if (file)
                {
                    fprintf(file, "%s %s %s %s\n", s.name, s.id, s.dept, s.course);
                    fclose(file);
                }
                printf("\nCourse full! Added to waitlist.");
                return;
            }
        }
    }
    printf("\nCourse not found!");
}

void viewWaitlistedStudent()
{
    FILE *file = fopen("waitlist.txt", "r");
    if (file == NULL)
    {
        printf("\nNo waitlisted students found!");
        return;
    }

    printf("\n|---------------- Waitlisted Students ----------------|\n");
    printf("| %-20s | %-15s | %-15s | %-15s |\n", "Name", "ID", "Department", "Course");
    printf("|-----------------------------------------------------|\n");

    WaitlistedStudent w;
    while (fscanf(file, "%s %s %s %s", w.name, w.id, w.dept, w.course) == 4)
    {
        printf("| %-20s | %-15s | %-15s | %-15s |\n", w.name, w.id, w.dept, w.course);
    }
    printf("|-----------------------------------------------------|\n");
    fclose(file);
}
void processWaitlist()
{
    FILE *waitlist = fopen("waitlist.txt", "r");
    if (!waitlist)
    {
        printf("\nNo waitlisted students found!");
        return;
    }

    FILE *tempWaitlist = fopen("temp_waitlist.txt", "w");
    if (!tempWaitlist)
    {
        fclose(waitlist);
        return;
    }

    WaitlistedStudent w;
    int processed = 0;

    while (fscanf(waitlist, "%s %s %s %s", w.name, w.id, w.dept, w.course) == 4)
    {
        int enrolled = 0;

        for (int i = 0; i < courseCount; i++)
        {
            if (strcmp(courses[i].name, w.course) == 0 &&
                courses[i].currentEnrollment < courses[i].maxEnrollment)
            {

                FILE *studentFile = fopen("students.txt", "a");
                if (studentFile)
                {
                    fprintf(studentFile, "%s %s %s %s\n", w.name, w.id, w.dept, w.course);
                    fclose(studentFile);
                }

                courses[i].currentEnrollment++;
                saveCourses();
                enrolled = 1;
                processed++;
                break;
            }
        }

        if (!enrolled)
        {
            fprintf(tempWaitlist, "%s %s %s %s\n", w.name, w.id, w.dept, w.course);
        }
    }

    fclose(waitlist);
    fclose(tempWaitlist);

    remove("waitlist.txt");
    rename("temp_waitlist.txt", "waitlist.txt");

    printf("\nEnrolled %d students from waitlist!", processed);
}
void adminInterface()
{
    int choice;
    do
    {
        clearScreen();
        printf("\n------------------------------------------------");
        printf("\n|############### ADMIN PORTAL #################|");
        printf("\n| 1. Add New Employee                         |");
        printf("\n| 2. Remove Employee                          |");
        printf("\n| 3. View Employee List                       |");
        printf("\n| 4. View All Courses                         |");
        printf("\n| 5. Add New Course                           |");
        printf("\n| 6. Delete Course                            |");
        printf("\n| 7. Enroll Student                           |");
        printf("\n| 8. View Waitlisted Students                 |");
        printf("\n| 9. Process Waitlist                         |");
        printf("\n| 0. Logout                                   |");
        printf("\n------------------------------------------------");
        printf("\nEnter your choice: ");

        scanf("%d", &choice);
        while (getchar() != '\n')
            ;

        switch (choice)
        {
        case 1:
            addNewEmployee();
            break;
        case 2:
            removeEmployee();
            break;
        case 3:
            viewEmployeeList();
            break;
        case 4:
            viewAllCourse();
            break;
        case 5:
            addNewCourse();
            break;
        case 6:
            deleteACourse();
            break;
        case 7:
            EnrollStudent();
            break;
        case 8:
            viewWaitlistedStudent();
            break;
        case 9:
            processWaitlist();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice!");
        }
        pauseSystem();
    } while (1);
}

void employeeInterface()
{
    int choice;
    do
    {
        clearScreen();
        printf("\n-----------------------------------------------");
        printf("\n|############## EMPLOYEE PORTAL ##############|");
        printf("\n| 1. View Courses                            |");
        printf("\n| 2. Enroll Student                          |");
        printf("\n| 3. View Waitlisted Students                |");
        printf("\n| 0. Logout                                  |");
        printf("\n----------------------------------------------");
        printf("\nEnter your choice: ");

        scanf("%d", &choice);
        while (getchar() != '\n')
            ;

        switch (choice)
        {
        case 1:
            viewAllCourse();
            break;
        case 2:
            EnrollStudent();
            break;
        case 3:
            viewWaitlistedStudent();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice!");
        }
        pauseSystem();
    } while (1);
}

void guestInterface()
{
    int choice;
    do
    {
        clearScreen();
        printf("\n--------------------------------------------");
        printf("\n|############### GUEST MODE ###############|");
        printf("\n| 1. View Available Courses               |");
        printf("\n| 2. View Waitlisted Students             |");
        printf("\n| 0. Return to Main Menu                  |");
        printf("\n--------------------------------------------");
        printf("\nEnter your choice: ");

        scanf("%d", &choice);
        while (getchar() != '\n')
            ;

        switch (choice)
        {
        case 1:
            viewAllCourse();
            break;
        case 2:
            viewWaitlistedStudent();
            break;
        case 0:
            return;
        default:
            printf("\nInvalid choice!");
        }
        pauseSystem();
    } while (1);
}

void mainInterface()
{
    int choice;
    while (1)
    {
        clearScreen();
        printf("\n------------------------------------------------");
        printf("\n|######### STUDENT ENROLLMENT SYSTEM #########|");
        printf("\n| Institute: Daffodil International University |");
        printf("\n|---------------------------------------------|");
        printf("\n| 1. Admin                                    |");
        printf("\n| 2. Employee                                 |");
        printf("\n| 3. Guest                                    |");
        printf("\n| 0. Exit                                     |");
        printf("\n-----------------------------------------------");
        printf("\nEnter your choice: ");

        scanf("%d", &choice);
        while (getchar() != '\n')
            ;

        switch (choice)
        {
        case 1:
        {
            char name[20];
            int password;
            clearScreen();
            printf("\n----------- ADMIN LOGIN -----------");
            printf("\nUsername: ");
            scanf("%19[^\n]", name);
            while (getchar() != '\n')
                ;
            printf("Password: ");
            scanf("%d", &password);
            while (getchar() != '\n')
                ;

            if (strcmp(name, admin) == 0 && password == adminpass)
            {
                adminInterface();
            }
            else
            {
                printf("\nInvalid credentials!");
                pauseSystem();
            }
            break;
        }
        case 2:
        {
            char name[20], pass[8];
            clearScreen();
            printf("\n---------- EMPLOYEE LOGIN ----------");
            printf("\nUsername: ");
            scanf("%19s", name);
            printf("Password: ");
            scanf("%7s", pass);
            while (getchar() != '\n')
                ;

            if (checkEmployeeLogin(name, pass))
            {
                employeeInterface();
            }
            else
            {
                printf("\nInvalid credentials!");
                pauseSystem();
            }
            break;
        }
        case 3:
            guestInterface();
            break;
        case 0:
            printf("\nExiting program. Goodbye!\n");
            return;
        default:
            printf("\nInvalid choice!");
            pauseSystem();
        }
    }
}

int main()
{
    loadCourses();
    mainInterface();
    return 0;
}