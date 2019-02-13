/*
Min Kim
uniq.c
Intro to OS / 2019 Spring
NOTES: WORKS WITH "%d" --> was changed to "%4d" in line 53, following piazzas permission, but needs correct printf to work 
*/

#include "types.h"
#include "user.h"

int compareLines(char *lineOne, char *lineTwo, int flagI) {
	if (lineOne[0] == '\0') {		//fixes common error with line formatting
		return 1;
	}
	if (flagI) {
		int firstLen = strlen(lineOne);
		int secLen = strlen(lineTwo);
		if (firstLen == secLen) {  //uppercase all letters
			int index = 0;
			for (; index < firstLen; ++index) {		//checks the characters individually as uppercase
				char firstChar = lineOne[index];
				char secChar = lineTwo[index];
				if (firstChar >= 'a' && firstChar <= 'z') {	//turn into uppercase
					firstChar -= 32;
				}
				if (secChar >= 'a' && firstChar <= 'z') {
					secChar -= 32;
				}
				if (firstChar != secChar) {
					return 1;
				}
			}
			return 0;

		}
		else {
			return 1;
		}
	}
	else {
		if (strcmp(lineOne, lineTwo) == 0) {
			return 0;		//equal lines
		}
		else {
			return 1;		//not equal
		}
	}
}

void printLine(char *line, char flagC, char flagD, int counter) {
	if (strlen(line) > 0) {
		if (flagC) {
			printf(1, "%4d ", counter);
		}
		if (flagD && counter > 1) {
			printf(1, "%s\n", line);
		}
		else if (!flagD) {
			printf(1, "%s\n", line);
		}
	}
}

void uniq(int fd, char flagC, char flagD, char flagI) {
	int bufAmount = 512;	//number of bytes--will be increased if needed
	char *lastLine = malloc(sizeof(char) * bufAmount);
	char *newLine = malloc(sizeof(char) * bufAmount);
	int currInd = 0; //index of newLine
	lastLine[0] = '\0';
	char buf[bufAmount];
	int charCount;
	int counter = 0;
	while ((charCount = read(fd, buf, sizeof(buf))) > 0) {
		int readInd = 0; //index of text file
		while (readInd < charCount) {
			while (buf[readInd] != '\r') {	//while line is not finished
				if (readInd >= charCount) {
					break;
				}
				if (currInd >= (bufAmount - 1)) {	//if we need to read more than 512 chracters-- double the size of memory allocated
					bufAmount *= 2;
					char *increasedLine = malloc(sizeof(char)*bufAmount);
					strcpy(increasedLine, newLine);
					free(newLine);
					newLine = increasedLine;
				}

				newLine[currInd] = buf[readInd];
				currInd += 1;
				readInd += 1;
			}
			readInd += 1;

			if (buf[readInd] == '\n') {		//found new line
				newLine[currInd] = '\0';
				currInd = 0;
				int areIdentical = compareLines(lastLine, newLine, flagI);
				if (areIdentical == 0) {
					counter += 1;
				}
				else {
					printLine(lastLine, flagC, flagD, counter);
					if (sizeof(lastLine) < sizeof(newLine)) {
						free(lastLine);
						lastLine = malloc(sizeof(char) * strlen(newLine) + 1);	//do i need a plus 1 here?
					}
					strcpy(lastLine, newLine);
					counter = 1;
				}
				readInd += 1;
			}
		}
	}

	if (charCount < 0) {
		printf(1, "uniq: read error\n");
		free(lastLine);
		free(newLine);
		close(fd);
		exit();
	}


	int areIdentical = compareLines(lastLine, newLine, flagI);
	if (areIdentical == 0) {
		counter += 1;
		printLine(lastLine, flagC, flagD, counter);
	}
	else {
		printLine(lastLine, flagC, flagD, counter);
		counter = 1;
		printLine(newLine, flagC, flagD, counter);
	}
	free(lastLine);
	free(newLine);
}

int main(int argc, char *argv[]) {
	char *file = "";
	char flagC, flagD, flagI = 0;
	int fd;

	if (argc <= 1) {
		uniq(0, 0, 0, 0);
		exit();
	}

	int index = 1;
	for (; index < argc; index++) {
		if (strcmp(argv[index], "-c") == 0) {
			flagC = 1;
		}
		else if (strcmp(argv[index], "-d") == 0) {
			flagD = 1;
		}
		else if (strcmp(argv[index], "-i") == 0) {
			flagI = 1;
		}
		else {
			file = argv[index];
		}
	}

	if (flagC && flagD) {
		printf(2, "-c and -d cannot happen at the same time\n");
		exit();
	}

	if (strlen(file) == 0) {
		uniq(0, flagC, flagD, flagI);
		exit();
	}

	if ((fd = open(file, 0)) < 0) {
		printf(2, "cannot open file: %s\n", file);
		exit();
	}

	uniq(fd, flagC, flagD, flagI);
	close(fd);
	exit();
}
