#include <stdio.h>
#include <string.h>

typedef struct {
    char name[50];
    float grade1;
    float grade2;
    float grade3;
} Student;

void findStudentByName(Student students[], int count) {
    char name[50];

    printf("Enter student name: ");
    scanf("%49s", name);

    int index = -1;

    for (int i = 0; i < count; i++) {
        if (strcmp(students[i].name, name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("Student not found.\n");
    } else {
        printf("%s's average = %.2f\n",
               students[index].name,
               getStudentAverage(students[index]));
    }
}

float getStudentAverage(Student s) {
    return (s.grade1 + s.grade2 + s.grade3) / 3.0;
}

int main() {
    Student students[] = {
        {"Ali",  12, 15, 17},
        {"Sara", 14, 18, 16},
        {"Nabil", 10, 11, 12}
    };
    int count = sizeof(students) / sizeof(students[0]);

    findStudentByName(students, count);

    return 0;
}

