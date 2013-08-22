#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include "csvtools.h"
#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>



#define MAX_LENGTH 2048


int firstlinetst (char* firstLine,int vopt,char col_seperator,int *quot_val) {
 	int colnum=0;
	nospaces(firstLine);
		
               if(vopt == 1)
                     printf("this is the first line\n");


                        if (col_seperator != ',') {
                                colnum = csvobjnum(firstLine,col_seperator,quot_val);
                                rplccomma(firstLine,col_seperator);
                        }

                        else {
                                colnum = csvobjnum(firstLine,',',quot_val);
                        }

                        printf("the value of quot_val is %d\n",*quot_val);

                        if(*quot_val == 1) {
                                fprintf(stderr,"error, you have oded quotation marks in the first row\n");
                                exit(4);
                        }
	return colnum;

}

void Help() {

	printf("Usage:  csv2odbc\n");
	printf("\t\t -v - Be Verbose when running the command\n");
	printf("\t\t -s - the seperator between the columns (Default \",\") \n");
	printf("\t\t -h - Display this manu\n");
	printf("\t\t -f - set The CSV file to read from (Default stdin)\n");
	printf("\t\t -d - DSN name - set by the odbc.ini file\n");
	printf("\t\t -t - Destantion Table Name\n");
	printf("\n");

}

int main(int argc,char *argv[]) {

	int c=0,dopt=0,topt=0,fopt=0,vopt=0;
	int quot_val=0;
	FILE *ifile = NULL;
	char *dname = NULL,buff[MAX_LENGTH];
	char *tname=NULL,*sql_q=NULL;
	char *col_stmt=NULL;
	char col_seperator=',';
	char *lenbuff=NULL;
	int linenum=0,colnum=0;
	opterr = 0;
	SQLHDBC dbc;
	SQLHENV env;
	SQLHSTMT stmt;
	SQLRETURN ret;
	SQLRETURN sql_ret;
	
	 while ((c = getopt (argc, argv, "hs:vf:d:t:c:")) != -1)
		switch (c) {

			case 'h':
				Help();
				exit(0);
			break;

			case 'c':
				col_stmt = optarg;
			break;

			case 's':
				 col_seperator = optarg[0];
			break;
			case 'f':
				if (ifile = fopen(optarg,"rt")){
					printf("the file %s is o.k.\n",optarg);
					fopt=1;
				}
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
			case 'v':
				vopt=1;
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

	if ((!getenv("C2ODSNAME")) && (!dname)) {
		fprintf(stderr,"missing target DS name\n");
		Help();
		exit(3);
	}
	else if (getenv("C2ODSNAME"))
			dname = getenv("C2ODSNAME");

	if ((!getenv("C2OTABLENAME")) && (!tname)) {
		fprintf(stderr,"missing target table name\n");
		Help();
		exit(2);
	}
	else if (getenv("C2OTABLENAME"))
			tname = getenv("C2OTABLENAME");
	
	// trying to connect to the Database 
	

        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
        SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
        SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);


        SQLCHAR dsnstr[30] = "DSN=";
        strcat(dsnstr,dname);
        strcat(dsnstr,";");

	if(vopt == 1)
		printf("trying to connect to %s with odbc\n",dname);
        ret = SQLDriverConnect(dbc, NULL, dsnstr, SQL_NTS,
                                NULL, 0, NULL, SQL_DRIVER_COMPLETE);

        if (!SQL_SUCCEEDED(ret)) {
                fprintf(stderr,"problem connecting to DSN - \"%s\" \n",dname);
                exit(2);
        }
	else {
		if(vopt == 1)
			printf("the SQL connection is looking good\n");
	}
	ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

	/// checking if a file has been select or to start listening to STDIN
	
	if(fopt == 0) {
		ifile = stdin;
		if(vopt == 1)
			printf("listening to STDIN for input\n");
	}

	// checking if the columns names are from the -c option
	
	if (col_stmt != NULL) {
		colnum = firstlinetst(col_stmt,vopt,col_seperator,&quot_val);
		sql_q = (char *)malloc(sizeof(sql_q) * ( strlen(tname) * strlen(col_stmt)));
                strncpy(sql_q,"select ",sizeof(sql_q));
                strcat(sql_q,col_stmt);
                strcat(sql_q," from ");
                strcat(sql_q,tname);
                strcat(sql_q," limit 1");
	
		sql_ret = SQLExecDirect(stmt, sql_q , SQL_NTS);
                     if (!SQL_SUCCEEDED(sql_ret)) {
                             fprintf(stderr,"problem with the first row of the clumns names\n");
                              exit(7);
                     }
                     if(vopt == 1)
                             printf("the clumns are consistant with the table\n");

                     if(vopt == 1)
                             printf("the columns count is %d\n",colnum);
	}

	while( ((fgets(buff,MAX_LENGTH,ifile)) != NULL) ) {

		if(strlen(buff)<2)
			exit(0);

		chomp(buff);

		// handling the first line to extract the columns names 
		// from the CSV file
		
		if ( (linenum == 0) && (col_stmt == NULL)) {

			colnum = firstlinetst(buff,vopt,col_seperator,&quot_val);
			col_stmt = buff;
			sql_q = (char *)malloc(sizeof(sql_q) * ( strlen(tname) * strlen(col_stmt)));
	                strncpy(sql_q,"select ",sizeof(sql_q));
        	        strcat(sql_q,col_stmt);
                	strcat(sql_q," from ");
                 	strcat(sql_q,tname);
                	strcat(sql_q," limit 1");

			sql_ret = SQLExecDirect(stmt, sql_q , SQL_NTS);
                       	if (!SQL_SUCCEEDED(sql_ret)) {
                               	fprintf(stderr,"problem with the first row of the clumns names\n");
                               	exit(7);
			}
			if(vopt == 1)
                       		printf("the clumns are consistant with the table\n");

			if(vopt == 1)
				printf("the columns count is %d\n",colnum);

		}

		if(vopt == 1)
			printf("%s\n",buff);
		linenum++;
	}

	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  	SQLFreeHandle(SQL_HANDLE_ENV, env);
        free(lenbuff);
        free(sql_q);

	return 0;
}
