#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <math.h>


typedef struct Review {

	size_t revLen = 0;
	
	int * revYear = (int *) malloc( revLen * sizeof ( int ) );
	int * revMonth = (int *) malloc( revLen * sizeof ( int ) );
	int * revDay = (int *) malloc( revLen * sizeof( int ) );
	int * revGrade = (int *) malloc( revLen * sizeof( int ) );
	char ** revMess = (char **) malloc( revLen * sizeof( char* ) );	

} Review;


typedef struct Date {

	size_t year;
	size_t month;
	size_t day;

} Date;


typedef struct TwoLastDate {

    Date lastDate;
    Date prevDate;

} TwoLastDate;


typedef struct SearchBest {

	size_t currMedian;
	long double currAverage;

	size_t countEqual;
	size_t minReviews;

	size_t currStart = 0;
	size_t currEnd = 0;

} SearchBest;



typedef struct Best {

	double largDiff = -1;

	size_t bestMedian = 0;
	double bestAverage = 0;

	size_t bestStart = 0;
	size_t bestEnd = 0;

} Best;


typedef struct VariablesForPrint {

	size_t bestRevCount = 0;
	size_t bestStart = 0;

} VariablesForPrint;


/**
 * Free all reviews.
 *
 * @param[in] struct  The structure of reviews
 */
void freeMemory ( Review * allReviews ) 
{
	free( allReviews->revYear );
	free( allReviews->revMonth );
	free( allReviews->revDay );
	free( allReviews->revGrade );
	
	// Free each message of review
	for ( size_t reviewInd = 0; reviewInd < allReviews->revLen; reviewInd++ ) {
		free( allReviews->revMess[ reviewInd ] );
	}
	
	free( allReviews->revMess );
}

/**
 * Compare grades
 *  
 * @param[in] void firstGrade pointer
 * @param[in] void secondGrade pointer
 * 
 * @returns 1 if firstGrade is larger, then 2
 * @returns -1 if firstGrade is lower, then 2
 * @returns 0 otherwise
 */
int cmpGrades ( const void * firstGrade, const void * secondGrade ) 
{
	return ( *( int * ) firstGrade - *( int * ) secondGrade );
}


/**
 * Check if year is leap.
 *  
 * @param[in] int int with year it should check
 * 
 * @returns true if year is leap
 * @returns false if year isn't leap
 */
bool isLeap ( int year ) 
{
	if ( year % 4 != 0 || (year % 100 == 0 && year % 400 != 0) || year % 4000 == 0) 
	{
		return false;	
	}
	return true;
}

/**
 * Do thing, that mean end of program.
 *
 * Print message and free memory.
 *
 * @param[in] struct  structure of reviews
 */
void End ( Review * allReviews ) 
{
	printf( "Nespravny vstup.\n" );
	freeMemory( allReviews );
}


/**
 * Increases size of all review arrays and variables.
 *
 * @param[out] struct  structure of reviews
 */
void increaseArrSize ( Review * currRevs ) 
{
	// Increment length of reviews and increase size of each array
	currRevs->revLen++;
		
	currRevs->revYear = (int *) realloc( currRevs->revYear, currRevs->revLen * sizeof( currRevs->revYear ) );
	currRevs->revMonth = (int *) realloc( currRevs->revMonth, currRevs->revLen * sizeof( currRevs->revMonth ) );
	currRevs->revDay = (int *) realloc( currRevs->revDay, currRevs->revLen * sizeof( currRevs->revDay ) );
	currRevs->revGrade = (int *) realloc( currRevs->revGrade, currRevs->revLen * sizeof( currRevs->revGrade ) );
	currRevs->revMess = (char **) realloc( currRevs->revMess, currRevs->revLen * sizeof( char* ) );
};


/**
 * Makes structure of last reviews dates or 2 last reviews dates.
 *
 * Get the date from structure Review and put values to the new structure.
 * Make previous date:
 * - if exists more then 1 review.
 *
 * @param[in] struct  structure of reviews
 * @returns structure of dates
 */
TwoLastDate generateReviewDate ( Review * currRevs ) 
{
	// Make structure with previous and current dates and fill variables
	TwoLastDate lastRevDates;
	
	lastRevDates.lastDate.year = currRevs->revYear[ currRevs->revLen - 1 ];
	lastRevDates.lastDate.month = currRevs->revMonth[ currRevs->revLen - 1 ];
	lastRevDates.lastDate.day = currRevs->revDay[ currRevs->revLen - 1 ];
	
	// if exists more then 1 review.
	if ( currRevs->revLen > 1 ) 
	{
		lastRevDates.prevDate.year = currRevs->revYear[ currRevs->revLen - 2 ];
		lastRevDates.prevDate.month = currRevs->revMonth[ currRevs->revLen - 2 ];
		lastRevDates.prevDate.day = currRevs->revDay[ currRevs->revLen - 2 ];
	}
	
	return lastRevDates;
}


/**
 * Read review from input into array.
 *
 * @returns review it read from input
 */
char * getReview () 
{
	// Make variables for new review and length of review
	char * fullReview = nullptr;
	size_t reviewLen = 0;
	
	// Get the review
	getline( &fullReview, &reviewLen, stdin );
	
	return fullReview;
}

/**
 * Check if previous date is larger then last.
 *
 * Compare dates from structures of date.
 *
 * @param[in] struct  structure of structure with date
 * @returns true if previous date is larger, then last, otherwise false.
 */
bool isPrevDateLarger ( TwoLastDate * currRevDate ) 
{
	if ( ( currRevDate->lastDate.year < currRevDate->prevDate.year ) ||
		 ( currRevDate->lastDate.year == currRevDate->prevDate.year && currRevDate->lastDate.month < currRevDate->prevDate.month ) || 
		 ( currRevDate->lastDate.year == currRevDate->prevDate.year && currRevDate->lastDate.month == currRevDate->prevDate.month && currRevDate->lastDate.day < currRevDate->prevDate.day ) )
	{
		return true;
	}
	return false;
}


/**
 * Put new review to the structure of reviews
 *
 * Make new helpful variable for review message (it helps to get the correct size of review message).
 * Read variables from new review line and put them to the structure.
 * Increase size of review message and copy temporary variable to the array of message.
 *
 * @param[out] struct  structure of reviews
 * @param[in] struct  structure of reviews
 * @param[in] string  new review
 * 
 * @returns count of review scan
 */
int fillReviewVariables ( Review * currRevs, char * fullReview ) 
{
	// Make variable * for new review message
    char tmpRevMess[ 4097 ] = "";
    size_t lastRevInd = currRevs->revLen - 1;

    int scanCount = sscanf( fullReview, "%d-%d-%d %d %s", &currRevs->revYear[ lastRevInd ], &currRevs->revMonth[ lastRevInd ], &currRevs->revDay[ lastRevInd ], &currRevs->revGrade[ lastRevInd ], tmpRevMess );
	
	// Prepeare place for new message
	currRevs->revMess[ lastRevInd ] = (char *) malloc( ( strlen(tmpRevMess) + 1 ) * sizeof( char ) );
	// Copy message from variable * to the review message
	strcpy( currRevs->revMess[ lastRevInd ], tmpRevMess );
    
    return scanCount;
}


/**
 * Check if review isn't valid.
 *
 * Create structure of last and previous date for compare.
 * Check if new review has correct format and check if date of new review is correct.
 * 
 * @returns true
 * - if format of month and day isn't correct
 * - if year, month and day isn't in correct range
 * - if review have not year, month, day, grade or message
 * - if grade if lower then 1
 * - if previous date is larger then last
 * @returns false otherwise
 *
 * @param[in] struct  structure of reviews
 * @param[in] array  string of new review
 */
bool isNotValidReview ( Review * currRevs, char * review)
{
	int reviewScan = fillReviewVariables( currRevs, review );
	// Make structure with last and previous date of reviews
	TwoLastDate revsDate = generateReviewDate( currRevs );
	
	// Check if review isn't valid
	if ( !( review[ 6 ] >= '0' && review[ 6 ] <= '9' && review[ 9 ] >= '0' && review[ 9 ] <= '9' ) || 
		revsDate.lastDate.year < 1800 || revsDate.lastDate.year > 3000 || revsDate.lastDate.month > 12 || revsDate.lastDate.month < 1 || revsDate.lastDate.day < 1 ||
		( ( revsDate.lastDate.month == 1 || revsDate.lastDate.month == 3 || revsDate.lastDate.month == 5 || revsDate.lastDate.month == 7 || revsDate.lastDate.month == 8 || revsDate.lastDate.month == 10 || revsDate.lastDate.month == 12 ) && revsDate.lastDate.day > 31 ) || 
		( ( revsDate.lastDate.month == 4 || revsDate.lastDate.month == 6 || revsDate.lastDate.month == 9 || revsDate.lastDate.month == 11 ) && revsDate.lastDate.day > 30 ) || 
		( isLeap( revsDate.lastDate.year ) && revsDate.lastDate.month == 2 && revsDate.lastDate.day > 29) || 
		( !isLeap( revsDate.lastDate.year ) && revsDate.lastDate.month == 2 && revsDate.lastDate.day > 28 ) ||
		reviewScan != 5 || currRevs->revGrade[ currRevs->revLen - 1 ] <= 0 || ( currRevs->revLen > 1 && isPrevDateLarger( &revsDate ) ))
	{
		return true;
	}
	return false;
}


/**
 * Read string of review from input.
 *
 * Get the full string from input. Check if this string has correct format and data.
 * Reading is not successful:
 * - if new review has incorrect format
 * - if new review has incorrect data
 *
 * @param[in] struct structure of reviews
 * @returns true if reading is successful, false otherwise
 */
bool readReview( Review * currRevs ) 
{
	// Make variables with full line of review
	char * review = getReview();

	if ( isNotValidReview( currRevs, review ) ) 
	{
		return false;
	}

	// Free memory after getline
	free( review );

	return true;	
}


/**
 * Append new review to the structure of reviews.
 *
 * Increase each array of review structure.
 * Append new review at new place.
 * 
 * @param[in] struct structure of reviews
 * @param[out] struct structure of reviews
 * 
 * @returns true if it appended new review, false otherwise
 */
bool appendReview ( Review * currRevs ) 
{
	increaseArrSize( currRevs );	
	
	if ( !readReview( currRevs ) ) 
	{
		End( currRevs );
		return false;
	}
	return true;
}


/**
 * Check if dates of first and last index reviews are equal 
 * 
 * @param[in] struct structure of reviews
 * @param[in] size_t first index of review
 * @param[in] size_t second index of review
 * 
 * @returns true if year, month and day are equal, false otherwise
 */
bool datesAreEqual ( Review * currRevs, size_t * firstRevInd, size_t * secondRevInd ) 
{
	if ( currRevs->revYear[ *firstRevInd ] == currRevs->revYear[ *secondRevInd ] && currRevs->revMonth[ *firstRevInd ] == currRevs->revMonth[ *secondRevInd ] && currRevs->revDay[ *firstRevInd ] == currRevs->revDay[ *secondRevInd ] ) 
	{
		return true;
	}
	return false;
}


/**
 * Check if next and current reviews date are not equal
 * 
 * @returns true:
 * 	- if there are reviews enough between last and first indexes AND exists next date AND next and current dates are not equal
 * @returns false otherwise
 * 
 * @param[in] struct structure of reviews
 * @param[in] struct minimal count of reviews
 * @param[in] size_t first index of review
 * @param[in] size_t last index of review
 */
bool isNextDifferent ( Review * currRevs, size_t minRevCount, size_t * firstRevInd, size_t * currRevInd ) 
{
	size_t nextRevInd = *currRevInd + 1;

	if ( ( *currRevInd - *firstRevInd + 1 >= minRevCount && currRevs->revYear[ nextRevInd ] && !datesAreEqual( currRevs, currRevInd, &nextRevInd ) )
		|| *currRevInd == currRevs->revLen - 1 ) 
	{
		return true;	
	}
	return false;
}


/**
 * Looking for median in the array of reviews grades.
 *
 * Sort array of grades and take median in this array. 
 *
 * @param[in] array  array of reviews grades
 * @param[in] struct   structure of variables, that helps to find best variables
 * @returns median from array of grades
 */
size_t getCurrentMedian ( int * revGrades, SearchBest * searchBest ) 
{
	// Sort array of grades
	qsort( revGrades, searchBest->currEnd - searchBest->currStart + 1, sizeof( int ), cmpGrades );

	// Get the middle value of array
	return revGrades[ ( searchBest->currEnd - searchBest->currStart ) / 2 ];
}


/**
 * Get average in the array of reviews grades.
 *
 * Make variable for calculating sum. Get averege from this sum.   
 *
 * @param[in] array  array of reviews grades
 * @param[in] struct   structure of variables, that helps to find best variables
 * @returns average from array of grades
 */
double getCurrentAverage ( int * revGrades, SearchBest * searchBest ) 
{	
	// Make variable for calculating average
	double locAverage = 0;

	// Get sum of reviews grades
	for ( size_t gradeNumber = 0; gradeNumber < searchBest->currEnd - searchBest->currStart + 1; gradeNumber++ ) 
	{
		locAverage += revGrades[ gradeNumber ];
	}

	// Divide sum of grades by count of grades
	locAverage /= ( searchBest->currEnd - searchBest->currStart + 1 );
	
	return locAverage;
}


/**
 * Exchange best variables, if it found more suitable chain.
 *
 * Make variable with new difference.
 * Exchange best variables if.
 * 	- if new difference is larger then best difference.
 * 	- if new difference and best difference are equal AND if end index of the new chain is larger then end index of the best chain  
 *
 * @param[in] struct   structure of variables, that helps to find best variables
 * @param[out] struct   structure of best variables
 */
void exchangeBest ( SearchBest * searchBest, Best * bestRes ) 
{
	// Make variables of current difference for compare
	double currDiff = fabs( searchBest->currMedian - searchBest->currAverage );

	if ( bestRes->largDiff < currDiff
		|| ( bestRes->largDiff == currDiff && bestRes->bestEnd < searchBest->currEnd ) ) 
	{
		// Exchange best variabless
		bestRes->largDiff = currDiff;
		bestRes->bestMedian = searchBest->currMedian;
		bestRes->bestAverage = searchBest->currAverage;
		bestRes->bestStart = searchBest->currStart;
		bestRes->bestEnd = searchBest->currEnd;
	}
	return;
}


/**
 * Looking for the best chain from reviews array.
 *
 * Check current reviews chain if this chain is suitable for us (If next review has different date).
 * Create array of grades from first index to the last index. Get median and average in this array. 
 * If this difference between median and average is better, exchange better variables.
 * Free memory.
 *
 * @param[in] struct structure of reviews
 * @param[out] struct   structure of variables, that helps to find best variables
 * @param[in] struct   structure of best variables
 * @param[in] size_t first index of review
 * @param[in] size_t last index of review
 */
void findBestReviewChain ( Review * currRevs, SearchBest * searchBest, Best * bestRes , size_t * firstRevInd, size_t * currRevInd ) 
{
	if ( isNextDifferent( currRevs, searchBest->minReviews, firstRevInd, currRevInd ) ) 
	{
		// Make new array of grades in range
		int * revGrades = (int *) malloc( ( searchBest->currEnd - searchBest->currStart + 1 ) * sizeof( int ));

		// Fill the array by grades
		for ( size_t gradeInd = 0; gradeInd < searchBest->currEnd - searchBest->currStart + 1; gradeInd++ ) {
			revGrades[ gradeInd ] = currRevs->revGrade[ searchBest->currStart + gradeInd ];
		}
		
		// Get median in array of grades
		searchBest->currMedian = getCurrentMedian( revGrades, searchBest );
		
		// Get average in array of grades
		searchBest->currAverage = getCurrentAverage( revGrades, searchBest );
		
		// Exchange best variables, if current variables are more suitable
		exchangeBest( searchBest, bestRes );
		
		free( revGrades );
	}	
	return;	
}


/**
 * Get the best chain of reviews.
 *
 * Make all possible options for the beginning and end of the chain.
 * Break the cycle, if sum of beginning position and minimal count of reviews is larger, then count of reviews.
 * Fill search best variables, in order to find the best chain.
 * Increase countEqual, if date of current review and date of first review are equal.
 * Check if current chain is better. If yes - exchange variables.
 * Then skip count of reviews with equal dates (We shouldn't check this chains).
 *
 * @param[in] struct structure of reviews
 * @param[out] struct   structure of variables, that helps to find best variables
 * @param[in] struct   structure of best variables
 */
void getBestReviewChain ( Review * currRevs, SearchBest * searchBest, Best * bestRes ) 
{
	// Get all possible chains of reviews
	for ( size_t firstRevInd = 0; firstRevInd < currRevs->revLen; firstRevInd++ ) 
	{
		// Reset current variables
		searchBest->countEqual = 0;
		searchBest->currStart = firstRevInd;
		searchBest->currEnd = firstRevInd;

		if ( searchBest->currStart + searchBest->minReviews > currRevs->revLen ) 
		{
			break;
		}

		for ( size_t currRevInd = firstRevInd; currRevInd < currRevs->revLen; currRevInd++, searchBest->currEnd++ ) 
		{
			// Reset current median and average
			searchBest->currMedian = 0;
			searchBest->currAverage = 0;
		
			// If date of first and this reviews are equal - don't make this review first afterwords
			if ( datesAreEqual( currRevs, &firstRevInd, &currRevInd ) ) 
			{
				searchBest->countEqual++;
			}
			
			// Check if current chain is more suitable. If yes - exchange best
			findBestReviewChain( currRevs, searchBest, bestRes, &firstRevInd, &currRevInd );
		}

		// Skip reviews, that have the same date as first review
		firstRevInd += searchBest->countEqual - 1;
	}
	return;
}


/**
 * Get minimal amount of reviews.
 *
 * End the program:
 * 	- if reading from input wasn't succesful
 * 	- if minimal amount is lower then 1
 * 	- if doesn't exist any review
 * Ask again:
 * 	- if minimal amount is larger, then length of reviews
 *
 * @param[in] struct   structure of reviews
 * @param[in] struct  structure of variables, that helps to find best variables
 * @returns 1 if end program
 * @returns 0 if ask again
 * @returns 2 otherwise
 */
size_t getMinAmountReviews ( Review * allReviews, SearchBest * searchBest ) 
{
	// Read minimal amount of reviews, and check if is correct
	if ( scanf( "%zu", &searchBest->minReviews ) != 1 || searchBest->minReviews < 1 || (searchBest->minReviews > allReviews->revLen && allReviews->revLen <= 0 ) ) 
	{
		End( allReviews );
		return 1;
	} else if ( searchBest->minReviews > allReviews->revLen && allReviews->revLen > 0 ) 
	{
		printf ( "Neexistuje.\n" );
		return 0;
	}
	return 2;
}


/**
 * Print head of the best review chain
 *
 * Print the info about best review chain in the correct format.
 *
 * @param[in] struct   structure of reviews
 * @param[in] struct  structure of variables, that helps to find best variables
 * @param[in] struct  structure of best variables
 * @param[out] struct  structure of variables, that we need for print
 */
void printBestHead ( Review * allReviews, SearchBest * searchBest, Best * bestRes , VariablesForPrint * printVars ) 
{
	size_t bestStart = bestRes->bestStart;
	size_t bestEnd = bestRes->bestEnd;
	
	printVars->bestRevCount = bestEnd - bestStart + 1;
	printVars->bestStart = bestStart;
	
	// Print head of best reviews in correct format
	printf( "%d-%02d-%02d - %d-%02d-%02d: %lf %zu\n", allReviews->revYear[ bestStart ], allReviews->revMonth[ bestStart ], allReviews->revDay[ bestStart ], allReviews->revYear[ bestEnd ], allReviews->revMonth[ bestEnd ], allReviews->revDay[ bestEnd ], bestRes->bestAverage, bestRes->bestMedian );
	
	return;
}


/**
 * Print full list of the best review chain
 *
 * Print the info about best review chain in the correct format.
 *
 * @param[in] struct   structure of reviews
 * @param[out] struct  structure of variables, that we need for print
 */
void printBestReviewList ( Review * allReviews, VariablesForPrint * printVars ) 
{
	// Print each review in correct format
	for ( size_t reviewNum = printVars->bestStart; reviewNum < printVars->bestStart + printVars->bestRevCount; reviewNum++ ) 
	{
		printf( "  %d: %s\n", allReviews->revGrade[ reviewNum ], allReviews->revMess[ reviewNum ] );
	}
	return;
}


/**
 * Get the action
 *
 * Get the sign, that means operation:
 * 	+: Add review
 * 	#: Print only head of the best chain
 * 	?: Print head and list of the best review chain
 */
char getAction () 
{
	// Read action to the variable
    char action;

	scanf( "%c ", &action );

    return action;
}


int main() 
{	
	// Make new structures
	Review myReviews;
	VariablesForPrint printVars;
	
	printf( "Recenze:\n" );
	
	while ( !feof( stdin ) ) 
	{
        char action = getAction();

		if ( action == '+' ) 
		{
			// Try to append new review. If it didn't add review - end the programm
			if ( !appendReview( &myReviews ) ) 
			{
				return 1;
			}
		} else if ( action == '?' || action == '#' ) 
		{
			// Make structures for getting best reviews chain
			SearchBest searchBest;
			Best bestRes;

			// Create variable with minimal amount of reviews
			size_t minAmountReviews = getMinAmountReviews( &myReviews, &searchBest );
			
			if ( minAmountReviews == 1 ) 
			{ // if minAmountReviews has bad value
				return 1;
			} else if ( minAmountReviews == 0 ) 
			{ // if minAmountReviews is larger then amount of all reviews
				continue;
			}
			
			// Get the best chain of reviews 
			getBestReviewChain( &myReviews, &searchBest, &bestRes );

			// Print the head of the best reviews chain in the correct format
			printBestHead( &myReviews, &searchBest, &bestRes, &printVars );
			
		} else if ( action != '\n' ) 
		{
			End( &myReviews );
			return 1;
		}
		if ( action == '?' ) 
		{
			//Print full chain in the correct format
			printBestReviewList( &myReviews, &printVars );
		}

	}
	// Free memory and finish programm
	freeMemory( &myReviews );
	
	return 0;
}
