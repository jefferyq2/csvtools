/*
 * the fallowing application will read a CSV file according to the
 * CSV RFC.
 * the application will first check the if the info is CSV competable and 
 * the will insert each row to the postgresql Database
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <libpq-fe.h>
#include <unistd.h>
#include "csvtools.h"


void Help() {

	printf("Usage:  csv2pgsql\n");
	printf("\t\t -v - Be Verbose when running the command\n");
	printf("\t\t -s - the seperator between the columns (Default: \",\") \n");
	printf("\t\t -h - Display this manu\n");
	printf("\t\t -f - set The CSV file to read from (Default: stdin)\n");
	printf("\t\t -H - set the postgresql Server hostname (Default: localhost)\n");
	printf("\t\t -u - set the postgresql user login (Default: none)\n");
	printf("\t\t -p - set the postgresql user Password (Default: none)\n");
	printf("\t\t -d - set the postgresql Database name\n");
	printf("\t\t -c - Column's names - set the columns name (Defualt: First Line)\n");
	printf("\t\t -t - Destantion Table Name\n");
	printf("\n");

}


void exitnacely(PGconn *conn) {

	PQfinish(conn);
	exit(1);
}

void tablecheck(char *col_stmt,char *tpgname,PGresult *res,PGconn *conn,int vopt) {

char *tsql_q;

	PQclear(res);
        tsql_q = (char *)calloc((strlen(tpgname) + strlen(col_stmt) + 25),sizeof(char));
        strncpy(tsql_q,"select ",sizeof(tsql_q));
        strcat(tsql_q,col_stmt);
        strcat(tsql_q," from ");
        strcat(tsql_q,tpgname);
        strcat(tsql_q," limit 1");

	res = PQexecParams(conn,tsql_q,0,NULL,NULL,NULL,NULL,1);
	if (PQresultStatus(res) != PGRES_TUPLES_OK)
    	{
		printf("problem with the columns names\n");
		if (vopt == 1)
        		fprintf(stderr, "select table test failed: %s\n", PQerrorMessage(conn));
        	PQclear(res);
        	exitnacely(conn);
	}
	else if (vopt == 1)
		printf("columns test successful \n");

 free(tsql_q);
}

int main(int argc,char *argv[]) {

	char buff[MAX_LENGTH];
	int i=0,k=0,c,rcode=0,vopt=0,fopt=0,quot_val=0;
	int colnum=0,linenum=0,exline=0;
	char *col_stmt=NULL;
	char col_seperator=',';
	char *sql_q=NULL;
	char *dbuser=NULL,*dbpass=NULL;
	char *dbhost=NULL,*dbname=NULL;
	char *tpgname=NULL;
	FILE *ifile = NULL;
	char *conninfo=NULL,*collist=NULL;
	PGconn     *conn;
	PGresult   *res;
	PGnotify   *notify;

	while ((c = getopt (argc, argv, "hs:vf:u:p:H:d:t:c:")) != -1)
		switch (c) {
			case 'h':
				Help();
			break;

			case 'c':
				col_stmt = optarg;
			break;

			case 's':
				col_seperator=optarg[0];
			break;
			
			case 't':
				tpgname = optarg;
			break;

			case 'v':
				vopt=1;
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

			case 'u':
				dbuser = optarg;
			break;

			case 'p':
				dbpass = optarg;
			break;

			case 'H':
				dbhost = optarg;
			break;

			case 'd':
				dbname = optarg;
			break;

			case '?':
                        if ( (optopt == 'f') || (optopt == 'd') || (optopt == 't') || (optopt == 'u')
				|| (optopt == 'p') || (optopt == 'H') || (optopt == 's') || (optopt == 'c') )
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

			break;

			default:
				Help();
				exit(1);		
			break;
		}

	

	if ( (!getenv("PGDATABASE")) && (!dbname) ) {
		fprintf(stderr,"error - missing target DataBase name\n\n");
                Help();
                exit(3);

	}

	else if (getenv("PGDATABASE")) 
		dbname = getenv("PGDATABASE");

	if ( (!getenv("CSV2PGTABLE") ) && (!tpgname) ) {
		fprintf(stderr,"error - there is no destination table specified\n\n");
		Help();
		exit(4);
	}

	else if (getenv("CSV2PGTABLE"))
			tpgname = getenv("CSV2PGTABLE");

	if (getenv("PGUSER"))
		dbuser = getenv("PGUSER");

	if (getenv("PGPASSWORD"))
		dbpass = getenv("PGPASSWORD");

	if (getenv("PGHOST"))
		dbhost = getenv("PGHOST");

	if (getenv("PGHOSTADDR"))
		dbhost = getenv("PGHOSTADDR");

	// trying the connect to the PostgresSQL server
	// this is working with a standard PostgreSQL C API
	
	conninfo = calloc((strlen(dbname)+10),sizeof(char));
	strcpy(conninfo,"dbname = ");
	strcat(conninfo,dbname);

	if(dbhost) {
		conninfo = realloc(conninfo,((strlen(conninfo)+strlen(dbhost)+10)*sizeof(char)));
		strcat(conninfo," host = ");
		strcat(conninfo,dbhost);
	}

	if (dbuser) {
		conninfo = realloc(conninfo,((strlen(conninfo)+strlen(dbuser)+10)*sizeof(char)));
		strcat(conninfo," user = ");
		strcat(conninfo,dbuser);

		if (!dbpass) {
			dbpass = getpass("insert password: ");
		}

		conninfo = realloc(conninfo,((strlen(conninfo)+strlen(dbpass)+15)*sizeof(char)));
		strcat(conninfo," password = ");
		strcat(conninfo,dbpass);
	}

	/* Make a connection to the database */
    	conn = PQconnectdb(conninfo);

	/* Check to see that the backend connection was successfully made */
	if (PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr, "error - Connection to database failed: %s\n",PQerrorMessage(conn));
		exitnacely(conn);
	}
	else if (vopt == 1)
		printf("the connection to PostgreSQL Database was successful\n");
	
	PQclear(res);
	res = PQexec(conn, "BEGIN");
	if (PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		fprintf(stderr, "error - BEGIN command failed: %s\n", PQerrorMessage(conn));
		PQclear(res);
		exitnacely(conn);		
	}

	if(fopt == 0) {
		ifile = stdin;
		if(vopt == 1)
			printf("listening to STDIN for input\n");
        }

        // checking if the columns names are from the -c option
        //
        
	
        if (col_stmt != NULL) {
                colnum = firstlinetst(col_stmt,vopt,col_seperator,&quot_val);
		tablecheck(col_stmt,tpgname,res,conn,vopt);
        }

	while( ((fgets(buff,MAX_LENGTH,ifile)) != NULL) ) {

		if (strlen(buff) < (colnum+1))
                        break;

		if ( (linenum == 0) && (col_stmt == NULL)) {

                        chomp(buff);
                        colnum = firstlinetst(buff,vopt,col_seperator,&quot_val);
                        col_stmt = (char *)calloc(strlen(buff),sizeof(char));
                        strcpy(col_stmt,buff);
			tablecheck(col_stmt,tpgname,res,conn,vopt);
			continue;
		}

		/* handling the rest of the lines by going over the line
 		 * checking that are referring to the CSV RFC
 		 */
		
		if (collist == NULL ) {
                        collist = (char *)calloc((strlen(buff)+(colnum*2)+1),sizeof(char));
                        k = colnum;
                }
                else {
                        collist = (char *)realloc(collist,((strlen(collist)+strlen(buff)+1)*sizeof(char)));
                }

                if (quot_val == 0) {
                        collist[0] = '\'';
                        i=1;
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


			sql_q = (char *)calloc((strlen(collist)+strlen(tpgname)+strlen(col_stmt)+200),sizeof(char));
			strcpy(sql_q,"insert into ");
                        strcat(sql_q,tpgname);
                        strcat(sql_q," (");
                        strcat(sql_q,col_stmt);
                        strcat(sql_q,") values(");
                        strcat(sql_q,collist);
                        strcat(sql_q,")");
			

			// this is the insert the CSV line into the database
			res = PQexec(conn,sql_q);
			if (PQresultStatus(res) != PGRES_COMMAND_OK) {
				fprintf(stderr, "inserting of fields failed: %s", PQerrorMessage(conn));
				PQclear(res);
				exitnacely(conn);
			}
			else {
				PQclear(res);
			}

			if (vopt == 1)
				printf("the sql : %s\n",sql_q);

			free(collist);
                        free(sql_q);
                        collist = NULL;
			sql_q=NULL;

		}
		
		else {
			if (exline == 0)
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
	/* end the transaction */
	res = PQexec(conn, "END");
	PQclear(res);

	/* close the connection to the database and cleanup */
	PQfinish(conn);
	
	free(conninfo);
	return rcode; 
}
