/*

12-8-21
Josh Klipstein
Running Tracker project

*/

#include <stdio.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <time.h>
#include <stddef.h>

struct Runner {
	
	float displacement;
	float h_disp;
	float v_disp;
	float speed;
	float clock;
	float calories;

};

void getRandom( unsigned int code, struct Runner *runPtr );
void init( struct Runner *runPtr );
int intro();
void run ( struct Runner *runPtr );
void results( struct Runner *runPtr );
int check( struct Runner *runPtr ); 

int main () {

   struct Runner runner; /* runner struct */
   struct Runner *runPtr = &runner; /* runner ptr */
   int escape = 0;

   /* Introduce user */   

   printf( "Hello world!\nThis program tracks where a runner goes relative to their " );
   printf( "starting point (usually a house), and then suggests whether they should" );
   printf( " turn back based on their calorie count.  Their number of calories is" );
   printf( " entered upon start.  It will go down at a rate of 1/3 of their running" );
   printf( " speed.  The system will suggest if the runner should head back to their" );
   printf( " starting point if their calorie count is less than half the displacement " );
   printf( "they are at.  The program will not let the runner expend more energy than provided; " );
   printf( "A runner may nor may not expend all of their calories when they return to  " );
   printf( "their starting point.  This program is not set to track any real runners yet, " );
   printf( " but offers a demonstration of tracking a virutal runner with a random number" );
   printf( " of calories to begin.  The time running is recorded and displayed along with " );
   printf( "the displacement and calorie count of the runner." );

   escape = intro(); /* prompt user to make choice */

   /* printf( "\n%p\n", runPtr ); DEBUG */

   while(escape == 0) {

   	init( runPtr ); /* reset runner */
   	run( runPtr );  /* run demonstration */
   	results( runPtr );  /* display results */
	printf("\n");
	escape = intro();

   }

   return 0;

} /* end main */

/* function to generate and return random numbers. */
void getRandom( unsigned int code, struct Runner *runPtr ) {

   unsigned int r;
 
   /* set the seed */
   srand( ( unsigned )time( NULL ) );
	
   /* switch structure for code */
   switch( code ) {

	case 1:
		r = rand() % 3;
		/* printf( "%d", r ); DEBUG */

		/* switch structure for displacement factors */
		switch(r) {
			case 0:

				/* accelerate horizontally */
				runPtr->h_disp += runPtr->speed;
				runPtr->calories -= ( 1.0 / 3.0 ) * runPtr->speed;
				break;

			case 1:

				/* accelerate vertically */
				runPtr->v_disp += runPtr->speed;
				runPtr->calories -= ( 1.0 / 3.0 ) * runPtr->speed;
				break;

			default:
				break; /* runner rests */
		};
		break;

	case 2:
		/* set number calories runner */
		r = ( rand() % 100 + 1 ) * 1000;
		runPtr->calories = r;
		break;

	case 3:
		/* decrease runner displacement based on speed */
		r = rand() % 3;

		switch(r) {
			case 0:

				if ( runPtr->h_disp > 0.0 ) {

					runPtr->h_disp -= runPtr->speed;
					runPtr->calories -= ( 1.0 / 3.0 ) * runPtr->speed;

				} /* end if */

			case 1:

				if ( runPtr->v_disp > 0.0 ) {

					runPtr->v_disp -= runPtr->speed;
					runPtr->calories -= ( 1.0 / 3.0 ) * runPtr->speed;

				} /* end if */

			default:
				break; /* runner rests */
		};
		break;

	case 4:
		/* Set runner speed */
		r = rand() % 21 + 2;

		runPtr->speed = r;

   };
 
} /* end getRandom */

void init( struct Runner *runPtr ) {

   runPtr->displacement = 0.0;
   runPtr->h_disp = 0.0;
   runPtr->v_disp = 0.0;
   runPtr->speed = 0.0;
   runPtr->calories = 0.0;
   runPtr->clock = 0.0;
   getRandom( 2, runPtr );
   getRandom( 4, runPtr );

} /* end init */

int intro() {

	char choice; /* choice for user to escape program or not. 
				default: 'n' */
	char newline;

	printf( "Enter y for run demonstration, or n for quit: " );

	choice = getchar(); /* Adapt user choice */
	newline = getchar();

	switch( choice ) {

		case 89:
		case 121:
			return 0;
			break;
		case 78:
		case 110:
			return 1;
			break;
		default:
			printf( "Invalid choice.\n\n" );
			intro();

	} /* end switch for choice */

} /* end intro */

void run ( struct Runner *runPtr ) {

   FILE *fp; /* file pointer */
   int tooFar = 0;
   struct tm* ptr;
   time_t t; /* time object */
   t = time( NULL );
   ptr = localtime( &t ); /* pointer for local time */
   clock_t time; /* clock objects for time */

   /* print initial parameters */
   printf( "\nRunner speed: %.2f km/h\n", runPtr->speed ); /* DEBUG */
   printf( "Runner starts with %.2f calories\n", runPtr->calories ); /* DEBUG */
   printf( "Running demonstration...\n" );

   /* open file for writing */
   fp = fopen( "results.txt", "w+" );

   /* Write header line in file w/ time/date */
   fprintf( fp, "Running simulation results for %s\n", asctime( ptr ) );
   fprintf( fp, "Runner speed is %.2f km/h\n", runPtr->speed );
   fprintf( fp, "Runner starts with %.2f calories\n", runPtr->calories );

   /* main loop */

   do {

	time = clock();
	tooFar = check( runPtr );

	if ( tooFar ) {

		/* runner turns back */
   		fprintf( fp, "Going back...\n" );
		getRandom( 3, runPtr );

   	} else getRandom( 1, runPtr );

	runPtr->clock += ( float )( time ) / ( float )( CLOCKS_PER_SEC );
	runPtr->displacement = hypot( runPtr->h_disp, runPtr->v_disp );
   	
   	/* print the actual value */
   	fprintf( fp, "\nNumber of seconds: %.2f\n", runPtr->clock );

   	/* print calories */
   	fprintf( fp, "Total calories: %.2f\n", runPtr->calories );

   	/* print displacement */
   	fprintf( fp, "Horiz. Displacement: %.2f\n", runPtr->h_disp );
	fprintf( fp, "Vert. Displacement: %.2f\n", runPtr->v_disp );
	/* printf( "%f\n", runPtr->displacement ); DEBUG */

   } while ( runPtr->displacement > 0.0 );

   /* close file */
   fclose( fp );

} /* end run */

void results( struct Runner *runPtr ) {

	FILE *fp;

	printf( "\n\nRESULTS:\n" );
	printf( "Runner calories left: %.2f\n", runPtr->calories );
	printf( "Total time: %.2f sec\n", runPtr->clock );
	printf( "Results also put in results.txt" );

	fp = fopen( "results.txt", "a+" );
	fprintf( fp, "\n\nRESULTS:\n");
   	fprintf( fp, "Runner calories left: %.2f\n", runPtr->calories );
	fprintf( fp, "Total time:  %.2f sec\n", runPtr->clock );  
   	fclose( fp );

} /* end results */

int check( struct Runner *runPtr ) {

	if ( runPtr->calories <= ( runPtr->displacement / 2.0 ) )
		return 1;

	return 0;

} /* end check */