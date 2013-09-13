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

	//if(vopt == 1)
	//	printf("the number of columns is : %d \n",colnum);

	return colnum;

}

void Help() {

	printf("Usage:  csv2odbc\n");
	printf("\t\t -v - Be Verbose when running the command\n");
	printf("\t\t -s - the seperator between the columns (Default \",\") \n");
	printf("\t\t -h - Display this manu\n");
	printf("\t\t -f - set The CSV file to read from (Default stdin)\n");
	printf("\t\t -d - DSN name - set by the odbc.ini file\n");
	printf("\t\t -c - Column's names - set the columns name (Defualt: First Line)\n");
	printf("\t\t -t - Destantion Table Name\n");
	printf("\n");

}

void tablecheck(char *col_stmt,char *tname,SQLRETURN sql_ret,SQLHSTMT stmt, int vopt) {
	
char *tsql_q;

	tsql_q = (char *)malloc(sizeof(char) * ( strlen(tname) + strlen(col_stmt) + 1));
        strncpy(tsql_q,"select ",sizeof(tsql_q));
        strcat(tsql_q,col_stmt);
        strcat(tsql_q," from ");
        strcat(tsql_q,tname);
        strcat(tsql_q," limit 1");

        sql_ret = SQLExecDirect(stmt, tsql_q , SQL_NTS);
		if (!SQL_SUCCEEDED(sql_ret)) {
			fprintf(stderr,"problem with the first row of the clumns names\n");
			exit(7);
		}
		if(vopt == 1) {
			printf("the clumns are consistant with the table\n");
		//	printf("the culomns are : %s \n",col_stmt);
		}
}

int main(int argc,char *argv[]) {

	int c=0,exline=0,rcode=0,fopt=0,vopt=0;
	int quot_val=0,i=0,k=0;
	FILE *ifile = NULL;
	char *dname = NULL,buff[MAX_LENGTH];
	char *tname=NULL,*sql_q=NULL;
	char *collist=NULL;
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
			break;

			case 't':
				tname = optarg;
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
		fprintf(stderr,"Error Number 1 - missing target DS name\n\n");
		Help();
		exit(3);
	}
	else if (getenv("C2ODSNAME"))
			dname = getenv("C2ODSNAME");

	if ((!getenv("C2OTABLENAME")) && (!tname)) {
		fprintf(stderr,"Error Number 2 - missing target table name\n\n");
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
                fprintf(stderr,"Error - problem connecting to DSN - \"%s\" \n",dname);
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
		tablecheck(col_stmt,tname,sql_ret,stmt,vopt);
	}

	while( ((fgets(buff,MAX_LENGTH,ifile)) != NULL) ) {

		if (strlen(buff) < (colnum+1))
			break;

		// handling the first line to extract the columns names 
		// from the CSV file
		
		if ( (linenum == 0) && (col_stmt == NULL)) {

			chomp(buff);
			colnum = firstlinetst(buff,vopt,col_seperator,&quot_val);
			col_stmt = (char *)calloc(strlen(buff),sizeof(char));
			strcpy(col_stmt,buff);
			//col_stmt = buff;
			
			tablecheck(col_stmt,tname,sql_ret,stmt,vopt);
			continue;
		}


		if (collist == NULL ) {
			collist = (char *)calloc((strlen(buff)+(colnum*2)+1),sizeof(char));
			k = colnum;
		}
		else {
			collist = (char *)realloc(collist,((strlen(collist)+strlen(buff)+1)*sizeof(char)));
		}

		if (quot_val == 0)
			collist[0] = '\'';
			i=1;
			

		for (c=0;c<strlen(buff);c++) {

			if (k < 0) {
				fprintf(stderr,"the number of columns is to much at line %d \n",linenum);
				//exit(5);
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
	
			if (linenum == 0)
				fprintf(stdout,"the values are : \n");
			
			/* put the sql insert and query build here
 			 * do not forget to print is for testing and for 
 			 * verbose request 
 			 */

			sql_q = (char *)calloc(((strlen(collist)+strlen(col_stmt)+30)*2),sizeof(char));
			strcpy(sql_q,"insert into ");
			strcat(sql_q,tname);
			strcat(sql_q," (");
			strcat(sql_q,col_stmt);
			strcat(sql_q,") values(");
			strcat(sql_q,collist);
			strcat(sql_q,");");

			sql_ret = SQLExecDirect(stmt, sql_q , SQL_NTS);
                		if (!SQL_SUCCEEDED(sql_ret)) {
                        		fprintf(stderr,"problem inserting row number %d\n",linenum);
                        		exit(7);
                		}
                		
			if (vopt == 1)
				printf("%s\n",sql_q);


			free(collist);
			free(sql_q);
			collist = NULL;
		}
		else {
			if (exline == 0)
				exline++;
			else {
				fprintf(stderr,"Invalid single quotation use in line %d\n",linenum);
				//exit(7);
				rcode=7;
				break;
			}
			
		}

		linenum++;
		memset(buff,'\0',0);
	}

	if (vopt == 1)
		printf("the number of lines is : %d \n",linenum);

	SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  	SQLFreeHandle(SQL_HANDLE_ENV, env);
        free(lenbuff);
        free(sql_q);

	return rcode;
}
