#define _CRT_SECURE_NO_WARNINGS
/*#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>*/ //uncomment this block to check for heap memory allocation leaks.
// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLINE 1023

typedef struct StudentCourseGrade
{
	char courseName[35];
	int grade;
}StudentCourseGrade;

typedef struct Student
{
	char name[35];
	StudentCourseGrade* grades; //dynamic array of courses
	int numberOfCourses;
}Student;


//Part A
void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
int countPipes(const char* lineBuffer, int maxCount);
char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents);
void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents);
void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor);
void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents);

//Part B
Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents);
void writeToBinFile(const char* fileName, Student* students, int numberOfStudents);
Student* readFromBinFile(const char* fileName);

int main()
{
	//Part A
	int* coursesPerStudent = NULL;
	int numberOfStudents = 0;
	countStudentsAndCourses("C:\\Users\\97250\\OneDrive\\שולחן העבודה\\לימודים\\סמסטר ב\\studentList (3).txt", &coursesPerStudent, &numberOfStudents);
	char*** students = makeStudentArrayFromFile("C:\\Users\\97250\\OneDrive\\שולחן העבודה\\לימודים\\סמסטר ב\\studentList (3).txt", &coursesPerStudent, &numberOfStudents);
	factorGivenCourse(students, coursesPerStudent, numberOfStudents, "Advanced Topics in C", +5);
	printStudentArray(students, coursesPerStudent, numberOfStudents); 
	studentsToFile(students, coursesPerStudent, numberOfStudents); //this frees all memory. Part B fails if this line runs. uncomment for testing (and comment out Part B)

	//Part B
	Student* student = transformStudentArray(students, coursesPerStudent, numberOfStudents);
	writeToBinFile("students.bin", student, numberOfStudents);
//	Student* testReadStudents = readFromBinFile("students.bin");

	//add code to free all arrays of struct Student


	/*_CrtDumpMemoryLeaks();*/ //uncomment this block to check for heap memory allocation leaks.
	// Read https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2019

	return 0;
}

void countStudentsAndCourses(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	char tempStr[MAXLINE];
	int NumOfStudents = 0;
	int counter = 0;

	FILE* studentList;
	studentList = fopen(fileName, "rt");
	if (studentList == NULL)
	{
		puts("Unable to open file...\n");
		exit(1);
	}

	while (!feof(studentList))
	{
		fgets(tempStr, MAXLINE, studentList);
		counter++;
	}

	*numberOfStudents = counter;
	rewind(studentList);
	*coursesPerStudent = (int*)calloc(counter, sizeof(int));
	if (*coursesPerStudent == NULL)
	{
		printf("allocation failed...");
		exit(1);
	}
	while (!feof(studentList))
	{

		fgets(tempStr, MAXLINE, studentList);
		(*coursesPerStudent)[NumOfStudents] = countPipes(tempStr, MAXLINE);
		NumOfStudents++;
	}

	fclose(studentList);
	return;
}

int countPipes(const char* lineBuffer, int maxCount)
{
	int counter = 0;
	int i = 0;
	if (lineBuffer == NULL)
	{
		return -1;
	}

	if (maxCount <= 0)
	{
		return 0;
	}

	while (lineBuffer[i] != '\0' || i == maxCount)
	{
		if (lineBuffer[i] == '|')
		{
			counter++;
		}
		i++;
	}
	return counter;
}

char*** makeStudentArrayFromFile(const char* fileName, int** coursesPerStudent, int* numberOfStudents)
{
	int len = 0;
	int counter = 0;
	char*** students = (char***)calloc(*numberOfStudents, sizeof(char**));
	if (students == NULL)
	{
		printf("allocation faild...\n");
		exit(1);
	}
	char line[MAXLINE];
	char temp[20];
	FILE* studentList;
	char* token;

	studentList = fopen(fileName, "r");
	if (studentList == NULL)
	{
		puts("Unable to open file...\n");
		exit(1);
	}

	for (int i = 0; i < *numberOfStudents; i++)
	{
		(students)[i] = (char**)calloc((*coursesPerStudent)[i] * 2 + 1, sizeof(char*));
		if (students[i] == NULL)
		{
			printf("allocation faild...\n");
			exit(1);
		}

		fgets(line, MAXLINE, studentList);

		for (int j = 1; j <= 2 * (*coursesPerStudent)[i]; j++)
		{
			if (counter == 0)
			{
				token = strtok(line, "|,\n");
				len = strlen(token);
				students[i][counter] = (char*)malloc(sizeof(char)*len);
				if (students[i][counter] == NULL)
				{
					printf("allocation faild...\n");
					exit(1);
				}
				
				strcpy(students[i][counter], token);
				counter = 1;
			}
			token = strtok(NULL, "|,\n");
			len = strlen(token);
			students[i][j] = (char*)malloc(sizeof(char)*len);
			if (students[i][j] == NULL)
			{
				printf("allocation faild...\n");
				exit(1);
			}
			strcpy(students[i][j], token);

		}
		counter = 0;
	}
	fclose(studentList);
	return students;

}

void factorGivenCourse(char** const* students, const int* coursesPerStudent, int numberOfStudents, const char* courseName, int factor)
{
	int grade_location = 0;
	char buffer[1023];
	int score=0;
	if (factor > -20 && factor < 20)
	{
		for (int i = 0; i < numberOfStudents; i++)
		{
			grade_location = (*(coursesPerStudent + i) * 2) + 1;
			for (int j = 1; j < grade_location; j = j + 2)
			{
				score = atoi(students[i][j + 1]);
				if (strcmp(students[i][j], courseName) == 0 )
					score += factor;
				_itoa(score, students[i][j + 1], 10);
				return;
			}//for_grade_location
		}//for_numberOfStudents
	}//if_factor is valid
	else
		return;
}


void printStudentArray(const char* const* const* students, const int* coursesPerStudent, int numberOfStudents)
{
	for (int i = 0; i < numberOfStudents; i++)
	{
		printf("name: %s\n*********\n", students[i][0]);
		for (int j = 1; j <= 2 * coursesPerStudent[i]; j += 2)
		{
			printf("course: %s\n", students[i][j]);
			printf("grade: %s\n", students[i][j + 1]);
		}
		printf("\n");
	}
}

void studentsToFile(char*** students, int* coursesPerStudent, int numberOfStudents)
{
	FILE* ptr_File;
	char buffer[1023];
	ptr_File = fopen("studentList.txt", "wt");
	if (ptr_File == NULL)
	{

		printf("unable to open ");
		exit(1);
	}
	for (int i = 0; i < numberOfStudents; i++)
	{
		int jumpe = (*(coursesPerStudent + i) * 2) + 1;
		for (int j = 0; j < jumpe; j++)
		{
			fputs(students[i][j], ptr_File);
			if (j == jumpe - 1)
				return;
			else
			{
				if (j % 2 == 0)
					fputc('|', ptr_File);
				else
					fputc(',', ptr_File);
			}//else_J
		}//for_jumpe
	}//for_numberOfStudents
	fclose(ptr_File);
	for (int i = 0; i < numberOfStudents; i++)
	{
		int jumpe = (*(coursesPerStudent + i) * 2) + 1;
		free(students[i][0]);
		for (int j = 1; j < jumpe; j += 2)
		{
			free(students[i][j]);
			free(students[i][j + 1]);
		}
		free(students[i]);
	}//for_numberOfStudents_2
	free(students);
	free(coursesPerStudent);
}

void writeToBinFile(const char* fileName, Student* students, int numberOfStudents)
{
	FILE* bFile = fopen(fileName, "wb");
	if (!bFile) {
		puts("cannot open file\n"); exit(1);
	}
	fwrite(&numberOfStudents, sizeof(int), 1, bFile);
	for (int i = 0; i < numberOfStudents; i++) {
		for (int j = 0; j < students[i].numberOfCourses; j++) {
		 fwrite(&students[i].grades[j].courseName, 35,1, bFile);
	     fwrite(&students[i].grades[j].grade, sizeof(int), 1, bFile);
		}

	}
	fclose(bFile);
}

Student* readFromBinFile(const char* fileName)
{
	FILE* bFile = fopen(fileName, "rb");
	if (!bFile) {
		puts("cannot open file\n"); exit(1);
	}
	int numofstudents;
	fread(&numofstudents, sizeof(int), 1, bFile);
	Student* students = (Student*)malloc(numofstudents * sizeof(Student));
	if (!students) {
		puts("allocation failled\n"); exit(1);
	}
	for (int i = 0; i < numofstudents; i++) {
		for (int j = 0; j < students[i].numberOfCourses; j++) {
			fread(&students[i].grades[j].courseName, 35,1, bFile);
			fread(&students [i].grades[j].grade, sizeof(int), 1, bFile);
		}

		students[i].grades = (StudentCourseGrade*)malloc(students[i].numberOfCourses * sizeof(StudentCourseGrade));

		if (!students[i].grades) {
			puts("allocation failled\n"); 
			exit(1);
		}
		fread(&students[i].grades, 39, students[i].numberOfCourses, bFile);


		
	}
	fclose(bFile);
}

Student* transformStudentArray(char*** students, const int* coursesPerStudent, int numberOfStudents)
{
	Student* student = (Student*)malloc(numberOfStudents * sizeof(Student));
	for (int i = 0; i < numberOfStudents; i++)
	{
		strcpy(student[i].name, students[i][0]);
		student[i].numberOfCourses = coursesPerStudent[i];
		student[i].grades = (StudentCourseGrade*)malloc(coursesPerStudent[i] * sizeof(StudentCourseGrade));
		for (int j = 1, k = 0; j < (coursesPerStudent[i] * 2) + 1; j++) {
			if (j % 2 == 1) {
				strcpy(student[i].grades[k].courseName, students[i][j]);
			}
			else {
				student[i].grades[k].grade = atoi(students[i][j]);
				k++;
			}
		}
	}
	return student;
}
