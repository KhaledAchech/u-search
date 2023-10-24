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

void search() {
	char *fName;
	positionText(5);
	printf("What file are you looking for today? \n");
	positionText(5);
	scanf("%s", fName);
	
	int i;
	char searchCommand[256];
	for (i=0; i<3; i++) {
		if (checkDrive(DRIVES[i])) {
			sprintf(searchCommand, "dir %c:\\%s /s /b /a-d", DRIVES[i], fName);
			system(searchCommand);
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
