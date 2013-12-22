#include <stdio.h>
#include <sqlite3.h>
#include <unistd.h>
#include <string.h>
#include "csvtools.h"
#include <stdlib.h>

static int callback(void *data,int argc,char *argv[],char **colname) {

	char **sql_res = (char **)data;
	*sql_res = calloc(strlen(argv[0]),sizeof(char));
	strcpy(*sql_res,argv[0]);
	return 0;
}

static int print_schema(void *data,int argc,char *argv[],char **colname) {

	return 0;
}

void Help(int exitcode)
{
	printf("Usage:  csv2sqlite\n");
	printf("\t\t -v - Be Verbose when running the command\n");
	printf("\t\t -s - the seperator between the columns (Default: \",\") \n");
	printf("\t\t -h - Display this manu\n");
	printf("\t\t -f - set The CSV file to read from (Default: stdin)\n");
	printf("\t\t -o - outfile name of the sqlite database (mandatory)\n");
	printf("\t\t -c - Column's names - set the columns name (Example: 'col1,col2,col3') \n");
	printf("\t\t -t - Destantion Table Name (mandatory)\n");
	printf("\t\t -l - list current table schema and exit \n");
	printf(" \n");

	exit(exitcode);
}

void exitnicely(sqlite3 *conn) {

	sqlite3_close(conn);
	exit(1);
}

int tablecheck(char *tname,sqlite3 *conn) {

	int t_res=0;
	char *Sqlerrmsg=NULL;
	char *tsql_stmt=NULL;
	char *data=NULL;

	tsql_stmt = calloc((strlen(tname)+70),sizeof(char));
	strcpy(tsql_stmt,"SELECT count(*) FROM sqlite_master WHERE type='table' AND name='");
	strcat(tsql_stmt,tname);
	strcat(tsql_stmt,"\'");

	//running the query
	t_res = sqlite3_exec(conn,tsql_stmt, callback,&data, &Sqlerrmsg);
	
	if ( t_res != SQLITE_OK ) {
			fprintf(stderr, "SQL error: %s\n", Sqlerrmsg);
	//		exitnicely(conn);
	}

	else {
		if (atoi(data) != 1) {
			fprintf(stderr,"unable to locate the table in the database\n");
			t_res = 1;
		}
		else
			t_res = 0;
	}

	free(data);
	free(Sqlerrmsg);
	free(tsql_stmt);
	return t_res;
}

int col_check(sqlite3 *conn,char *col_stmt,char *tname,int vopt,int *quot_val) {

	int i=0,rcode=0;
	int res=0;
	char *csql_stmt=NULL;
	char *ErrMesg=NULL;

	csql_stmt = (char *)calloc((strlen(tname)+strlen(col_stmt)+30),sizeof(char));
	strcpy(csql_stmt,"select ");
	strcat(csql_stmt,col_stmt);
	strcat(csql_stmt," from ");
	strcat(csql_stmt,tname);
	strcat(csql_stmt," limit 1");

	res = sqlite3_exec(conn,csql_stmt,0,0,&ErrMesg);

	if ( res != SQLITE_OK ) {
                        fprintf(stderr, "SQL error: %s\n", ErrMesg);
	//		exitnicely(conn);
			rcode=1;
        }	
	
	//free(data);
	free(ErrMesg);
	free(csql_stmt);
	return rcode;

}

int main(int argc,char *argv[]) {

	sqlite3 *conn;
	int exline,res=0,linenum=0;
	int rcode=0,colnum=0,quot_val=0;
	int i=0,c=0,k=0,vopt=0,mopt=0;
	char col_seperator=',';
	char buff[MAX_LENGTH];
	char *collist=NULL,*sql_q=NULL;;
	char *col_stmt=NULL,*tname=NULL,*ofile=NULL;
	FILE *ifile=NULL;

	while (( c = getopt (argc, argv , "vs:hf:o:lc:t:y")) != -1 ) {

		switch (c) {
			
			case 'v':
				vopt=1;
			break;

			case 's':
				col_seperator = optarg[0];
			break;

			case 'h':
				Help(0);
			break;

			case 'f':
				if (!(ifile = fopen(optarg,"rt"))) {
					fprintf(stderr,"the file \"%s\" can not be opened\n",optarg);
					exit(1);

				}
			break;
						
			case 'o':
				ofile  = optarg;
			break;

			case 'c':
				col_stmt = optarg;
				mopt++;
			break;

			case 't':
				tname = optarg;
			break;

			case 'l':
				
				mopt++;;
			break;

			case '?':
				if ( (optopt == 'f') || (optopt == 'o') || (optopt == 'c') || (optopt == 's') ||
					(optopt == 't') ) 
				{
					fprintf (stderr," " "Option -%c requires an argument.\n", optopt);
				}
				
				else if (isprint(optopt))
					fprintf(stderr,"Unknown option `-%c'.\n", optopt);

				else {
					fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
					exit(EXIT_FAILURE);
				}

			default:
				Help(1);
			break;
		}
	}


	// checking for environment variables
	
	if ( mopt != 1) {
		fprintf(stderr,"-c / -g - only one option should be specified\n");
		Help(1);
	}
	
	if ( (!getenv("CSV2LITECOLS")) && (!col_stmt) ) {
		
		fprintf(stderr,"a columns statment was not provided ... exiting\n");
		Help(2);

	}

	else if (getenv("CSV2LITECOLS"))
		col_stmt = getenv("CSV2LITECOLS");

	if ( (!getenv("CSV2LITEOUTPUT")) && (!ofile) ) {
	
		fprintf(stderr,"error - no output file specified\n");
		Help(2);

	}

	else if (getenv("CSV2LITEOUTPUT"))
		ofile = getenv("CSV2LITEOUTPUT");

	else {
		res = sqlite3_open(ofile,&conn);
		if (res != 0)
			fprintf(stderr,"the file \"%s\" can not be opened or writen\n",ofile);
	}

	
	if ( (!getenv("CSV2LITETABLE")) && (!tname) ) {

		fprintf(stderr,"error - destination table name not set\n");
		Help(2);
	}

	else if (getenv("CSV2LITETABLE"))
			tname = getenv("CSV2LITETABLE");
	



	if (tablecheck(tname,conn) != 0 ) {
			fprintf(stderr,"the table \"%s\" does not exist\n",tname);
			exitnicely(conn);
	}

	else {
		nospaces(col_stmt);

		if (col_seperator != ',')
			rplccomma(col_stmt,col_seperator);

		if (!col_check(conn,col_stmt,tname,vopt,&quot_val)) {
		
			colnum = csvobjnum(col_stmt,',',&quot_val);
			if (vopt == 1)
				printf("the columns (%d) are consistant with the table \"%s\"\n",colnum,tname);
		}
	}

	
	/*
 	 * checking if the input file is STDIN or a FILE
 	 */


	if (ifile == NULL) {
		ifile = stdin;
		if(vopt == 1)
			printf("listening to STDIN for input\n");
	}

	
	while( ((fgets(buff,MAX_LENGTH,ifile)) != NULL) ) {


		/*
                 * checking the input lines to see if they are consistant with
 		 * the CSV RFC and inserting those lines in to the database's
 		 * table
 		 */

		if (collist == NULL ) {
			collist = (char *)calloc((strlen(buff)+(colnum*2)+1),sizeof(char));
			k = colnum;
			exline=0;
			collist[0] = '\'';
			i=1;
                }

                else {
			collist = (char *)realloc(collist,((strlen(collist)+strlen(buff)+1)*sizeof(char)));
		}


		for (c=0;c<strlen(buff);c++) {


			if (k < 0) {
				fprintf(stderr,"error - the number of columns is to much at line %d \n",linenum);
				rcode=5;
				break;
			}

			if ((buff[c] == col_seperator) && (quot_val == 0)) {

				collist[i] = '\'';
				i++;
				collist[i] = ',';
				i++;
				collist[i] = '\'';
				k--;
				i++;
			}

                                else if ((buff[c] == '\"') || (buff[c] == '\'')) {
                                        if(quot_val == 0)
                                                quot_val=1;
                                        else if (quot_val == 1)
                                                quot_val=0;
                                }

                                else if ( buff[c] == '\n' ) {
                                        collist[i] = '\'';
                                        i++;
                                }

                                else {
                                        collist[i] = buff[c];
                                        i++;
                                }

		}

		if (quot_val == 0) {
			
			sql_q = (char *)calloc((strlen(collist)+strlen(tname)+strlen(col_stmt)+150),sizeof(char));
			strcpy(sql_q,"insert into ");
			strcat(sql_q,tname);
			strcat(sql_q," (");
			strcat(sql_q,col_stmt);
			strcat(sql_q,") values(");
			strcat(sql_q,collist);
			strcat(sql_q,")");
			
			/*
 			 * this is the insert the CSV line into the database
 			*/

			res = sqlite3_exec(conn,sql_q, 0,0, 0);

			if ( res != SQLITE_OK ) {
                        	fprintf(stderr, "SQL error: problem inserting line number %d\n",linenum);
			//	exitnicely(conn);
				rcode=8;
				break;
			}

			free(collist);
			free(sql_q);
			collist = NULL;
			sql_q = NULL;
		}

		else {
			if (exline ==0)
				exline++;
			else {
				fprintf(stderr,"error - Invalid single quotation use in line %d\n",linenum);
				rcode=7;
				break;
			}
		}

		linenum++;
		memset(buff,'\0',0);

	}

	fclose(ifile);
	// closing the sqlite connection
	sqlite3_close(conn);
	return rcode;
}
