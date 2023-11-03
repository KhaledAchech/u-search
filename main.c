#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

const char DRIVES[4] = {'C','D','G','\0'};
pthread_mutex_t lock;
int progress = 0;

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

void init() {
	system("cls");
	system("color 0A");
	banner();
	printDate();
}

void *loadingBar(void *args) 
{ 
	char a = 177, b = 219;
	int barLength = 26;
	
	printf("\n\n\n\n"); 
	printf("\n\n\n\n\t\t\t\t\tLoading...\n\n"); 
	printf("\t\t\t\t\t"); 
	
	int i;
	while (progress < 100) {
		pthread_mutex_lock(&lock);
		
		printf("\r\t\t\t\t\t");
        for (i=0; i<barLength; i++) {
			if (i < (progress * barLength / 100)) printf("%c", b);
			else printf("%c", a);
		}
		printf(" %d%%", progress);
		fflush(stdout);
		pthread_mutex_unlock(&lock);
	}

	return NULL;
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
	            positionText(5);
				printf("\nSearching in : %s\n", subDirectory);
				positionText(5);
	            windowsSearch(subDirectory, fName, cntr);
			}
		} else {
            // Check if the file matches the search criteria
            if (strcmp(findFileData.cFileName, fName) == 0) {
            	printf("\n");
            	printf("\n");
            	positionText(5);
				printf("Found: %s \n", searchPath);
				(*cntr)++;
            	printf("\n");
            	printf("\n");
            }
		}
    } while (FindNextFile(hFind, &findFileData) != 0);
	
	DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) printf("FindNextFile failed (%d)\n", dwError);
	
	FindClose(hFind);
    
}

void search() {
	char fName[32];
	char *path;
	positionText(5);
	printf("What file are you looking for? \n");
	positionText(5);
	scanf("%s", fName);
    
	char plural;
	int i, foundCntr=0;
	for (i=0; i<3; i++) {
		path = checkDrive(DRIVES[i]);
		if (path[0] != '\0') {
			windowsSearch(path, fName, &foundCntr);
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
	
	init();
	menu();
	
   	return 0;
}
