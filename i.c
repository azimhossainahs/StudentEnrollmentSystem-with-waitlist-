#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COURSES 10

char admin[] = "admin";
int adminpass = 123;

// Structure definitions
typedef struct Employee
{
    char name[20];
    char id[15];
    char pass[8];
    char course[25];
    char dept[20];
    struct Employee *next;
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

// Global variables
Course courses[MAX_COURSES];
int courseCount = 0;
Employee *employeeList = NULL;

// Function prototypes
void clearScreen();
void pauseSystem();
void loadCourses();
void saveCourses();
void loadEmployeesFromFile();
void saveEmployeesToFile();
void freeEmployeeList();
int checkEmployeeLogin(char *name, char *pass);
void addNewEmployee();
void viewEmployeeList();
void removeEmployee();
void addNewCourse();
void viewAllCourse();
void deleteACourse();
void EnrollStudent();
void viewWaitlistedStudent();
void processWaitlist();
void mainInterface();
void adminInterface();
void employeeInterface();
void guestInterface();

// Utility functions
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

// Course management
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

// Employee management
void loadEmployeesFromFile()
{
    FILE *file = fopen("employees.dat", "rb");
    if (!file)
        return;

    Employee temp, *newEmp, *prev = NULL;
    while (fread(&temp, sizeof(Employee), 1, file))
    {
        newEmp = (Employee *)malloc(sizeof(Employee));
        *newEmp = temp;
        newEmp->next = NULL;

        if (!employeeList)
        {
            employeeList = newEmp;
        }
        else
        {
            prev->next = newEmp;
        }
        prev = newEmp;
    }
    fclose(file);
}

void saveEmployeesToFile()
{
    FILE *file = fopen("employees.dat", "wb");
    if (!file)
        return;

    Employee *current = employeeList;
    while (current)
    {
        fwrite(current, sizeof(Employee), 1, file);
        current = current->next;
    }
    fclose(file);
}

void freeEmployeeList()
{
    Employee *current = employeeList;
    while (current)
    {
        Employee *temp = current;
        current = current->next;
        free(temp);
    }
    employeeList = NULL;
}

int checkEmployeeLogin(char *name, char *pass)
{
    Employee *current = employeeList;
    while (current)
    {
        if (strcmp(current->name, name) == 0 &&
            strcmp(current->pass, pass) == 0)
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

void addNewEmployee()
{
    Employee *newEmp = (Employee *)malloc(sizeof(Employee));
    if (!newEmp)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    printf("\nEnter Name: ");
    scanf("%19s", newEmp->name);
    printf("Enter ID: ");
    scanf("%14s", newEmp->id);
    printf("Enter Password: ");
    scanf("%7s", newEmp->pass);
    printf("Enter Course: ");
    scanf("%24s", newEmp->course);
    printf("Enter Department: ");
    scanf("%19s", newEmp->dept);

    newEmp->next = NULL;

    if (!employeeList)
    {
        employeeList = newEmp;
    }
    else
    {
        Employee *temp = employeeList;
        while (temp->next)
            temp = temp->next;
        temp->next = newEmp;
    }

    saveEmployeesToFile();
    printf("\nEmployee added successfully!");
}

void viewEmployeeList()
{
    if (!employeeList)
    {
        printf("\nNo employees found!");
        return;
    }

    printf("\n|--------------------- Employee List ---------------------|\n");
    printf("| %-20s | %-15s | %-15s |\n", "Name", "ID", "Department");
    printf("|------------------------------------------------------|\n");

    Employee *current = employeeList;
    while (current)
    {
        printf("| %-20s | %-15s | %-15s |\n",
               current->name, current->id, current->dept);
        current = current->next;
    }
    printf("|------------------------------------------------------|\n");
}

void removeEmployee()
{
    char empID[15];
    printf("\nEnter Employee ID to remove: ");
    scanf("%14s", empID);

    Employee *current = employeeList, *prev = NULL;
    while (current)
    {
        if (strcmp(current->id, empID) == 0)
        {
            if (prev)
            {
                prev->next = current->next;
            }
            else
            {
                employeeList = current->next;
            }
            free(current);
            saveEmployeesToFile();
            printf("\nEmployee removed successfully!");
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("\nEmployee not found!");
}

// Course management
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
        printf("| %-20s | %-15s | %3d/%-3d |\n",
               courses[i].name, courses[i].courseFaculty,
               courses[i].currentEnrollment, courses[i].maxEnrollment);
    }
    printf("|---------------------------------------------|\n");
}

void deleteACourse()
{
    char courseName[25];
    printf("\nEnter course name to delete: ");
    scanf("%24s", courseName);

    for (int i = 0; i < courseCount; i++)
    {
        if (strcmp(courses[i].name, courseName) == 0)
        {
            for (int j = i; j < courseCount - 1; j++)
            {
                courses[j] = courses[j + 1];
            }
            courseCount--;
            saveCourses();
            printf("\nCourse deleted successfully!");
            return;
        }
    }
    printf("\nCourse not found!");
}

// Student management
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
    if (!file)
    {
        printf("\nNo waitlisted students found!");
        return;
    }

    printf("\n|---------------- Waitlisted Students ----------------|\n");
    printf("| %-20s | %-15s | %-15s | %-15s |\n",
           "Name", "ID", "Department", "Course");
    printf("|-----------------------------------------------------|\n");

    WaitlistedStudent w;
    while (fscanf(file, "%s %s %s %s", w.name, w.id, w.dept, w.course) == 4)
    {
        printf("| %-20s | %-15s | %-15s | %-15s |\n",
               w.name, w.id, w.dept, w.course);
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
                    fprintf(studentFile, "%s %s %s %s\n",
                            w.name, w.id, w.dept, w.course);
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
            fprintf(tempWaitlist, "%s %s %s %s\n",
                    w.name, w.id, w.dept, w.course);
        }
    }

    fclose(waitlist);
    fclose(tempWaitlist);

    remove("waitlist.txt");
    rename("temp_waitlist.txt", "waitlist.txt");

    printf("\nEnrolled %d students from waitlist!", processed);
}

// Interfaces
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
            scanf("%19s", name);
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
    loadEmployeesFromFile();

    mainInterface();

    saveEmployeesToFile();
    freeEmployeeList();
    return 0;
}