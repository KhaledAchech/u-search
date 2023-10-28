#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

const char DRIVES[4] = {'C','D','G','\0'};
HANDLE hSearchFinishedEvent;
HANDLE hLoadingBarThread;

struct SearchParameters {
    const char* path;
    const char* fName;
};

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

void loadingBar()
{ 
    char a = 177, b = 219; 
  
    printf("\n\n\n\n"); 
    printf("\n\n\n\n\t\t\t\t\tLoading...\n\n"); 
    printf("\t\t\t\t\t"); 
  	
	int i;
    for (i = 0; i < 26; i++)
		printf("%c", a); 
  
    printf("\r"); 
    printf("\t\t\t\t\t"); 
  
    for (i = 0; i < 26; i++) { 
        printf("%c", b);
        fflush(stdout);
        Sleep(1000); 
    } 

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
	else 											 return 	   "";
}

void showResults() {
	
}

DWORD WINAPI windowsSearch(LPVOID lpParam) {
	
	struct SearchParameters* searchParams = (struct SearchParameters*)lpParam;
	const char* path = searchParams->path;
    const char* fName = searchParams->fName;
	
	char searchPath[MAX_PATH];
	snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
	
	WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile(searchPath, &findFileData);
    
	if (hFind == INVALID_HANDLE_VALUE) {
		// printf("FindFirstFile failed (%d)\n", GetLastError());
		return;
	}

	do {
		if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
    		// Skip '.' and '..' directories
			if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
	            char subDirectory[MAX_PATH];
	            if (searchPath[strlen(searchPath)-1] == '*') searchPath[strlen(searchPath)-1] = '\0';
	            snprintf(subDirectory, sizeof(subDirectory), "%s\\%s", searchPath, findFileData.cFileName);
	            searchParams->path = subDirectory;
	            searchParams->fName = fName;
	            windowsSearch(searchParams);
	        }
        } else {
            // Check if the file matches the search criteria
            if (strcmp(findFileData.cFileName, fName) == 0) {
				printf("Found: %s \n", searchPath);
            }
        }
        
        
    } while (FindNextFile(hFind, &findFileData) != 0);
	
	
	DWORD dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES) {
        printf("FindNextFile failed (%d)\n", dwError);
    }
	
	FindClose(hFind);
	free((void *)path);
	SetEvent(hSearchFinishedEvent);
    return 0;
    
}

void search() {
	char fName[32];
	char *path;
	positionText(5);
	printf("What file are you looking for today? \n");
	positionText(5);
	scanf("%s", fName);
    
	int i;
	struct SearchParameters searchParams;
	hSearchFinishedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	for (i=0; i<3; i++) {
		path = checkDrive(DRIVES[i]);
		if (path[0] != '\0') {
			searchParams.path = path;
    		searchParams.fName = fName;
    		HANDLE hSearchThread = CreateThread(NULL, 0, windowsSearch, &searchParams, 0, NULL);
    		if (hSearchThread == NULL) return 1;
			loadingBar();
			WaitForSingleObject(hSearchFinishedEvent, INFINITE);
			CloseHandle(hSearchFinishedEvent);
    		CloseHandle(hSearchThread);
			//windowsSearch(path, fName);
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
