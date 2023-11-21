#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>
#define BUFSIZE 65536

DWORD errorID;
LPVOID errorMsg;

const char DRIVES[4] = {'C','D','G','\0'};
int loadingFlag = 1;

void banner() {
	printf("\n");
	printf("     /\\__\\         /\\  \\         /\\  \\         /\\  \\         /\\  \\    \n");
	printf("    /:/  /        /::\\  \\       /::\\  \\       /::\\  \\       /::\\  \\   \n");
	printf("   /:/  /        /:/\\ \\  \\     /:/\\:\\  \\     /:/\\:\\  \\     /:/\\:\\  \\  \n");
	printf("  /:/  /  ___   _\\:\\~\\ \\  \\   /::\\~\\:\\  \\   /::\\~\\:\\  \\   /::\\~\\:\\  \\ \n");
	printf(" /:/__/  /\\__\\ /\\ \\:\\ \\ \\__\\ /:/\\:\\ \\:\\__\\ /:/\\:\\ \\:\\__\\ /:/\\:\\ \\:\\__\\\n");
	printf(" \\:\\  \\ /:/  / \\:\\ \\:\\ \\/__/ \\:\\~\\:\\ \\/__/ \\/__\\:\\/:/  / \\/_|::\\:/  / \n");
	printf("  \\:\\  /:/  /   \\:\\ \\:\\__\\    \\:\\ \\:\\__\\        \\::/  /     |:|::/  /  \n");
	printf("   \\:\\/:/  /     \\:\\/:/  /     \\:\\ \\/__/        /:/  /      |:|\\/__/   \n");
	printf("    \\::/  /       \\::/  /       \\:\\__\\         /:/  /       |:|  |     \n");
	printf("     \\/__/         \\/__/         \\/__/         \\/__/         \\|__|     \n");
	printf("\n");
}

// TO DO: add row tabulation as well
void positionText(int col) {
	printf("\033[%dC", col);
}

void printDate() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	positionText(5);
	printf("Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void hideCursor()
{
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
}

void init() {
	SetConsoleTitle("Ultimum Searcher");
	system("cls");
	system("color 0A");
	banner();
	printDate();
	hideCursor();
}

// TO DO: FIX: clearing the loading stream persist
void loading() { 
	const int trigger = 50000; // ms
	const int numDots = 4;
	const char prompt[] = "Loading";
	int i;
	
	printf("\n\n");
	while (loadingFlag) {
		// Return and clear with spaces, then return and print prompt.
		printf("\r\t%*s\r\t%s", sizeof(prompt) - 1 + numDots, "", prompt);
		fflush(stdout);
	
		// Print numDots number of dots, one every trigger milliseconds.
		for (i=0; i<numDots; i++) {
			usleep(trigger);
			fputc('.', stdout);
			fflush(stdout);
		}
	}
	
	//Delete the last "Loading..." output
	printf("\r%*s", sizeof(prompt) - 1 + numDots, "");
}

// TO DO: make the necessary changes in the other parts of the code to use this helper method instead; a helping method to print errors
void handlingErrors() {
	errorID = getLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		errorID,
		0,
		(LPWSR)&errorMsg,
		0,
		NULL
	);
	
	if (errorMsg != NULL) {
		wprintf(L"Error %u: %s\n", errorID, errorMsg);
		LocalFree(errorMsg);
	} else {
		fprintf(stderr, "Error formating message for code %d", errorID);
	}
}

// Saves a copy to the file in the windows user local temp folder
void saveToTemp() {
	DWORD bufferSize = BUFSIZE;
	char tempPath[MAX_PATH];

	DWORD result = GetTempPathA(bufferSize, tempPath);
	handlingErrors();
	if (result) printf("Found the temp path: %s", tempPath);
}

char *checkDrive(char driveLetter) {
	char drivePath[4];
	sprintf(drivePath, "%c:", driveLetter);
	if(GetDriveType(drivePath) != DRIVE_NO_ROOT_DIR) return drivePath;
	return "";
}

void windowsSearch(const char* path, const char* fName, int* cntr) {
	
	char searchPath[MAX_PATH];
	snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
	
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    
	if (hFind == INVALID_HANDLE_VALUE) return;
	
	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Skip '.' and '..' directories
			if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
				char subDirectory[MAX_PATH];
				if (searchPath[strlen(searchPath)-1] == '*') searchPath[strlen(searchPath)-1] = '\0';
				snprintf(subDirectory, sizeof(subDirectory), "%s\\%s", searchPath, findFileData.cFileName);
				windowsSearch(subDirectory, fName, cntr);
			}
		} else {
            // Check if the file matches the search criteria
            if (strcmp(findFileData.cFileName, fName) == 0) {
            	// TO DO: add the result to the temp list of paths;
				(*cntr)++;
            }
		}
    } while (FindNextFile(hFind, &findFileData) != 0);
	
	DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) printf("FindNextFile failed (%d)\n", dwError);
	
	FindClose(hFind);
}

void* loadingThreadFunction(void* arg) {
	loading(); // Start the loading animation.
	return NULL;
}

void search() {
	char fName[32];
	char *path;
	pthread_t loadingThread;
	
	positionText(5);
	printf("What file are you looking for? \n");
	positionText(5);
	scanf("%s", fName);
    
	char plural;
	int i, foundCntr=0;
	for (i=0; i<3; i++) {
		path = checkDrive(DRIVES[i]);
		if (path[0] != '\0') {
			loadingFlag = 1;
			// Start the loading thread.
			if (pthread_create(&loadingThread, NULL, loadingThreadFunction, NULL) != 0) {
				perror("Error creating loading thread");
				exit(1);
			}
			windowsSearch(path, fName, &foundCntr);
			loadingFlag = 0;
            pthread_join(loadingThread, NULL);
		}
	}
	
	plural = (foundCntr > 1) ? 's' : '\0'; // 's' for multiple results or an empty char ('\0').
   	printf("\n");
	printf("\n");
	positionText(5);
	if (foundCntr > 0) printf("We found %d file%c with the '%s' name.\n", foundCntr, plural, fName);
}

void menu() {
	int choice;
	
	do {
		printf("\n");
		printf("\n");
		positionText(5);
		printf("U-searcher Menu:\n");
		positionText(7);
		printf("1. Search for a File\n");
		positionText(7);
		printf("2. Exit\n");
		printf("\n");
		positionText(5);
		printf("Enter your choice: ");
		scanf("%d", &choice);
		printf("\n");
		
		switch (choice) {
		case 1:
			search();
			break;
		case 2:
			exit(1);
			break;
		default:
			positionText(5);
			printf("Invalid choice. Try again.\n");
		}
	} while (choice != 2);

}

int main() {
	saveToTemp();
//	init();
//	menu();
	
   	return 0;
}
