#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include "csvtools.h"
#include <mysql.h>

void Help() {

        printf("Usage:  csv2mysql\n");
        printf("\t\t -v - Be Verbose when running the command\n");
        printf("\t\t -s - the seperator between the columns (Default: \",\") \n");
        printf("\t\t -h - Display this manu\n");
        printf("\t\t -f - set The CSV file to read from (Default: stdin)\n");
        printf("\t\t -H - set the mysql Server hostname (Default: localhost)\n");
        printf("\t\t -u - set the mysql user login (Default: none)\n");
        printf("\t\t -p - set the mysql user Password (Default: none)\n");
        printf("\t\t -d - set the mysql Database name\n");
        printf("\t\t -c - Column's names - set the columns name (Defualt: First Line)\n");
        printf("\t\t -t - Destantion Table Name\n");
        printf("\n");

}

void exitnacely(MYSQL *conn) {

        mysql_close(conn);
        exit(1);

}

void tablecheck(char *col_stmt,char *tmyname,MYSQL_RES *res,MYSQL *conn,int vopt) {
	char *tsql_q;

	tsql_q = (char *)calloc((strlen(tmyname) + strlen(col_stmt) + 25),sizeof(char));
        strncpy(tsql_q,"select ",sizeof(tsql_q));
	strcat(tsql_q,col_stmt);
        strcat(tsql_q," from ");
        strcat(tsql_q,tmyname);
        strcat(tsql_q," limit 1");

	//printf("the stmt is %s\n",tsql_q);
	if (mysql_query(conn,tsql_q)) {
		fprintf(stderr,"error - problem with the table columns - %s",mysql_error(conn));
		exitnacely(conn);
	}

	else if ( vopt == 1)
		printf("the table check was successful\n");

	res = mysql_use_result(conn);
	mysql_free_result(res);
	free(tsql_q);
}

int main(int argc,char *argv[]) {
	
	char buff[MAX_LENGTH];
	char *collist=NULL,*sql_q=NULL;
	MYSQL *conn;
	MYSQL_RES *res;
	//MYSQL_ROW row;
	int rcode=0,exline=0,linenum=0;
	int k=0,i=0,colnum=0,quot_val=0;
	int vopt=0,c,dbopt=0;
	char col_seperator=',';
	FILE *ifile = NULL;
	char *dbuser=NULL,*dbpass=NULL;
	char *dbhost=NULL,*dbname=NULL;
	char *col_stmt=NULL,*tmyname=NULL;

		while ((c = getopt (argc, argv, "hs:vf:u:p:H:d:t:c:")) != -1)
		{
			
			switch (c) {
				case 'h':
					Help();
					exit(0);
				break;

				case 's':
					col_seperator = optarg[0];
				break;

				case 'v':
					vopt = 1;
				break;

				case 'f':
					if (ifile = fopen(optarg,"rt")){
						printf("the file %s is o.k.\n",optarg);
						//fopt=1;
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

				case 'c':
					col_stmt = optarg;
				break;

				case 't':
					tmyname = optarg;
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
		}

		/* checking for connection parameters and 
 		 * environment variables
 		*/

		if (getenv("CSV2MYCOLUMNS"))
			col_stmt = getenv("CSV2MYCOLUMNS");

		if ( (!getenv("CSV2MYTABLE") ) && (!tmyname) ) {
			fprintf(stderr,"error - there is no destination table specified\n\n");
			Help();
			exit(2);
		}

		else if (getenv("CSV2MYTABLE"))
			tmyname = getenv("CSV2MYTABLE");

		if ( (!getenv("CSV2MYDBNAME") ) && (!dbname) ) {
			fprintf(stderr,"no destination Database name specified Aborting\n");
			Help();
			exit(3);
		}

		else if (getenv("CSV2MYDBNAME"))
			dbname = getenv("CSV2MYDBNAME");

		if ( (!getenv("MYSQL_HOST") ) && (!dbhost) ) {
		
			dbopt = 1;
			dbhost = calloc(strlen("localhost")+2,sizeof(char));
			strcpy(dbhost,"localhost");		
		}

		else if (getenv("MYSQL_HOST"))
			dbhost = getenv("MYSQL_HOST");

		if (getenv("MYSQL_USER"))
			dbuser = getenv("MYSQL_USER");

		if (getenv("MYSQL_PASS"))
			dbpass = getenv("MYSQL_PASS");

		if(dbuser) {
			if (!dbpass)
				dbpass = getpass("insert password: ");
		}

		// connection to the mysql Database
		
		conn = mysql_init(NULL);

		if (!mysql_real_connect(conn, dbhost, dbuser, dbpass, dbname, 0, NULL, 0)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
			exitnacely(conn);
		}		

		else if (vopt == 1)
			printf("the connection to the MySQL was successful\n");		

		if (ifile == NULL) {
			ifile = stdin;
			if(vopt == 1)
				printf("listening to STDIN for input\n");
		}

		// checking if the columns names are from the -c option

		if (col_stmt != NULL) {
			colnum = firstlinetst(col_stmt,vopt,col_seperator,&quot_val);
			tablecheck(col_stmt,tmyname,res,conn,vopt);
		}
		
		while( ((fgets(buff,MAX_LENGTH,ifile)) != NULL) ) {

			if (strlen(buff) < (colnum+1))
				break;

			if ( (linenum == 0) && (col_stmt == NULL)) {

				chomp(buff);
				colnum = firstlinetst(buff,vopt,col_seperator,&quot_val);
				col_stmt = (char *)calloc(strlen(buff),sizeof(char));
				strcpy(col_stmt,buff);
				tablecheck(col_stmt,tmyname,res,conn,vopt);
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


				sql_q = (char *)calloc((strlen(collist)+strlen(tmyname)+strlen(col_stmt)+200),sizeof(char));
				strcpy(sql_q,"insert into ");
				strcat(sql_q,tmyname);
				strcat(sql_q," (");
				strcat(sql_q,col_stmt);
				strcat(sql_q,") values(");
				strcat(sql_q,collist);
				strcat(sql_q,")");


				// this is the insert the CSV line into the database
				//
				
				if (mysql_query(conn,sql_q)) {
					fprintf(stderr,"problem inserting data to Mysql : %s\n",mysql_error(conn));
					exitnacely(conn);
			 	}
				else {
					res = mysql_use_result(conn);
					mysql_free_result(res);
				}

				free(collist);
				free(sql_q);
				collist = NULL;
				sql_q = NULL;
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

	/* close connection */
	fclose(ifile);
	mysql_free_result(res);
	mysql_close(conn);

	if (dbopt == 1)
		free(dbhost);

	return rcode;
}
