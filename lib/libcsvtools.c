#include "csvtools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * int csvquotation(char* csvLine)
 * check for number of quotation marks in the given line and returns zero if it's even
 * returns 1 if it is oded
 * int csvobjnum(char* csvLine,char csvDelimiter)
 * returns the amount of objects in the line with the given delimiter
 * char* csvcheckdel(char* csvLine)
 * check for special characters and returning the most likly delimiter
 * char* csvgetheader(char* csvLine)
 * returns an array of string which contains the words for the line without the delimiter
 * char* csvgetheader(char* csvLine, char* csvDelimiter)
 * returns an array of string which contains the works for the line without the delimiter
 *
 */

void Print_csvtools()
{
	printf("you are working with csvtools\n");
}

void chomp(char *s)
{
	s[strcspn ( s, "\n" )] = '\0';
	setbuf(stdin,NULL);
}

const int csvobjnum(char *csvLine,char delimiter,int *quotation_val) {
	int i=0,objnum=0;
	*quotation_val=0;
	for (i=0;i<(strlen(csvLine)-1);i++) {
		if((csvLine[i] == '\"') || (csvLine[i] == '\'')) {
                        if(*quotation_val == 0)
                                *quotation_val=1;
                        else if (*quotation_val == 1)
                                *quotation_val=0;
                }

		if ((csvLine[i] == delimiter) && (*quotation_val == 0))
			objnum++;
	}
	return objnum;
}

void nospaces(char* csvLine) {

	int i=0;
	for (i=0;i<strlen(csvLine);i++)
		if (csvLine[i] == ' ') {
			fprintf(stderr,"Spaces found in the first Line, aborting...\n");
			exit(5);
		}
}

void rplccomma(char* csvLine,char delimiter) {
	int i=0;
	for (i=0;i<strlen(csvLine);i++)
		if (csvLine[i] == delimiter)
			csvLine[i] = ',';
}


int firstlinetst (char* firstLine,int vopt,char col_seperator,int *quot_val) {
        int colnum=0;
        nospaces(firstLine);

	if (vopt == 1)

                        if (col_seperator != ',') {
                                colnum = csvobjnum(firstLine,col_seperator,quot_val);
                                rplccomma(firstLine,col_seperator);
                        }

                        else {
                                colnum = csvobjnum(firstLine,',',quot_val);
                        }

                        if(*quot_val == 1) {
                                fprintf(stderr,"error, you have oded quotation marks in the first row\n");
                                exit(4);
                        }

        return colnum;

}

