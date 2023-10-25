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

int checkDrive(char driveLetter) {
	char drivePath[4];
	sprintf(drivePath, "%c:\\", driveLetter);
	return GetDriveType(drivePath) != DRIVE_NO_ROOT_DIR;
}

void windowsSearch(const char* directory, const char* fName) {
	char searchPath[MAX_PATH];
	WIN32_FIND_DATA findFileData;
	sprintf(searchPath, "%c\\*", directory); // TO DO: to be fixed.
	printf("search path: %s\n", searchPath);
    HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    
	if (hFind == INVALID_HANDLE_VALUE) {
		printf("FindFirstFile failed (%d)\n", GetLastError());
		return;
	}
	
	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    		// Skip '.' and '..' directories
			if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
	            char subDirectory[MAX_PATH];
	            sprintf(subDirectory, "%s\\%s", directory, findFileData.cFileName);
	            printf("%s\n", subDirectory);
	            windowsSearch(subDirectory, fName);
	        }
        } else {
            // Check if the file matches the search criteria
            if (strcmp(findFileData.cFileName, fName) == 0) {
                printf("Found: %s\\%s\n", directory, findFileData.cFileName);
            }
        }
    } while (FindNextFile(hFind, &findFileData) != 0);
	
	FindClose(hFind);
    
}

void search() {
	char fName[32];
	positionText(5);
	printf("What file are you looking for today? \n");
	positionText(5);
	scanf("%s", fName);
	
	int i;
	for (i=0; i<3; i++) {
		if (checkDrive(DRIVES[i])) {
			windowsSearch(DRIVES[i], fName);
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
