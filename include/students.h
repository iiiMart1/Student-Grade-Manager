#ifndef STUDENTS_H
#define STUDENTS_H

typedef struct {
    int TD;
    int TP;
    int EXAM;
} Opsys;

typedef struct {
    int id;
    char name[30];
    Opsys opsys;
} Student;

// Structure to pass data to threads
typedef struct {
    Student *students;
    int start;
    int end;
} ThreadData;


// Function declarations for sorting algorithms
void merge_sort_by_average(Student students[], int left, int right);
void bitonic_sort_by_name(Student students[], int low, int cnt, int dir);
void findStudentByName_threaded(Student students[], int count, int num_threads);
float calculate_student_average(Student student);

// Function declarations for main menu operations
void display_menu();
void add_student(Student students[], int *count);
void display_students(Student students[], int count);
void calculate_averages(Student students[], int count);
void sort_by_average(Student students[], int count);
void sort_by_name(Student students[], int count);
void test_sorting_functions(Student students[], int count);
void display_students_with_averages(Student students[], int count);
void create_test_data(Student students[], int *count, int num_students);
void findStudentByName(Student students[], int count);
void sort_by_average_thread(Student students[], int count);

// Thread function declaration
void calculate_averages_pthread(Student students[], int count);
void merge_sort_pthread(Student students[], int count);
void random_name(char *name, int min_len, int max_len);

#endif

