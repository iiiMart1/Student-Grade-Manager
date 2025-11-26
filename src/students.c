#include "students.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_STUDENTS 10000
sem_t s;

// Helper function to calculate student average
float calculate_student_average(Student student)
{
    return (student.opsys.TD + student.opsys.TP) * 0.2 + student.opsys.EXAM * 0.6;
}

// Merge function for merge sort
void merge_by_average(Student students[], int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Create temporary arrays using dynamic allocation
    Student *L = (Student *)malloc(n1 * sizeof(Student));
    Student *R = (Student *)malloc(n2 * sizeof(Student));

    // Copy data to temporary arrays
    for (i = 0; i < n1; i++)
        L[i] = students[left + i];
    for (j = 0; j < n2; j++)
        R[j] = students[mid + 1 + j];

    // Merge the temporary arrays back
    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2)
    {
        if (calculate_student_average(L[i]) >= calculate_student_average(R[j]))
        {
            students[k] = L[i];
            i++;
        }
        else
        {
            students[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements of L[]
    while (i < n1)
    {
        students[k] = L[i];
        i++;
        k++;
    }

    // Copy remaining elements of R[]
    while (j < n2)
    {
        students[k] = R[j];
        j++;
        k++;
    }

    free(L);
    free(R);
}

// Merge sort implementation for average grades (descending order)
void merge_sort_by_average(Student students[], int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        merge_sort_by_average(students, left, mid);
        merge_sort_by_average(students, mid + 1, right);
        merge_by_average(students, left, mid, right);
    }
}
// Bitonic sort helper functions
void bitonic_compare(Student students[], int i, int j, int dir)
{
    if ((dir == 1 && strcmp(students[i].name, students[j].name) > 0) ||
        (dir == 0 && strcmp(students[i].name, students[j].name) < 0))
    {
        Student temp = students[i];
        students[i] = students[j];
        students[j] = temp;
    }
}

void bitonic_merge(Student students[], int low, int cnt, int dir)
{
    if (cnt > 1)
    {
        int k = cnt / 2;
        for (int i = low; i < low + k; i++)
        {
            bitonic_compare(students, i, i + k, dir);
        }
        bitonic_merge(students, low, k, dir);
        bitonic_merge(students, low + k, k, dir);
    }
}
// Bitonic sort implementation for student names (ascending order)
void bitonic_sort_by_name(Student students[], int low, int cnt, int dir)
{
    if (cnt > 1)
    {
        int k = cnt / 2;
        bitonic_sort_by_name(students, low, k, 1);
        bitonic_sort_by_name(students, low + k, k, 0);
        bitonic_merge(students, low, cnt, dir);
    }
}

// Display the main menu options
void display_menu()
{
    printf("\n=== Student Management Menu ===\n");
        printf("1) Add Student\n");
        printf("2) Display Students\n");
        printf("3) Calculate Averages\n");
        printf("4) Sort Students\n");
        printf("5) Test Sorting Functions\n");
        printf("6) Create Test Data\n");
        printf("7) Find Student by Name\n");
        printf("8) Exit\n");
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
        printf("Grades === TD: %d, TP: %d, EXAM: %d Average: %.2f \n",
               students[i].opsys.TD, students[i].opsys.TP, students[i].opsys.EXAM, calculate_student_average(students[i]));
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
        printf("Student ID: %d\n", students[i].id);
        printf("Student Name: %s\n", students[i].name);
        printf("Average: %.2f\n", calculate_student_average(students[i]));
        printf("--------------\n");
    }
}


// Thread worker function to calculate averages for a range of students
void *calculate_averages_worker(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    // Calculate averages for assigned range
    for (int i = data->start; i < data->end; i++)
    {
                sem_wait(&s);

        float average = (data->students[i].opsys.TD + data->students[i].opsys.TP) * 0.2 + data->students[i].opsys.EXAM * 0.6;

        // Use semaphore to protect shared resources when printing
        printf("Thread: Student %s (ID: %d) - Average: %.2f\n",
               data->students[i].name, data->students[i].id, average);
        sem_post(&s);
    }

    return NULL;
}

void calculate_averages_pthread(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students in the system.\n");
        return;
    }

    printf("\n=== Student Averages (Threaded) ===\n");

    #define NUM_THREADS 10
    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];

    int chunk = count / NUM_THREADS;
    int extra = count % NUM_THREADS; // leftover students

    int start = 0;

    for (int t = 0; t < NUM_THREADS; t++)
    {
        data[t].students = students;
        data[t].start = start;

        // last threads may get 1 extra student
        data[t].end = start + chunk + (t < extra ? 1 : 0);

        start = data[t].end;

        pthread_create(&threads[t], NULL, calculate_averages_worker, &data[t]);
    }

    // wait for all threads
    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);

    printf("Threaded calculation completed!\n");
}

// Sort students by average grade using merge sort
void sort_by_average(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    if (count > 0)
    {
        merge_sort_by_average(students, 0, count - 1);
    }

    printf("\n=== After Sorting by Average (Descending) ===\n");
    display_students(students, count);
}
void sort_by_average_thread(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    printf("\n=== Before Sorting by Average ===\n");
    display_students(students, count);

    if (count > 0)
    {
        merge_sort_pthread(students, count);
    }

    printf("\n=== After Sorting by Average (Descending) ===\n");
    display_students(students, count);
}

// Sort students by name using bitonic sort
void sort_by_name(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }
    if (count > 0)
    {
        bitonic_sort_by_name(students, 0, count, 1); // 1 for ascending order
    }

    printf("\n=== After Sorting by Name (Ascending) ===\n");
    display_students(students, count);
}

// Test sorting functions with various data sets
void test_sorting_functions(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students in the system.\n");
        return;
    }
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
void create_test_data(Student students[], int *count, int num_students)
{
    *count = 0;  // Reset the current count

    for (int i = 0; i < num_students && *count < MAX_STUDENTS; i++)
    {
        // Assign a unique ID
        students[*count].id = i + 1;

        // Generate a random 6-7 letter name
        random_name(students[*count].name, 6, 7);

        // Generate random grades between 7 and 20
        students[*count].opsys.TD = 7 + rand() % 14;
        students[*count].opsys.TP = 7 + rand() % 14;
        students[*count].opsys.EXAM = 7 + rand() % 14;

        (*count)++;  // Increment total students
    }

    printf("Test data created with %d students.\n", *count);
}
void random_name(char *name, int min_len, int max_len)
{
    const char *consonants = "bcdfghjklmnpqrstvwxyz";
    const char *vowels = "aeiou";

    int len = min_len + rand() % (max_len - min_len + 1);

    for (int i = 0; i < len; i++)
    {
        if (i % 2 == 0)
            name[i] = consonants[rand() % strlen(consonants)];
        else
            name[i] = vowels[rand() % strlen(vowels)];
    }

    // Capitalize the first letter
    if (len > 0)
        name[0] = name[0] - 'a' + 'A';

    name[len] = '\0';
}
#ifdef _WIN32 
#include <windows.h>
double get_time_ms()
{
    static LARGE_INTEGER freq;
    static int freq_init = 0;

    if (!freq_init) {
        QueryPerformanceFrequency(&freq);
        freq_init = 1;
    }

    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return (double)counter.QuadPart * 1000.0 / (double)freq.QuadPart;
}

#else
#include <time.h>

double get_time_ms()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}
#endif