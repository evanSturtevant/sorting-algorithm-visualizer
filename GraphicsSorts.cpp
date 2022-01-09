/*
Evan Sturtevant, Emma Pinto
Graphics Sorts
Writing Assignment 02
Algorithms and Analysis; Dr. Kruse
10.13.2021
*/

#include <GL/glut.h>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cstdlib>
#include <sstream>
#include <direct.h>

using namespace std;

//Graphics functions
void idle();
void display();
void myInit();
void calcOrigin();
void reshape(GLint, GLint);
void keyboard(unsigned char, int, int);

//CSV creation parameters
//Feel free to change these!
#define ASCEND false
#define RANDOM true

#define CREATE_SMALL true //If you already have small or med csv files generated you can disable these
#define CREATE_MED true

//FileNames
string smallCSV = "files/sortOutputsSmall_";
string medCSV = "files/sortOutputMed_";

//Graphics parameters
int HEIGHT_OF_WINDOW = 750;
int WIDTH_OF_WINDOW = 1200;
const int smallN = 50, medN = 150;
int smallArray[smallN], medArray[medN];

//Graphics globals
int insertionArray[medN], selectionArray[medN], mergeArray[medN], quickArray[medN];
int arrayIndex = 0;
bool finishedInsertion, finishedSelection, finishedMerge, finishedQuick;
char currentDisplay = 's';
int insertionOrigin[2], selectionOrigin[2], mergeOrigin[2], quickOrigin[2];


void swap(int arr[], int x, int y) {
	int temp = arr[x];
	arr[x] = arr[y];
	arr[y] = temp;
}

void randomizeArrays() {
	for (int i = 0; i < medN; i++) {
		if (i < smallN) 
			swap(smallArray, i, rand() % smallN);
		swap(medArray, i, rand() % medN);
	}
}

void genArrays() {
	for (int i = 0; i < medN; i++) {
		if (i < smallN) {
			if (ASCEND)
				smallArray[i] = i + 1;
			else
				smallArray[i] = smallN - i;
		}
		if (ASCEND)
			medArray[i] = i + 1;
		else
			medArray[i] = medN - i;
	}
	if (RANDOM)
		randomizeArrays();
}

void printArray(int arr[], int size) {
	cout << endl << '[';
	bool sorted = true;
	for (int i = 0; i < size-1; i++) {
		cout << arr[i] << ',';
		if (!(arr[i] <= arr[i + 1]))
			sorted = false;
	} cout << arr[size - 1] << ']' << endl;
	cout << "Sorted: ";
	if (sorted)
		cout << "true";
	else
		cout << "false";
	cout << endl;
}

void printArrays() {
	cout << "Small: " << endl;
	printArray(smallArray, smallN);
	cout << std::string(50, '-') << endl;

	cout << "Medium: " << endl;
	printArray(medArray, medN);
	cout << std::string(50, '-') << endl;

}

void appendCSV(int A[], int length, ofstream& outFile) {
	for (int i = 0; i < length - 1; i++) {
		outFile << right << setw(4) << setfill('0') << A[i] << ',';
	}outFile << right << setw(4) << setfill('0') << A[length - 1] << endl;
}

//***********************************************SORTING ALGORITHMS***********************************************
void insertionSort(int A[], int length, ofstream& outFile) {
	for (int j = 1; j < length; j++) {
		appendCSV(A, length, outFile);
		int key = A[j];
		int i = j - 1;
		if (A[i] <= key) 
			appendCSV(A, length, outFile);
		while (i >= 0 and A[i] > key) {
			A[i + 1] = A[i];
			A[i] = key;
			i--;
			appendCSV(A, length, outFile);
		}A[i + 1] = key;
	}
}

void selectionSort(int A[], int length, ofstream& outFile) {
	for (int i = 0; i < length - 1; i++) {
		int smallest = i;
		for (int j = i + 1; j < length; j++) {
			appendCSV(A, length, outFile);
			if (A[smallest] > A[j]) {
				smallest = j;
			}
		}
		swap(A, i, smallest);
		appendCSV(A, length, outFile);
	}
}

void merge(int A[], const int length, ofstream& outFile, int p, int q, int r) {
	int n1 = q - p + 1;
	int n2 = r - q;
	int L[medN]; int R[medN];
	
	for (int i = 0; i < n1; i++) {
		L[i] = A[p + i];
	}
	for (int j = 0; j < n2; j++) {
		R[j] = A[q + j + 1];
	}

	L[n1] = INT_MAX; R[n2] = INT_MAX;

	int i = 0; int j = 0;
	for (int k = p; k <= r; k++) {
		if (L[i] <= R[j]) {
			A[k] = L[i];
			i++;
		}
		else {
			A[k] = R[j];
			j++;
		}
		appendCSV(A, length, outFile);
	}

}

void mergeSort(int A[], int length, ofstream& outFile, int p, int r) {
	if (p < r) {
		int q = floor((p + r) / 2);
		mergeSort(A, length, outFile, p, q);
		mergeSort(A, length, outFile, q + 1, r);
		merge(A, length, outFile, p, q, r);
	}
}

int partition(int A[], int length, ofstream& outFile, int p, int r) {
	int x = A[r];
	int i = p - 1;

	for (int j = p; j <= r - 1; j++) {
		appendCSV(A, length, outFile);
		if (A[j] <= x) {
			i = i + 1;
			swap(A, i, j);
		}
	}
	swap(A, r, i + 1);
	appendCSV(A, length, outFile);
	return i + 1;
}

void quickSort(int A[], int length, ofstream& outFile, int p, int r) {
	if (p < r) {
		int q = partition(A, length, outFile, p, r);
		quickSort(A, length, outFile, p, q - 1);
		quickSort(A, length, outFile, q + 1, r);
	}
}

void insertionSortPrep() {
	int insertionSmall[smallN], insertionMed[medN];

	copy(smallArray, smallArray + smallN, insertionSmall);
	copy(medArray, medArray + medN, insertionMed);

	clock_t start, finish;
	double time;

	if (CREATE_SMALL) {
		cout << "Creating Small Insertion! ";
		start = clock();

		ofstream smallInsertionFile;
		smallInsertionFile.open(smallCSV + "Insertion.csv");
		insertionSort(insertionSmall, smallN, smallInsertionFile);
		smallInsertionFile.close();
		
		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;

	}
	
	if (CREATE_MED) {
		cout << "Creating Med Insertion! ";
		start = clock();

		ofstream medInsertionFile;
		medInsertionFile.open(medCSV + "Insertion.csv");
		insertionSort(insertionMed, medN, medInsertionFile);
		medInsertionFile.close();

		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;
	}
}

void selectionSortPrep() {
	int selectionSmall[smallN], selectionMed[medN];

	copy(smallArray, smallArray + smallN, selectionSmall);
	copy(medArray, medArray + medN, selectionMed);


	clock_t start, finish;
	double time;

	if (CREATE_SMALL) {
		cout << "Creating Small Selection! ";
		start = clock();

		ofstream smallSelectionFile;
		smallSelectionFile.open(smallCSV + "Selection.csv");
		selectionSort(selectionSmall, smallN, smallSelectionFile);
		smallSelectionFile.close();

		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;
	}

	if (CREATE_MED) {
		cout << "Creating Med Selection! ";
		start = clock();

		ofstream medSelectionFile;
		medSelectionFile.open(medCSV + "Selection.csv");
		selectionSort(selectionMed, medN, medSelectionFile);
		medSelectionFile.close();

		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;
	}
}

void mergeSortPrep() {
	int mergeSmall[smallN], mergeMed[medN];

	copy(smallArray, smallArray + smallN, mergeSmall);
	copy(medArray, medArray + medN, mergeMed);

	clock_t start, finish;
	double time;

	if (CREATE_SMALL) {
		cout << "Creating Small Merge! ";
		start = clock();

		ofstream smallMergeFile;
		smallMergeFile.open(smallCSV + "Merge.csv");
		mergeSort(mergeSmall, smallN, smallMergeFile, 0, smallN - 1);
		smallMergeFile.close();

		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;
	}

	if (CREATE_MED) {
		cout << "Creating Med Merge! ";
		start = clock();

		ofstream medMergeFile;
		medMergeFile.open(medCSV + "Merge.csv");
		mergeSort(mergeMed, medN, medMergeFile, 0, medN - 1);
		medMergeFile.close();

		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;
	}
}

void quickSortPrep() {
	int quickSmall[smallN], quickMed[medN];

	copy(smallArray, smallArray + smallN, quickSmall);
	copy(medArray, medArray + medN, quickMed);

	clock_t start, finish;
	double time;

	if (CREATE_SMALL) {
		cout << "Creating Small Quick! ";
		start = clock();

		ofstream smallQuickFile;
		smallQuickFile.open(smallCSV + "Quick.csv");
		quickSort(quickSmall, smallN, smallQuickFile, 0, smallN - 1);
		smallQuickFile.close();

		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;
	}

	if (CREATE_MED) {
		cout << "Creating Med Quick! ";
		start = clock();

		ofstream medQuickFile;
		medQuickFile.open(medCSV + "Quick.csv");
		quickSort(quickMed, medN, medQuickFile, 0, medN - 1);
		medQuickFile.close();

		finish = clock();
		time = (double(finish) - double(start)) / CLOCKS_PER_SEC;
		cout << "Finished in " << time << "s" << endl;
	}
}

int main(int argc, char** argv) {
	srand(time(NULL));
	_mkdir("./files");
	
	cout << "Once the simulation starts press 'm' or 's' to switch between small and medium sized N" << endl <<
		"Space or 'q' to quit" << endl <<
		"Any other button will reset sort progress!" << endl <<
		"Feel free to disable generating the files once you have them and to mess around with the randomized array" << endl <<
		"(these are boolean defintions in the top of the C++ file)" << endl << endl;

	if (CREATE_SMALL || CREATE_MED) {
		cout << "Creating CSV files!" << endl;
	}
	
	genArrays();
	insertionSortPrep();
	selectionSortPrep();
	mergeSortPrep();
	quickSortPrep();
	
	Sleep(500);

	finishedInsertion = false;
	finishedSelection = false;
	finishedMerge = false;
	finishedQuick = false;
	//*
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(WIDTH_OF_WINDOW, HEIGHT_OF_WINDOW);
	glutInitWindowPosition(20, 20);
	glutCreateWindow("SORT!");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	myInit();

	glutIdleFunc(idle);
	display();

	glutMainLoop();

	//*/
	return 0;
}


//***********************************************GRAPHICS CODE***********************************************
string getFileSizeName() {
	switch (currentDisplay) {
	case 's':
		return smallCSV;
	case 'm':
		return medCSV;
	}
}

int getFileSize() {
	switch (currentDisplay) {
	case 's':
		return smallN;
	case 'm':
		return medN;
	}
}

void convertString(int A[], string line) {
	int N = getFileSize();
	stringstream stream(line);

	for (int i = 0; i < N; i++) {
		if (stream.good()) {
			string token;
			getline(stream, token, ',');

			stringstream strToken(token);
			strToken >> A[i];
		}
	}
	string token;
	getline(stream, token);

	stringstream strToken(token);
	strToken >> A[N - 1];
}

void readFile(int A[], string fileName, bool& sortFinished) {
	string line;
	ifstream file;

	file.open(getFileSizeName() + fileName);
	file.seekg((5 * getFileSize() + 1) * arrayIndex);
	if (getline(file, line)) {
		convertString(A, line);
	}
	else {
		sortFinished = true;
	}
	file.close();
}

void idle() {
	readFile(insertionArray, "Insertion.csv", finishedInsertion);
	readFile(selectionArray, "Selection.csv", finishedSelection);
	readFile(mergeArray, "Merge.csv", finishedMerge);
	readFile(quickArray, "Quick.csv", finishedQuick);

	arrayIndex++;
	glutPostRedisplay();
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT);

	int length = getFileSize();
	double spaceForRect = (WIDTH_OF_WINDOW / 2.0) / (double)length;
	double padding = 0;//spaceForRect * 0.005;

	
	//Insertion Sort
	if (finishedInsertion) {
		glColor3f(0, 1, 0);
	}
	else {
		glColor3f(0, 0, 0);
	}

	for (int i = 0; i < length; i++) {
		int rectHeight = ((double)insertionArray[i] / length) *(.95 * HEIGHT_OF_WINDOW / 2.0);

		glBegin(GL_POLYGON);
		glVertex2d((insertionOrigin[0] + (i * spaceForRect + padding)), insertionOrigin[1]);
		glVertex2d((insertionOrigin[0] + ((i+1) * spaceForRect) - (padding+1)), insertionOrigin[1]);
		glVertex2d((insertionOrigin[0] + ((i+1) * spaceForRect) - (padding+1)), insertionOrigin[1] + rectHeight);
		glVertex2d((insertionOrigin[0] + (i * spaceForRect + padding)), insertionOrigin[1] + rectHeight);
		glEnd();
	}

	//Selection Sort
	if (finishedSelection) {
		glColor3f(0, 1, 0);
	}
	else {
		glColor3f(0, 0, 0);
	}

	for (int i = 0; i < length; i++) {
		int rectHeight = ((double)selectionArray[i] / length) * (.95 * HEIGHT_OF_WINDOW / 2.0);

		glBegin(GL_POLYGON);
		glVertex2d((selectionOrigin[0] + (i * spaceForRect + padding)), selectionOrigin[1]);
		glVertex2d((selectionOrigin[0] + ((i + 1) * spaceForRect) - (padding + 1)), selectionOrigin[1]);
		glVertex2d((selectionOrigin[0] + ((i + 1) * spaceForRect) - (padding + 1)), selectionOrigin[1] + rectHeight);
		glVertex2d((selectionOrigin[0] + (i * spaceForRect + padding)), selectionOrigin[1] + rectHeight);
		glEnd();
	}

	//Merge Sort
	if (finishedMerge) {
		glColor3f(0, 1, 0);
	}
	else {
		glColor3f(0, 0, 0);
	}

	for (int i = 0; i < length; i++) {
		int rectHeight = ((double)mergeArray[i] / length) * (.95 * HEIGHT_OF_WINDOW / 2.0);

		glBegin(GL_POLYGON);
		glVertex2d((mergeOrigin[0] + (i * spaceForRect + padding)), mergeOrigin[1]);
		glVertex2d((mergeOrigin[0] + ((i + 1) * spaceForRect) - (padding + 1)), mergeOrigin[1]);
		glVertex2d((mergeOrigin[0] + ((i + 1) * spaceForRect) - (padding + 1)), mergeOrigin[1] + rectHeight);
		glVertex2d((mergeOrigin[0] + (i * spaceForRect + padding)), mergeOrigin[1] + rectHeight);
		glEnd();
	}

	//Quick Sort
	if (finishedQuick) {
		glColor3f(0, 1, 0);
	}
	else {
		glColor3f(0, 0, 0);
	}

	for (int i = 0; i < length; i++) {
		int rectHeight = ((double)quickArray[i] / length) * (.95 * HEIGHT_OF_WINDOW / 2.0);

		glBegin(GL_POLYGON);
		glVertex2d((quickOrigin[0] + (i * spaceForRect + padding)), quickOrigin[1]);
		glVertex2d((quickOrigin[0] + ((i + 1) * spaceForRect) - (padding + 1)), quickOrigin[1]);
		glVertex2d((quickOrigin[0] + ((i + 1) * spaceForRect) - (padding + 1)), quickOrigin[1] + rectHeight);
		glVertex2d((quickOrigin[0] + (i * spaceForRect + padding)), quickOrigin[1] + rectHeight);
		glEnd();
	}

	//Boarder lines between the sorts
	glColor3f(1, 0, 0);

	glBegin(GL_LINES);
	glVertex2d(WIDTH_OF_WINDOW / 2, 0);
	glVertex2d(WIDTH_OF_WINDOW / 2, HEIGHT_OF_WINDOW);
	glEnd();
	glBegin(GL_LINES);
	glVertex2d(0, HEIGHT_OF_WINDOW / 2);
	glVertex2d(WIDTH_OF_WINDOW, HEIGHT_OF_WINDOW / 2);
	glEnd();

	glutSwapBuffers();

}

void myInit()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, WIDTH_OF_WINDOW, 0, HEIGHT_OF_WINDOW);
	glMatrixMode(GL_MODELVIEW);
}

void calcOrigin() {
	insertionOrigin[0] = 0; insertionOrigin[1] = HEIGHT_OF_WINDOW / 2;
	selectionOrigin[0] = 0; selectionOrigin[1] = 0;
	mergeOrigin[0] =  WIDTH_OF_WINDOW / 2; mergeOrigin[1] = HEIGHT_OF_WINDOW / 2 ;
	quickOrigin[0] =  WIDTH_OF_WINDOW / 2; quickOrigin[1] = 0;
}

void reshape(GLint w, GLint h)
{

	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glMatrixMode(GL_MODELVIEW);
	HEIGHT_OF_WINDOW = h;
	WIDTH_OF_WINDOW = w;
	calcOrigin();

}

void keyboard(unsigned char key, int x, int y) {
	if (key == 'q' || key == 'Q' || key == ' ') exit(0);
	if (key == 's' || key == 'S') {
		currentDisplay = 's';
	}
	if (key == 'm' || key == 'M') {
		currentDisplay = 'm';
	}
	arrayIndex = 0;
	finishedInsertion = false;
	finishedSelection = false;
	finishedMerge = false;
	finishedQuick = false;
}