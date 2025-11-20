#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "students.h"

#define MAX_STUDENTS 100

// Function prototypes
void display_menu();
void add_student(Student students[], int *count);
void display_students(Student students[], int count);
void calculate_averages(Student students[], int count);
void sort_by_average(Student students[], int count);
void sort_by_name(Student students[], int count);
void test_sorting_functions(Student students[], int count);
void display_students_with_averages(Student students[], int count);
void create_test_data(Student students[], int *count);

// Display the main menu options
void display_menu()
{
    printf("\n=== Main Menu ===\n");
    printf("1. Add Student\n");
    printf("2. Display All Students\n");
    printf("3. Calculate Averages\n");
    printf("4. Exit\n");
    printf("5. Sort by Average Grade (Merge Sort)\n");
    printf("6. Sort by Name (Bitonic Sort)\n");
    printf("7. Test Sorting Functions\n");
    printf("8. Create Test Data\n");
    printf("8. Find Student by Name\n");
}

// Add a new student to the system
void add_student(Student students[], int *count)
{
    if (*count >= MAX_STUDENTS)
    {
        printf("Error: Maximum student capacity reached!\n");
        return;
    }

    Student *new_student = &students[*count];

    printf("Enter Student ID: ");
    scanf("%d", &new_student->id);
    getchar();

    printf("Enter Student Name: ");
    fgets(new_student->name, 30, stdin);
    new_student->name[strcspn(new_student->name, "\n")] = 0;

    int grade;

    printf("Enter TD grade (0-20): ");
    scanf("%d", &grade);
    if (grade < 0 || grade > 20)
    {
        printf("Error: Grade must be between 0 and 20!\n");
        return;
    }
    new_student->opsys.TD = grade;

    printf("Enter TP grade (0-20): ");
    scanf("%d", &grade);
    if (grade < 0 || grade > 20)
    {
        printf("Error: Grade must be between 0 and 20!\n");
        return;
    }
    new_student->opsys.TP = grade;

    printf("Enter EXAM grade (0-20): ");
    scanf("%d", &grade);
    if (grade < 0 || grade > 20)
    {
        printf("Error: Grade must be between 0 and 20!\n");
        return;
    }
    new_student->opsys.EXAM = grade;

    (*count)++;
    printf("Student added successfully!\n");
}

// Display all students in the system
void display_students(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students in the system.\n");
        return;
    }

    printf("\n=== Student List ===\n");
    for (int i = 0; i < count; i++)
    {
        printf("ID: %d\n", students[i].id);
        printf("Name: %s\n", students[i].name);
        printf("Grades === TD: %d, TP: %d, EXAM: %d\n",
               students[i].opsys.TD, students[i].opsys.TP, students[i].opsys.EXAM);
        printf("-----------------------------------\n");
    }
}

// Calculate and display averages for all students
void calculate_averages(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students in the system.\n");
        return;
    }

    printf("\n=== Student Averages ===\n");
    for (int i = 0; i < count; i++)
    {
        float average = (students[i].opsys.TD + students[i].opsys.TP) * 0.2 + students[i].opsys.EXAM * 0.6;
        printf("Student ID: %d\n", students[i].id);
        printf("Student Name: %s\n", students[i].name);
        printf("Average: %.2f\n", average);
        printf("--------------\n");
    }
}

// Sort students by average grade using merge sort
void sort_by_average(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    printf("\n=== Before Sorting by Average ===\n");
    display_students_with_averages(students, count);

    merge_sort_by_average(students, 0, count - 1);

    printf("\n=== After Sorting by Average (Descending) ===\n");
    display_students_with_averages(students, count);
}

// Sort students by name using bitonic sort
void sort_by_name(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    printf("\n=== Before Sorting by Name ===\n");
    display_students_with_averages(students, count);

    bitonic_sort_by_name(students, 0, count, 1);

    printf("\n=== After Sorting by Name (Ascending) ===\n");
    display_students_with_averages(students, count);
}

// Test sorting functions with various data sets
void test_sorting_functions(Student students[], int count)
{
    printf("\n=== Testing Sorting Functions ===\n");

    // Test with current data
    if (count > 0)
    {
        printf("Testing with current student data (%d students)...\n", count);
        printf("Original order:\n");
        display_students_with_averages(students, count);

        // Test merge sort by average
        printf("\n--- Testing Merge Sort by Average ---\n");
        Student test_students[MAX_STUDENTS];
        memcpy(test_students, students, count * sizeof(Student));

        merge_sort_by_average(test_students, 0, count - 1);
        printf("After merge sort by average:\n");
        display_students_with_averages(test_students, count);

        // Test bitonic sort by name
        printf("\n--- Testing Bitonic Sort by Name ---\n");
        memcpy(test_students, students, count * sizeof(Student));
        bitonic_sort_by_name(test_students, 0, count, 1);
        printf("After bitonic sort by name:\n");
        display_students_with_averages(test_students, count);
    }
}

// Display students with their average grades
void display_students_with_averages(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students in the system.\n");
        return;
    }

    for (int i = 0; i < count; i++)
    {
        float average = calculate_student_average(students[i]);
        printf("ID: %d, Name: %s, Average: %.2f\n", students[i].id, students[i].name, average);
    }
}

// Create test data for sorting tests
void create_test_data(Student students[], int *count)
{
    // Clear existing data
    *count = 0;

    // Test data with various scenarios
    Student test_data[] = {
        {001, "Alice", {15, 16, 14}},
        {002, "Charlie", {12, 13, 11}},
        {003, "Bob", {18, 17, 19}},
        {004, "David", {8, 9, 7}},
        {005, "Eve", {20, 19, 18}}};

    int test_count = sizeof(test_data) / sizeof(test_data[0]);
    for (int i = 0; i < test_count && *count < MAX_STUDENTS; i++)
    {
        students[*count] = test_data[i];
        (*count)++;
    }

    printf("Test data created with %d students.\n", *count);
}

void findStudentByName(Student students[], int count)
{
    char name[50];

    printf("Enter student name: ");
    scanf("%49s", name);

    int index = -1;

    for (int i = 0; i < count; i++)
    {
        if (strcmp(students[i].name, name) == 0)
        {
            index = i;
            break;
        }
    }

    if (index == -1)
    {
        printf("Student not found.\n");
    }
    else
    {
        printf("%s's average = %.2f\n",
               students[index].name,
               calculate_student_average(students[index]));
    }
}

int main()
{
    Student students[MAX_STUDENTS];
    int student_count = 0;
    int choice;

    printf("=== Student Grade Manager ===\n");

    do
    {
        display_menu();
        printf("Enter your choice: ");
        if (scanf("%d", &choice) != 1)
        {
            printf("\nInvalid input. Please enter a number.\n");
            while (getchar() != '\n')
                ; // Clear invalid input
            continue;
        }
        getchar(); // Clear newline from buffer

        switch (choice)
        {
        case 1:
            add_student(students, &student_count);
            break;
        case 2:
            display_students(students, student_count);
            break;
        case 3:
            calculate_averages(students, student_count);
            break;
        case 4:
            printf("Exiting program. Goodbye!\n");
            break;
        case 5:
            sort_by_average(students, student_count);
            break;
        case 6:
            sort_by_name(students, student_count);
            break;
        case 7:
            test_sorting_functions(students, student_count);
            break;
        case 8:
            create_test_data(students, &student_count);
            break;
        case 9:
            findStudentByName(students, student_count);
            break;
        default:
            printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}