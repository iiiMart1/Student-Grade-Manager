#include "students.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAX_THREADS 8
#define MAX_STUDENTS 10000

/* Semaphore used to serialize console output in threaded workers so lines
   don't interleave when multiple threads print simultaneously. Initialize
   with sem_init(&s, 0, 1) in main. */
sem_t s;

/* Mutexes used to protect data */
pthread_mutex_t qs_mutex = PTHREAD_MUTEX_INITIALIZER;      /* quicksort (avg) */
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;  /* mergesort (avg)*/
pthread_mutex_t qs_name_mutex = PTHREAD_MUTEX_INITIALIZER; /* quicksort (name) */
pthread_mutex_t find_mutex = PTHREAD_MUTEX_INITIALIZER;    /* protects found flag in search */

/* Active thread counters for each area. Protected by their corresponding mutex. */
int qs_name_active_threads = 0, qs_active_threads = 0, active_threads = 0;

float calculate_student_average(Student student)
{
    return (student.opsys.TD + student.opsys.TP) * 0.2f + student.opsys.EXAM * 0.6f;
}

/* ------------------------------------------------------------------
 * display_menu
 * - Simple helper that prints the program menu.
 * ------------------------------------------------------------------ */
void display_menu()
{
    printf("\n========= Menu =========\n");
    printf("1) Add Student\n");
    printf("2) Display Students\n");
    printf("3) Calculate Averages\n");
    printf("4) Sort Students\n");
    printf("5) Find Student By Name\n");
    printf("6) Find Student By Name MT\n");
    printf("7) Create Test Data\n");
    printf("8) Statistics\n");
    printf("9) Exit\n");
}

/* ------------------------------------------------------------------
 * merge_by_average
 * - Merge step of merge sort.
 * - Sort order: descending by student average (highest first).
 * - Allocates temporary arrays for left/right halfes.
 * - On allocation failure it simply return after cleaning up.
 * ------------------------------------------------------------------ */
void merge_by_average(Student students[], int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    /* temporary buffers */
    Student *L = (Student *)malloc(n1 * sizeof(Student));
    Student *R = (Student *)malloc(n2 * sizeof(Student));
    if (!L || !R)
    {
        free(L);
        free(R);
        perror("merge_by_average: malloc");
        return;
    }

    /* copy data into temporaries */
    for (i = 0; i < n1; i++)
        L[i] = students[left + i];
    for (j = 0; j < n2; j++)
        R[j] = students[mid + 1 + j];

    /* merge back into students[left..right] */
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2)
    {
        if (calculate_student_average(L[i]) >= calculate_student_average(R[j]))
            students[k++] = L[i++];
        else
            students[k++] = R[j++];
    }

    /* remaining elements */
    while (i < n1)
        students[k++] = L[i++];
    while (j < n2)
        students[k++] = R[j++];

    free(L);
    free(R);
}

/* ------------------------------------------------------------------
 * merge_sort_by_average (single-threaded)
 * - Classic recursive merge sort for the array interval [left..right].
 * ------------------------------------------------------------------ */
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

/* forward declaration used by threaded variant */
void *merge_sort_worker(void *arg);

/* ------------------------------------------------------------------
 * merge_sort_by_average_mt (multi-threaded)
 * - Attempt to spawn a new thread to sort the left half while the
 *   current thread sorts the right half.
 * - Uses active_threads counter protected by thread_mutex to cap threads.
 * - If we can't create a new thread we fall back to sorting both halves
 *   in the current thread.
 * ------------------------------------------------------------------ */
void merge_sort_by_average_mt(Student students[], int left, int right)
{
    if (left >= right)
        return;

    int mid = left + (right - left) / 2;
    pthread_t thread;
    int thread_created = 0;

    /* try to claim a thread slot */
    pthread_mutex_lock(&thread_mutex);
    if (active_threads < MAX_THREADS)
    {
        active_threads++;
        thread_created = 1;
    }
    pthread_mutex_unlock(&thread_mutex);

    if (thread_created)
    {
        MergeSortData *data = malloc(sizeof(MergeSortData));
        if (!data)
        {
            /* allocation failed, fallback to sequential */
            merge_sort_by_average(students, left, mid);
        }
        else
        {
            data->students = students;
            data->left = left;
            data->right = mid;
            pthread_create(&thread, NULL, merge_sort_worker, data);

            /* do right half here */
            merge_sort_by_average_mt(students, mid + 1, right);

            /* wait for left half to finish */
            pthread_join(thread, NULL);
        }
    }
    else
    {
        /* no slot available: do both halves here */
        merge_sort_by_average_mt(students, left, mid);
        merge_sort_by_average_mt(students, mid + 1, right);
    }

    /* merge sorted halves */
    merge_by_average(students, left, mid, right);
}

/* ------------------------------------------------------------------
 * merge_sort_worker
 * - Thread entry used by merge_sort_by_average_mt. It frees the
 *   passed MergeSortData and decrements the active thread counter.
 * ------------------------------------------------------------------ */
void *merge_sort_worker(void *arg)
{
    MergeSortData *data = (MergeSortData *)arg;
    if (data)
    {
        merge_sort_by_average_mt(data->students, data->left, data->right);
        free(data);
    }

    pthread_mutex_lock(&thread_mutex);
    active_threads--;
    pthread_mutex_unlock(&thread_mutex);

    return NULL;
}

/* ------------------------------------------------------------------
 * Quicksort by average (single-threaded)
 * - Partition helper below; overall sort is descending by average.
 * ------------------------------------------------------------------ */
int partition_by_average(Student students[], int left, int right)
{
    float pivot = calculate_student_average(students[right]);
    int i = left - 1;

    for (int j = left; j < right; j++)
    {
        if (calculate_student_average(students[j]) >= pivot)
        {
            i++;
            Student tmp = students[i];
            students[i] = students[j];
            students[j] = tmp;
        }
    }

    Student tmp = students[i + 1];
    students[i + 1] = students[right];
    students[right] = tmp;

    return i + 1;
}

void quick_sort_by_average(Student students[], int left, int right)
{
    if (left < right)
    {
        int pi = partition_by_average(students, left, right);
        quick_sort_by_average(students, left, pi - 1);
        quick_sort_by_average(students, pi + 1, right);
    }
}

/* declaration for quicksort worker */
void *quick_sort_worker(void *arg);

/* ------------------------------------------------------------------
 * quick_sort_by_average_mt
 * - Thread-limited quicksort. Same pattern as mergesort_mt:
 *   spawn thread for left partition if allowed; process right partition
 *   in current thread, then join.
 * - Uses qs_mutex + qs_active_threads to cap concurrency.
 * ------------------------------------------------------------------ */
void quick_sort_by_average_mt(Student students[], int left, int right)
{
    if (left >= right)
        return;

    int pi = partition_by_average(students, left, right);
    int spawn_thread = 0;
    pthread_t thread;

    pthread_mutex_lock(&qs_mutex);
    if (qs_active_threads < MAX_THREADS)
    {
        qs_active_threads++;
        spawn_thread = 1;
    }
    pthread_mutex_unlock(&qs_mutex);

    if (spawn_thread)
    {
        QuickSortData *data = malloc(sizeof(QuickSortData));
        if (!data)
        {
            quick_sort_by_average(students, left, pi - 1);
        }
        else
        {
            data->students = students;
            data->left = left;
            data->right = pi - 1;
            pthread_create(&thread, NULL, quick_sort_worker, data);

            /* sort right side here */
            quick_sort_by_average_mt(students, pi + 1, right);

            pthread_join(thread, NULL);
        }
    }
    else
    {
        /* no thread available: run sequentially */
        quick_sort_by_average_mt(students, left, pi - 1);
        quick_sort_by_average_mt(students, pi + 1, right);
    }
}

/* ------------------------------------------------------------------
 * quick_sort_worker
 * - Worker entry for quicksort threads; frees data and decrements counter.
 * ------------------------------------------------------------------ */
void *quick_sort_worker(void *arg)
{
    QuickSortData *data = (QuickSortData *)arg;
    if (data)
    {
        quick_sort_by_average_mt(data->students, data->left, data->right);
        free(data);
    }

    pthread_mutex_lock(&qs_mutex);
    qs_active_threads--;
    pthread_mutex_unlock(&qs_mutex);

    return NULL;
}

/* ------------------------------------------------------------------
 * Name-based comparator & partition helpers
 * - compare_students_by_name: qsort comparator (ascending).
 * - partition_by_name / quick_sort_by_name_mt: threaded quicksort by name.
 * ------------------------------------------------------------------ */
int compare_students_by_name(const void *a, const void *b)
{
    const Student *s1 = (const Student *)a;
    const Student *s2 = (const Student *)b;
    return strcmp(s1->name, s2->name);
}

/* Partition ascending by name */
int partition_by_name(Student students[], int left, int right)
{
    char *pivot = students[right].name;
    int i = left - 1;

    for (int j = left; j < right; j++)
    {
        if (strcmp(students[j].name, pivot) <= 0)
        {
            i++;
            Student tmp = students[i];
            students[i] = students[j];
            students[j] = tmp;
        }
    }

    Student tmp = students[i + 1];
    students[i + 1] = students[right];
    students[right] = tmp;

    return i + 1;
}
void *quick_sort_name_worker(void *arg);

/* Multithreaded quicksort by name (same pattern as others) */
void quick_sort_by_name_mt(Student students[], int left, int right)
{
    if (left >= right)
        return;

    int pi = partition_by_name(students, left, right);
    int spawn_thread = 0;
    pthread_t thread;

    pthread_mutex_lock(&qs_name_mutex);
    if (qs_name_active_threads < MAX_THREADS)
    {
        qs_name_active_threads++;
        spawn_thread = 1;
    }
    pthread_mutex_unlock(&qs_name_mutex);

    if (spawn_thread)
    {
        QuickSortData *data = malloc(sizeof(QuickSortData));
        if (!data)
        {
            /* allocation failed: fall back to sequential */
            quick_sort_by_name_mt(students, left, pi - 1);
        }
        else
        {
            data->students = students;
            data->left = left;
            data->right = pi - 1;
            pthread_create(&thread, NULL, quick_sort_name_worker, data);

            /* sort right side in current thread */
            quick_sort_by_name_mt(students, pi + 1, right);

            pthread_join(thread, NULL);
        }
    }
    else
    {
        quick_sort_by_name_mt(students, left, pi - 1);
        quick_sort_by_name_mt(students, pi + 1, right);
    }
}

/* quicksort name worker — frees data and decrements counter */
void *quick_sort_name_worker(void *arg)
{
    QuickSortData *data = (QuickSortData *)arg;
    if (data)
    {
        quick_sort_by_name_mt(data->students, data->left, data->right);
        free(data);
    }

    pthread_mutex_lock(&qs_name_mutex);
    qs_name_active_threads--;
    pthread_mutex_unlock(&qs_name_mutex);

    return NULL;
}

/* ------------------------------------------------------------------
 * add_student
 * - Interactive helper to read a single student's data and append to array.
 * - Uses fgets to read the name and scanf for numeric fields.
 * - Validates grades are in [0,20]. If input is invalid it return without
 *   incrementing count.
 * ------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------
 * display_students
 * - prints all students with their average.
 * ------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------
 * calculate_averages (single-threaded)
 * - Prints average for each student.
 * ------------------------------------------------------------------ */
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

/* ------------------------------------------------------------------
 * calculate_averages_worker + calculate_averages_pthread
 * - Worker prints averages for a chunk and uses semaphore `s`.
 * ------------------------------------------------------------------ */
void *calculate_averages_worker(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    for (int i = data->start; i < data->end; i++)
    {
        sem_wait(&s);

        float average = (data->students[i].opsys.TD + data->students[i].opsys.TP) * 0.2f + data->students[i].opsys.EXAM * 0.6f;

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

#define NUM_THREADS 2
    pthread_t threads[NUM_THREADS];
    ThreadData data[NUM_THREADS];

    int chunk = count / NUM_THREADS;
    int extra = count % NUM_THREADS; // leftover students
    int start = 0;

    for (int t = 0; t < NUM_THREADS; t++)
    {
        data[t].students = students;
        data[t].start = start;

        /* give the first `extra` threads one extra element each */
        data[t].end = start + chunk + (t < extra ? 1 : 0);
        start = data[t].end;

        pthread_create(&threads[t], NULL, calculate_averages_worker, &data[t]);
    }

    /* wait for all worker threads to finish */
    for (int t = 0; t < NUM_THREADS; t++)
        pthread_join(threads[t], NULL);

    printf("Threaded calculation completed!\n");
}

/* ------------------------------------------------------------------
 * Sorting wrappers used by the UI
 * - They call the recursive algorithms and then print the results.
 * ------------------------------------------------------------------ */
void sort_by_average_msort(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    merge_sort_by_average(students, 0, count - 1);

    printf("\n=== After MergeSort by Average (Descending) ===\n");
    display_students(students, count);
}

void sort_by_average_qsort(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    quick_sort_by_average(students, 0, count - 1);

    printf("\n=== After QuickSort by Average (Descending) ===\n");
    display_students(students, count);
}

void sort_by_average_threaded_msort(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    merge_sort_by_average_mt(students, 0, count - 1);

    printf("\n=== After MultiThreaded MergeSort by Average (Descending) ===\n");
    display_students(students, count);
}

void sort_by_average_threaded_qsort(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    quick_sort_by_average_mt(students, 0, count - 1);

    printf("\n=== After MultiThreaded QuickSort by Average (Descending) ===\n");
    display_students(students, count);
}

/* ------------------------------------------------------------------
 * Name-based sorting helpers
 * - sort_by_name uses qsort (simple and reliable).
 * - sort_by_name_mt runs the threaded quicksort variant.
 * ------------------------------------------------------------------ */
void sort_by_name(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    qsort(students, count, sizeof(Student), compare_students_by_name);

    printf("\n=== After Sorting by Name (Ascending) ===\n");
    display_students(students, count);
}

void sort_by_name_mt(Student students[], int count)
{
    if (count == 0)
    {
        printf("No students to sort.\n");
        return;
    }

    quick_sort_by_name_mt(students, 0, count - 1);

    printf("\n=== After Sorting by Name (Ascending) ===\n");
    display_students(students, count);
}

/* ------------------------------------------------------------------
 * findStudentByName (single-threaded)
 * - Uses fgets to support spaces in names.
 * - Exact match only. => CASE SENSITIVE <= .
 * ------------------------------------------------------------------ */
void findStudentByName(Student students[], int count)
{
    char name[50];

    printf("Enter student name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

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
        printf("Student not found.\n");
    else
        printf("%s's average = %.2f\n", students[index].name, calculate_student_average(students[index]));
}

/* ------------------------------------------------------------------
 * Parallel search worker
 * - Each worker scans its assigned chunk and sets a shared found_flag
 *   when it finds a match. Access to found_flag is protected by
 *   find_mutex to avoid races.
 * - The caller must ensure the target_name pointer lives until threads join.
 * ------------------------------------------------------------------ */
void *search_worker(void *arg)
{
    SearchThreadData *data = (SearchThreadData *)arg;

    for (int i = data->start; i < data->end; i++)
    {
        if (*data->found_flag) /* stop early if another thread found it */
            return NULL;

        if (strcmp(data->students[i].name, data->target_name) == 0)
        {
            pthread_mutex_lock(&find_mutex);
            if (!(*data->found_flag)) /* double-check under lock */
            {
                data->found_index = i;
                *data->found_flag = 1;
            }
            pthread_mutex_unlock(&find_mutex);
            return NULL;
        }
    }

    return NULL;
}

/* ------------------------------------------------------------------
 * findStudentByName_mt
 * - Splits the array into NUM_THREADS chunks and launches search_worker
 *   on each chunk. Joins all threads and reports the first match (if any).
 * - The search is exact and case-sensitive.
 * ------------------------------------------------------------------ */
void findStudentByName_mt(Student students[], int count)
{
    char name[50];
    printf("Enter student name: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    pthread_t threads[NUM_THREADS];
    SearchThreadData thread_data[NUM_THREADS];
    int found_flag = 0;

    int chunk = count / NUM_THREADS;
    int extra = count % NUM_THREADS;
    int start = 0;

    for (int t = 0; t < NUM_THREADS; t++)
    {
        thread_data[t].students = students;
        thread_data[t].start = start;
        thread_data[t].end = start + chunk + (t < extra ? 1 : 0);
        thread_data[t].target_name = name; /* AI Generated I Dont Understand 100% Whats Happening Here ^_^*/
        thread_data[t].found_index = -1;
        thread_data[t].found_flag = &found_flag;

        pthread_create(&threads[t], NULL, search_worker, &thread_data[t]);
        start = thread_data[t].end;
    }

    int final_index = -1;
    for (int t = 0; t < NUM_THREADS; t++)
    {
        pthread_join(threads[t], NULL);
        if (thread_data[t].found_index != -1)
            final_index = thread_data[t].found_index;
    }

    if (final_index == -1)
        printf("Student not found.\n");
    else
        printf("%s's average = %.2f\n",
               students[final_index].name,
               calculate_student_average(students[final_index]));
}

/* ------------------------------------------------------------------
 * create_test_data
 * - Fill array with pseudo-random students for testing.
 * - Grades are sampled uniformly from 5..20 inclusive.
 * - Caller passes desired num_students and pointer to count; *count will
 *   be reset to the number actually created.
 * ------------------------------------------------------------------ */
void create_test_data(Student students[], int *count, int num_students)
{
    *count = 0; /* reset */

    for (int i = 0; i < num_students && *count < MAX_STUDENTS; i++)
    {
        /* assign id */
        students[*count].id = i + 1;

        /* random name */
        random_name(students[*count].name, 6, 7);

        /* uniform grades 7..20 */
        students[*count].opsys.TD = rand() % 14 + 6;
        students[*count].opsys.TP = rand() % 14 + 6;
        students[*count].opsys.EXAM = rand() % 14 + 6;

        (*count)++;
    }

    printf("Test data created with %d students.\n", *count);
}

/* ------------------------------------------------------------------
 * random_name
 * - Generate a pronounceable test name by alternating consonants and vowels.
 * - Capitalizes the first letter.
 * ------------------------------------------------------------------ */
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

    /* capitalize */
    if (len > 0)
        name[0] = name[0] - 'a' + 'A';

    name[len] = '\0';
}

/* ------------------------------------------------------------------
 * class_average, top_n_students, bottom_n_students, grade_distribution
 * - Simple stats helpers. No threading here.
 * - top/bottom assume the array is sorted appropriately when required.
 * ------------------------------------------------------------------ */
float class_average(Student students[], int count)
{
    if (count == 0)
        return 0.0f;

    float sum = 0;
    for (int i = 0; i < count; i++)
        sum += calculate_student_average(students[i]);

    printf("Class Average : %.2f\n", sum / count);
    return sum / count;
}

void top_n_students(Student students[], int count, int n)
{
    if (n > count)
        n = count;

    printf("\n--- Top %d Students ---\n", n);

    for (int i = 0; i < n; i++)
    {
        float avg = calculate_student_average(students[i]);
        printf("%d) %s  Average: %.2f\n", i + 1, students[i].name, avg);
    }
}

void bottom_n_students(Student students[], int count, int n)
{
    if (n > count)
        n = count;

    printf("\n--- Bottom %d Students ---\n", n);
    int start = count - n;
    for (int i = count - 1; i >= start; i--)
    {
        float avg = calculate_student_average(students[i]);
        printf("%d) %s  Average: %.2f\n", i + 1, students[i].name, avg);
    }
}

void grade_distribution(Student students[], int count)
{
    if (count == 0)
    {
        puts("No students in the system.");
        return;
    }

    int A = 0, B = 0, C = 0, D = 0, F = 0;

    for (int i = 0; i < count; i++)
    {
        float avg = calculate_student_average(students[i]);

        if (avg >= 16)
            A++;
        else if (avg >= 14)
            B++;
        else if (avg >= 12)
            C++;
        else if (avg >= 10)
            D++;
        else
            F++;
    }

    printf("\n--- Grade Distribution ---\n");
    printf("Excellent : %d (%.2f%%)\n", A, 100.0 * A / count);
    printf("Very Good : %d (%.2f%%)\n", B, 100.0 * B / count);
    printf("Good      : %d (%.2f%%)\n", C, 100.0 * C / count);
    printf("Average   : %d (%.2f%%)\n", D, 100.0 * D / count);
    printf("Weak      : %d (%.2f%%)\n", F, 100.0 * F / count);
}

/* ------------------------------------------------------------------
 * get_time_ms
 * - Platform-specific high-resolution clock used by main.c.
 * ------------------------------------------------------------------ */
#ifdef _WIN32
#include <windows.h>
double get_time_ms()
{
    static LARGE_INTEGER freq;
    static int freq_init = 0;

    if (!freq_init)
    {
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
