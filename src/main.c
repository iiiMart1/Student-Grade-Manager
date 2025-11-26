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
            double single_time = 0, multi_time = 0, start, end, start2, end2;
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
                double speedup = single_time / multi_time ;
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
            printf("1) By Name\n2) By Average\nEnter choice: ");
            if (scanf("%d", &sub_choice) != 1)
            {
                while (getchar() != '\n')
                    ;
                break;
            }

            if (sub_choice == 1)
                sort_by_name(students, student_count);
            else if (sub_choice == 2)
                sort_by_average(students, student_count);
            else
                printf("Invalid choice.\n");

            break;
        }

        case 5:
            test_sorting_functions(students, student_count);
            break;

        case 6:
        {
            int num;
            printf("Enter number of students to generate: ");
            if (scanf("%d", &num) != 1)
            {
                while (getchar() != '\n')
                    ;
            } // input validation
            create_test_data(students, &student_count, num);
        }
        break;
        case 7:
            findStudentByName(students, student_count);
            break;

        case 8:
            printf("Exiting program. Goodbye!\n");
            break;

        default:
            printf("Invalid choice. Try again.\n");
        }

    } while (choice != 8);
    sem_destroy(&s);
    return 0;
}