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
	unsigned int file_opt:1;
	unsigned int test_opt:1;
	unsigned int quot_val:1;
} arg_options;


void Help(int rcode) {

        printf("Usage:  csvsplit\n");
        printf("\t\t -v - Be Verbose when running the command\n");
        printf("\t\t -s - the seperator between the columns (Default: \",\") \n");
        printf("\t\t -h - Display this manu\n");
        printf("\t\t -n - use a Numeric Prefix for the new files (Default) \n");
        printf("\t\t -a - use an Alphabetical Prefix for the new files\n");
        printf("\t\t -f - set The CSV file to read from (Mandatory)\n");
        printf("\t\t -c - Column's names - set the columns name (Defualt: First Line)\n");
        printf("\n");

	exit(rcode);
}

int main(int argc,char *argv[]) {

	int rcode=0;


	return rcode;
}
