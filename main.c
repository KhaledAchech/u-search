#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

const char DRIVES[4] = {'C','D','G','\0'};
pthread_mutex_t lock;
int progress = 0;

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
		usleep(1000000);
	}
	printf("\nTask completed!\n");
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
		pthread_mutex_lock(&lock);
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
				progress += 10;
            	printf("\n");
            	printf("\n");
            }
		}
		pthread_mutex_unlock(&lock);
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
	pthread_t loadingThread;
    pthread_mutex_init(&lock, NULL);

	while (1) { 
		search();
		pthread_mutex_lock(&lock);
		progress = 100;
		pthread_mutex_unlock(&lock);
		pthread_join(loadingThread, NULL);
		pthread_mutex_destroy(&lock);
		//quit();
	}
	
   	return 0;
}
