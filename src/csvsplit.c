/*
 * the fallowing application will read a CSV file according to the
 * CSV RFC.
 * the application will first check the if the info is CSV competable then
 * it will split the source files to X number of files
 * the csvtools is an open source project for parsing csv files
 * into multiple databases for the Unix/Linux platform
 * Copyright (C) yyyy  Oren Oichman
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Gnomovision version 1, Copyright (C) Oren Oichman
 * Gnomovision comes with ABSOLUTELY NO WARRANTY; for details
 * type `csvsplit -h'.  This is free software, and you are welcome
 * to redistribute it under certain conditions; type `canoectl -h' 
 * for details.
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include "csvtools.h"


typedef struct {
	unsigned int ver_opt:1;
	unsigned int ifile_opt:1;
	unsigned int ofile_opt:1;
	unsigned int num_opt:1;
	unsigned int alph_opt:1;
	unsigned int incl_opt:1;
	unsigned int line_opt:1;
	unsigned int size_opt:1;
	unsigned int quot_val:1;
} arg_options;


void Help(int rcode) {

        printf("Usage:  csvsplit\n");
        printf("\t\t -v - Be Verbose when running the command\n");
        printf("\t\t -s - -s=CSVS_SEPERATOR\n\
		\tthe seperator between the columns (Default: \",\") \n");
        printf("\t\t -h - Display this manu\n");
        printf("\t\t -n - use a Numeric Prefix for the new files (Default) \n");
        printf("\t\t -a - use an Alphabetical Prefix for the new files\n");
        printf("\t\t -f - set The CSV file to read from (Mandatory)\n");
	printf("\t\t -o - -o=CSVS_OUTPUT\n\
		\tset the output files name before the prefix (Default: original file name)\n");
	printf("\t\t -i - -i=CSVS_COLUMNS_INCLUDE\n\
		\tinclude the columns names in each new file (Default: no)\n");
	printf("\t\t -l - -l=CSVS_LINE_NUMBER\n\
		\tnumber of lines for each file (Default: 10,000)\n");
	printf("\t\t -z - -z=CSVS_FILE_SIZE\n\
		\tSplit to new files by file size(in Megabyte)\n");
        printf("\t\t -c - -c=CSVS_COLUMNS_NAME\n\
		\tColumn's names - set the columns name (Defualt: First Line)\n");
        printf("\n");

	exit(rcode);
}

int main(int argc,char *argv[]) {

	int c,rcode=0,fsize=0;
	long int numLines=10000;
	char seperator=NULL;
	char *col_stmt=NULL,*opName=NULL;
	FILE *ofile=NULL, *ifile=NULL;

	arg_options argopts = { 0 , 0, 0, 0, 0, 0, 0, 0, 0 };

	while ((c = getopt(argc, argv, "vs:hnaf:o:il:z:c:")) != -1 ) 
		switch(c) {
			case 'v':
				argopts.ver_opt = 1;
			break;
			
			case 's':
				seperator = optarg;
			break;

			case 'h':
				Help(0);
			break;

			case 'n':
				argopts.num_opt = 1;
			break;

			case 'a':
				argopts.alph_opt = 1;
			break;

			case 'f':

				if (ifile = fopen(optarg,"rt")){
					printf("the file %s is o.k.\n",optarg);
					argopts.ifile_opt = 1;
				}
				else {
					fprintf(stderr,"the file \"%s\" can not be opened\n",optarg);
					exit(1);
				}

			break;

			case 'o':

				if (ofile = fopen(optarg,"wt")){
					printf("the file %s is o.k.\n",optarg);
					argopts.ofile_opt = 1;
				}
				else {
					fprintf(stderr,"the file \"%s\" can not be written\n",optarg);
					exit(1);
				}

			break;
		
			case 'i':
				argopts.incl_opt = 1;
			break;

			case 'l':
				argopts.line_opt =1;
				numLines = atoi(optarg);
			break;

			case 'z':
				argopts.size_opt =1;
				fsize = atoi(optarg);
			break;
			
			case 'c':
				col_stmt = optarg;
			break;	

			default:
				Help(1);
			break;
		}

	if ( argopts.ifile_opt == 0) {
		fprintf(stderr,"an input file must be specified\n");
		Help(1);
	}

	if ( (argopts.num_opt == 1) && (argopts.alph_opt == 1 ) ) {
		fprintf(stderr,"only one of the arguments should be specified ( -n  \\ -a) \n");
		Help(2);
	}
	
   return rcode;
}
