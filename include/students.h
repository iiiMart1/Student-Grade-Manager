#ifndef STUDENTS_H
#define STUDENTS_H

/* Simple container for three numeric grades */
typedef struct
{
    int TD;
    int TP;
    int EXAM;
} Opsys;

/* Student record. Keep name length small for this demo; be careful with overflow. */
typedef struct
{
    int id;
    char name[30];   
    Opsys opsys;
} Student;

/* Thread worker data for chunked average calculation */
typedef struct
{
    Student *students;  /* pointer to the shared student array */
    int start;          
    int end;            
} ThreadData;

/* Small helper passed to threaded mergesort */
typedef struct
{
    Student *students;
    int left;
    int right;
} MergeSortData;

/* Small helper passed to threaded quicksort */
typedef struct
{
    Student *students;
    int left, right;
} QuickSortData;

/* Thread data for parallel search (findStudentByName_mt) */
typedef struct
{
    Student *students;
    int start;
    int end;
    const char *target_name; /* points into stack memory in caller */
    int found_index;         /* -1 if not found in this chunk */
    int *found_flag;         /* pointer to shared flag used to stop other threads */
} SearchThreadData;

/* --- Sorting / utility functions (API) --- */
void merge_sort_by_average(Student students[], int left, int right);
float calculate_student_average(Student student);
void quick_sort_by_average(Student students[], int left, int right);

/* --- Main menu operations (API) --- */
void display_menu(void);
void add_student(Student students[], int *count);
void display_students(Student students[], int count);
void calculate_averages(Student students[], int count);
void sort_by_average_msort(Student students[], int count);
void sort_by_name(Student students[], int count);
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

/* --- Threaded helpers --- */
void calculate_averages_pthread(Student students[], int count);
void random_name(char *name, int min_len, int max_len);
void merge_sort_by_average_mt(Student students[], int left, int right);
void quick_sort_by_average_mt(Student students[], int left, int right);
void quick_sort_by_name_mt(Student students[], int left, int right);
void findStudentByName_mt(Student students[], int count);

#endif /* STUDENTS_H */
