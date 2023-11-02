#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

const char DRIVES[4] = {'C','D','G','\0'};

void init() {
	system("cls");
	system("color 0A");
}

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

void positionText(int col) {
	printf("\033[%dC", col);
}

void printDate() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	positionText(5);
	printf("Date: %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

void loadingBar() 
{ 
	// Initialize char for printing 
	// loading bar 
	char a = 177, b = 219;
	
	printf("\n\n\n\n"); 
	printf("\n\n\n\n\t\t\t\t\tLoading...\n\n"); 
	printf("\t\t\t\t\t"); 
	
	// Print initial loading bar 
	int i;
	for (i=0; i<26; i++) printf("%c", a);

	// Set the cursor again starting 
	// point of loading bar 
	printf("\r"); 
	printf("\t\t\t\t\t"); 
	
	// Print loading bar progress 
	for (i=0; i<26; i++) { 
	    printf("%c", b); 
	    // Sleep for 1 second 
	    Sleep(1000); 
	}
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
	printf("What file are you looking for today? \n");
	positionText(5);
	scanf("%s", fName);
	
	char plural;
	int i, foundCntr=0;
	for (i=0; i<3; i++) {
		path = checkDrive(DRIVES[i]);
		if (path[0] != '\0') windowsSearch(path, fName, &foundCntr);
	}
	
	positionText(5);
	plural = (foundCntr > 1) ? 's' : '\0'; // 's' for multiple results or an empty char ('\0').
	if (foundCntr > 0) printf("We found %d file%c with that name.\n", foundCntr, plural);
}

void quit() {
	// TO DO : not secured and not working properly; need changing!
	int exitCode;
	positionText(5);
	printf("Type 1 if you want to exit? \n");
	positionText(5);
	scanf("%d", exitCode);
	if (exitCode == 1) exit(exitCode);
	return;
}

int main() {
	
	init();
	banner();
	printDate();
	//loadingBar();
	while (1) { 
		search();
		//quit();
	}
	
   	return 0;
}
