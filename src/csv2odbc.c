#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include "libcsvtools.h"

void Help() {

	printf("Usage:  csv2odbc\n");
	printf("\t\t -h - \"Display this manu\"\n");
	printf("\t\t -f - \"The CSV file name\"\n");
	printf("\t\t -d - \"DSN name\"\n");
	printf("\t\t -t - \"Destantion Table Name\"\n");
	printf("\n");

}

int main(int argc,char *argv[]) {

	int c=0,dopt=0,topt=0;
	FILE *ifile = NULL;
	char *dname = NULL;
	char *tname=NULL;
	opterr = 0;
	
	 while ((c = getopt (argc, argv, "hf:d:t:")) != -1)
		switch (c) {

			case 'h':
				Help();
				exit(0);
			break;
			
			case 'f':
				if(ifile = fopen(optarg,"rt"))
					printf("the file %s is o.k.\n",optarg);
				else {
					fprintf(stderr,"the file \"%s\" can not be opened\n",optarg);
					exit(1);
				}
			break;
			case 'd':
				dname = optarg;
				dopt = 1;
			break;

			case 't':
				tname = optarg;
				topt = 1;
			break;
			case '?':
                        if ( (optopt == 'f') || (optopt == 'd') || (optopt == 't') )
            		{
                           fprintf (stderr," " "Option -%c requires an argument.\n", optopt);
			}
            		else if (isprint (optopt))
              		{
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			}
              		else {
                           fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                           exit(EXIT_FAILURE);
			}

			default:
				Help();
				exit(1);
			break;
				
		}
	if ((!getenv("C2OTABLENAME")) && (!tname))
		fprintf(stderr,"missing target table name\n");
	else if (getenv("C2OTABLENAME"))
			tname = getenv("C2OTABLENAME");
	
	 Print_csvtools();
	return 0;
}
