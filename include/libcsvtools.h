/* 
 * int csvobjnum(cahr * csvLine) 
 * check for the amont of objects on the line
 * int csvobjnum(char* csvLine,char* csvDelimiter)
 * check for the amount of objects with the given delimiter
 * char* csvcheckdel(char* csvLine)
 * check for special characters and returning the most likly delimiter
 * char* csvgetheader(char* csvLine)
 * returns an array of string which contains the works for the line without the delimiter
 * char* csvgetheader(char* csvLine, char* csvDelimiter)
 * returns an array of string which contains the works for the line without the delimiter
 *
 */

#ifndef LIBCSVTOOLS_H_INCLUDED
#define LIBCSVTOOLS_H_INCLUDED

int Print_csvtools();

#endif
