#include "students.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// Helper function for bitonic sort - compare and swap
void comp_and_swap(Student students[], int i, int j, int dir)
{
    int comparison = strcmp(students[i].name, students[j].name);
    if ((dir == 1 && comparison > 0) || (dir == 0 && comparison < 0))
    {
        Student temp = students[i];
        students[i] = students[j];
        students[j] = temp;
    }
}

// Bitonic merge function
void bitonic_merge(Student students[], int low, int cnt, int dir)
{
    if (cnt > 1)
    {
        int k = cnt / 2;

        // Compare and swap elements across the entire sequence
        for (int i = low; i < low + k; i++)
        {
            comp_and_swap(students, i, i + k, dir);
        }

        // Recursively merge the two halves
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

        // Sort first half in ascending order
        bitonic_sort_by_name(students, low, k, 1);

        // Sort second half in descending order
        bitonic_sort_by_name(students, low + k, k, 0);

        // Merge the whole sequence in ascending order
        bitonic_merge(students, low, cnt, dir);
    }
}
