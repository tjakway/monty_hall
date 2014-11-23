/**
 * Monty Hall problem simulator using the public domain ISAAC crypto
 * RNG
 * Thomas Jakway, 11/23/14
 */
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "isaac64.h"

/*number of times to run the simulation*/
#define COUNT 100

#define SWITCH TRUE //do we switch doors or not?

ub8 rand_lim(unsigned int limit);
int get_rand_not(unsigned int limit, unsigned int not[], unsigned int not_size);

int main()
{
	//initialize the cryptographic random number generator
	randinit(FALSE);
	
	/*num_success = # of correct guesses (got the car)
	 * num_failure = # of incorrect guesses (got the goat)*/
	unsigned int num_success=0, num_failure=0;
	
	for(int i = 0; i < COUNT; i++)
	{
		//TRUE = has the car, FALSE = has the goat
		isaac64();
		unsigned int doors[] = {0, 0, 0};
		
		unsigned int car_index = rand_lim(2); //pick a random door to put the car behind
		doors[car_index] = TRUE; 
		
		//get a 2nd random number that is not the car--this is the player's pick
		//get a random number between 0 and 2 (since the problem presents 3 doors)
		unsigned int not_size = 1;
		unsigned int not[] = { car_index };
		int players_pick = get_rand_not(2, not, not_size); 
		assert(players_pick > -1);
		
		//get a random integer that is not the player's choice and is not the car
		unsigned int not_reveal_size = 2;
		unsigned int not_reveal[] = { car_index, players_pick };
		int montys_pick = get_rand_not(2, not_reveal, not_reveal_size);
		
		//monty revealed the door--do you switch?
		if(SWITCH)
		{
			//switching means you pick whatever monty didn't pick
			unsigned int new_pick_not_size = 1;
			unsigned int new_pick_not[] = { montys_pick };
			players_pick = get_rand_not(2, new_pick_not, new_pick_not_size);
		}
		
		//did the player win?
		if(players_pick == car_index)
		{
			num_success++;
		}
		else
		{
			num_failure++;
		}
	}
	
	printf("\n**********MONTY HALL SIMULATION RESULTS**********\n");
	if(SWITCH)
		printf("\nThe player chose TO switch.");
	else
		printf("\nThe player chose NOT to switch.");
	printf("\nCOUNT = %d (number of times the simulation was run)", COUNT);
	printf("\nNUM_SUCCESS = %u", num_success);
	printf("\nNUM_FAILURE = %u", num_failure);
	printf("\nPercent correct guesses (GOT THE CAR): %f", ((double)num_success)/((double)num_failure));
	printf("\nPercent incorrect guesses (GOT THE GOAT): %f", ((double)num_success)/((double)num_failure));
}

/**
 * get a random number between 0 and limit inclusive that is not any of the values in not
 */
int get_rand_not(unsigned int limit, unsigned int not[], unsigned int not_size)
{
	int pick = -1;
	while(TRUE)
	{
		pick = rand_lim(limit);
		
		//this is an inversion of the usual pattern
		//instead of checking for membership in an array, check for not membership in an array
		//if any value matches pick, this pick value is bad--run the loop again
		for(int i = 0; i < not_size; i++)
		{
			if(pick == not[i])
				continue;
		}
	}
	
	//pick is an array index and rand_lim should return between 0 and limit inclusive
	assert(pick > -1);
}



/**
 * see http://stackoverflow.com/questions/2999075/generate-a-random-number-within-range
 * generate random number without skew
 */
ub8 rand_lim(unsigned int limit) {
/* return a random number between 0 and limit inclusive.
 */
	/*UB8MAXVAL= RAND_MAX*/
    int divisor = UB8MAXVAL/(limit+1);
    int retval;

    do { 
        retval = rand() / divisor; //ISAAC redefines rand so this is OK
    } while (retval > limit);

    return retval;
}