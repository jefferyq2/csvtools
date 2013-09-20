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


int main(int argc,char *argv[]) {

	int c,rcode=0,vopt=0,fopt=0,quot_val=0;
	int colnum;
	char *col_stmt=NULL;
	char col_seperator=',';
	char *dbuser=NULL,*dbpass=NULL;
	char *dbhost=NULL,*dbname=NULL;
	char *tpgname=NULL;
	FILE *ifile = NULL;
	char *conninfo;
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
		fprintf(stderr, "error - Connection to database failed: %s",PQerrorMessage(conn));
		exitnacely(conn);
	}
	else if (vopt == 1)
		printf("the connection to PostgreSQL Database was successful\n");

	if(fopt == 0) {
		ifile = stdin;
		if(vopt == 1)
			printf("listening to STDIN for input\n");
        }

        // checking if the columns names are from the -c option
        //
        
        if (col_stmt != NULL) {
                colnum = firstlinetst(col_stmt,vopt,col_seperator,&quot_val);
 //               tablecheck(col_stmt,tname,sql_ret,stmt,vopt);
        }

	
	free(conninfo);
	return rcode; 
}
