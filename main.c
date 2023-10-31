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

char *checkDrive(char driveLetter) {
	char drivePath[4];
	sprintf(drivePath, "%c:", driveLetter);
	if(GetDriveType(drivePath) != DRIVE_NO_ROOT_DIR) return drivePath;
	return "";
}

void windowsSearch(const char* path, const char* fName) {
	
	char searchPath[MAX_PATH];
	snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
	
	sWIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    
	if (hFind == INVALID_HANDLE_VALUE) return;
	
	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			// Skip '.' and '..' directories
			if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
	            char subDirectory[MAX_PATH];
	            if (searchPath[strlen(searchPath)-1] == '*') searchPath[strlen(searchPath)-1] = '\0';
	            snprintf(subDirectory, sizeof(subDirectory), "%s\\%s", searchPath, findFileData.cFileName);
	            windowsSearch(subDirectory, fName);
			}
		} else {
            // Check if the file matches the search criteria
            if (strcmp(findFileData.cFileName, fName) == 0) {
            	printf("\n");
            	printf("\n");
            	positionText(5);
				printf("Found: %s \n", searchPath);
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
	
	int i;
	for (i=0; i<3; i++) {
		path = checkDrive(DRIVES[i]);
		if (path[0] != '\0') {
			windowsSearch(path, fName);
		}
	}
}

int main() {
	
	init();
	banner();
	printDate();
	search();
   	
   	return 0;
}
