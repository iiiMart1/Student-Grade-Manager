#ifndef STUDENTS_H
#define STUDENTS_H

typedef struct
{
    int TD;
    int TP;
    int EXAM;
} Opsys;

typedef struct
{
    int id;
    char name[30];
    Opsys opsys;
} Student;

// Structure to pass data to threads
typedef struct
{
    Student *students;
    int start;
    int end;
} ThreadData;
typedef struct
{
    Student *students;
    int left;
    int right;
} MergeSortData;
typedef struct
{
    Student *students;
    int left, right;
} QuickSortData;
typedef struct
{
    Student *students;
    int start;
    int end;
    const char *target_name;
    int found_index;
    int *found_flag;
} SearchThreadData;

// Function declarations for sorting algorithms
void merge_sort_by_average(Student students[], int left, int right);
float calculate_student_average(Student student);
void quick_sort_by_average(Student students[], int left, int right);

// Function declarations for main menu operations
void display_menu();
void add_student(Student students[], int *count);
void display_students(Student students[], int count);
void calculate_averages(Student students[], int count);
void sort_by_average_msort(Student students[], int count);
void sort_by_name(Student students[], int count);
void test_sorting_functions(Student students[], int count);
void display_students_with_averages(Student students[], int count);
void create_test_data(Student students[], int *count, int num_students);
void findStudentByName(Student students[], int count);
void sort_by_average_threaded_msort(Student students[], int count);
void sort_by_average_threaded_qsort(Student students[], int count);
void sort_by_average_qsort(Student students[], int count);
void sort_by_name_mt(Student students[], int count);
float class_average(Student students[], int count);
void top_n_students(Student students[], int count,int n);
void bottom_n_students(Student students[], int count, int n);
void grade_distribution(Student students[], int count);
// Thread function declaration
void calculate_averages_pthread(Student students[], int count);
void random_name(char *name, int min_len, int max_len);
void merge_sort_by_average_mt(Student students[], int left, int right);
void quick_sort_by_average_mt(Student students[], int left, int right);
void quick_sort_by_name_mt(Student students[], int left, int right);
void findStudentByName_mt(Student students[], int count);

#endif
