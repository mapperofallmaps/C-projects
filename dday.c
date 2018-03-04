/* 24/03/16
 * Daniel Lau 3376221
 * Doomsday.c
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
 
#define THURSDAY 0
#define FRIDAY   1
#define SATURDAY 2
#define SUNDAY   3
#define MONDAY   4
#define TUESDAY  5
#define WEDNESDAY 6
 
#define TRUE 1
#define FALSE 0
#define DAYS_PER_WEEK 7
 
int dayOfWeek (int doomsday, int leapYear, int month, int day);
 
int main (int argc, char *argv[]) {

   assert (dayOfWeek (THURSDAY,  FALSE,  4,  4) == THURSDAY);
   assert (dayOfWeek (FRIDAY,    FALSE,  6,  6) == FRIDAY);
   assert (dayOfWeek (MONDAY,    FALSE,  8,  8) == MONDAY);
   assert (dayOfWeek (WEDNESDAY, FALSE, 10, 10) == WEDNESDAY);
   assert (dayOfWeek (FRIDAY,    FALSE, 12, 12) == FRIDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  9,  5) == THURSDAY);
   assert (dayOfWeek (FRIDAY,    FALSE,  5,  9) == FRIDAY);
   assert (dayOfWeek (SUNDAY,    FALSE,  7, 11) == SUNDAY);
   assert (dayOfWeek (TUESDAY,   FALSE, 11,  7) == TUESDAY);
   assert (dayOfWeek (WEDNESDAY, FALSE,  3,  7) == WEDNESDAY);
 
   assert (dayOfWeek (THURSDAY,  FALSE,  4,  5) == FRIDAY);
   assert (dayOfWeek (SATURDAY,  FALSE,  6,  5) == FRIDAY);
   assert (dayOfWeek (MONDAY,    FALSE,  8,  9) == TUESDAY);
   assert (dayOfWeek (WEDNESDAY, FALSE, 10,  9) == TUESDAY);
   assert (dayOfWeek (FRIDAY,    FALSE, 12, 20) == SATURDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  9,  9) == MONDAY);
   assert (dayOfWeek (FRIDAY,    FALSE,  5,  5) == MONDAY);
   assert (dayOfWeek (SUNDAY,    FALSE,  7,  7) == WEDNESDAY);
   assert (dayOfWeek (TUESDAY,   FALSE, 11, 11) == SATURDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  3, 30) == SATURDAY);
 
   assert (dayOfWeek (TUESDAY,   FALSE,  2,  28) == TUESDAY);
   assert (dayOfWeek (TUESDAY,   FALSE,  2,  27) == MONDAY);
   assert (dayOfWeek (THURSDAY,  FALSE,  1,  3)  == THURSDAY);
   assert (dayOfWeek (FRIDAY, FALSE, 6, 4) ==WEDNESDAY );
   assert (dayOfWeek (FRIDAY, FALSE, 2, 26) ==WEDNESDAY );
   assert (dayOfWeek (SATURDAY, TRUE, 9, 20) ==SUNDAY);
 
   printf ("all tests passed - You are awesome!\n");
   return EXIT_SUCCESS;
}

int dayOfWeek (int doomsday, int leapYear, int month, int day) {
       if (month == 1) {
        month = 3 + leapYear;
    } else if (month == 2) {
        month = 28 + leapYear;
    } else if (month == 3 || month == 5 || month == 7 || month == 9 || month == 11) {
        month += (-4 + (((8 - month) > 0)*8));
    }
   
    return (((doomsday + 14) - (month % 7) + (day % 7)) % 7);
}
