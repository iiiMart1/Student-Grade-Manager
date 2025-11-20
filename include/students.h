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

// Function declarations for sorting algorithms
void merge_sort_by_average(Student students[], int left, int right);
void bitonic_sort_by_name(Student students[], int low, int cnt, int dir);
float calculate_student_average(Student student);

#endif