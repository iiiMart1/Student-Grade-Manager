#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "students.h"
#include <pthread.h>
#include <semaphore.h>


#define MAX_STUDENTS 10000
double get_time_ms();
extern sem_t s;

int main()
{
    Student students[MAX_STUDENTS];
    int student_count = 0;
    int choice;
    sem_init(&s, 0, 1);

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

        case 3: // Calculate Averages
            double single_time = 1.0, multi_time = 0, start, end, start2, end2;
            int sub_choice;
            printf("1) Single-thread\n2) Multi-thread\n3) Compare\nEnter choice: ");
            if (scanf("%d", &sub_choice) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }

            if (sub_choice == 1)
            {
                start = get_time_ms();
                calculate_averages(students, student_count);
                end = get_time_ms();
                single_time = end - start; // ✅ assign, don't redeclare
                printf("Single-thread time: %.3f ms\n", single_time);
            }
            else if (sub_choice == 2)
            {
                start2 = get_time_ms();
                calculate_averages_pthread(students, student_count);
                end2 = get_time_ms();
                multi_time = end2 - start2; // ✅ assign
                printf("Multi-thread time: %.3f ms\n", multi_time);
                single_time = end - start;
                double speedup = single_time / multi_time;
                printf("Speedup By: %.2fx\n", speedup);
            }

            else
            {
                printf("Invalid choice.\n");
            }
            break;

        case 4: // Sort Students
        {
            int sub_choice;
            printf("1) By Average\n2) By Name\nEnter choice: ");
            if (scanf("%d", &sub_choice) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }

            if (sub_choice == 1)
            {
                int sort_choice;
                printf("1) MergeSort\n2) QuickSort\nEnter choice: ");
                if (scanf("%d", &sort_choice) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    break;
                }
                if (sort_choice == 1)
                {
                    double single_time = 1.0, multi_time = 0, start, end, start2, end2;
                    int sub_sub_choice;
                    printf("1) Single Threaded\n2) Multi Threaded\nEnter choice: ");
                    if (scanf("%d", &sub_sub_choice) != 1)
                    {
                        while (getchar() != '\n')
                            ;
                        break;
                    }
                    if (sub_sub_choice == 1)
                    {
                        start = get_time_ms();
                        sort_by_average_msort(students, student_count);
                        end = get_time_ms();
                        single_time = end - start; 
                        printf("Single-thread time: %.3f ms\n", single_time);
                    }
                    else if (sub_sub_choice == 2)
                    {

                        start2 = get_time_ms();
                        sort_by_average_threaded_msort(students, student_count);
                        end2 = get_time_ms();
                        multi_time = end2 - start2; 
                        printf("Multi-thread time: %.3f ms\n", multi_time);
                        single_time = end - start;
                        double speedup = single_time / multi_time;
                        printf("Speedup By: %.2fx\n", speedup);
                    }
                    else
                        printf("Invalid choice.\n");
                    break;
                }
                if (sort_choice == 2)
                {
                    double single_time = 1.0, multi_time = 0, start, end, start2, end2;
                    int sub_sub_choice;
                    printf("1) Single Threaded\n2) Multi Threaded\nEnter choice: ");
                    if (scanf("%d", &sub_sub_choice) != 1)
                    {
                        while (getchar() != '\n')
                            ;
                        break;
                    }
                    if (sub_sub_choice == 1)
                    {
                        start = get_time_ms();
                        sort_by_average_qsort(students, student_count);
                        end = get_time_ms();
                        single_time = end - start; 
                        printf("Single-thread time: %.3f ms\n", single_time);
                    }
                    else if (sub_sub_choice == 2)
                    {

                        start2 = get_time_ms();
                        sort_by_average_threaded_qsort(students, student_count);
                        end2 = get_time_ms();
                        multi_time = end2 - start2; 
                        printf("Multi-thread time: %.3f ms\n", multi_time);
                        single_time = end - start;
                        double speedup = single_time / multi_time;
                        printf("Speedup By: %.2fx\n", speedup);
                    }
                    else
                        printf("Invalid choice.\n");
                    break;
                }
            }

            else if (sub_choice == 2)
            {
                double single_time = 1.0, multi_time = 0, start, end, start2, end2;
                int sub_sub_choice;
                printf("1) Single Threaded\n2) Multi Threaded\nEnter choice: ");
                if (scanf("%d", &sub_sub_choice) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    break;
                }
                if (sub_sub_choice == 1)
                {
                    start = get_time_ms();
                    sort_by_name(students, student_count);
                    end = get_time_ms();
                    single_time = end - start; 
                    printf("Single-thread time: %.3f ms\n", single_time);
                }
                else if (sub_sub_choice == 2)
                {

                    start2 = get_time_ms();
                    sort_by_name_mt(students, student_count);
                    end2 = get_time_ms();
                    multi_time = end2 - start2; 
                    printf("Multi-thread time: %.3f ms\n", multi_time);
                    single_time = end - start;
                    double speedup = single_time / multi_time;
                    printf("Speedup By: %.2fx\n", speedup);
                }
            }

            else
                printf("Invalid choice.\n");

            break;
        }

        case 5:
            findStudentByName(students, student_count);
            break;

        case 6:
            findStudentByName_mt(students, student_count);
            break;
        case 7:
        {
            int num;
            printf("Enter number of students to generate: ");
            if (scanf("%d", &num) != 1)
            {
                while (getchar() != '\n')
                    ;
            } 
            create_test_data(students, &student_count, num);
        }
        break;
        case 8:
            int sta_choice;
            printf("1) Class Average:\n2) Top Students: \n3) Bottom Students: \n4) Grade Distribution: \nEnter choice: ");

            if (scanf("%d", &sta_choice) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }
            if (sta_choice == 1)
            {
                class_average(students, student_count);
                break;
            }
            if (sta_choice == 2)
            {
                int n;
                printf("Enter Number of Students To Sort ASC : ");
                if (scanf("%d", &n) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    break;
                }
                top_n_students(students, student_count, n);
                break;
            }
            if (sta_choice == 3)
            {
                int n;
                printf("Enter Number of Students To Sort DES : ");
                if (scanf("%d", &n) != 1)
                {
                    while (getchar() != '\n')
                        ;
                    break;
                }
                bottom_n_students(students, student_count, n);
                break;
            }
            if (sta_choice == 4)
            {
                grade_distribution(students, student_count);
                break;
            }

            break;
        case 9:
            printf("Exiting program. Goodbye!\n");
            break;

        default:
            printf("Invalid choice. Try again.\n");
        }
    } while (choice != 9);
    sem_destroy(&s);
    return 0;
}