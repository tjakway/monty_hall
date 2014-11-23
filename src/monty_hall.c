/**
 * Monty Hall problem simulator using the public domain ISAAC crypto
 * RNG
 * Thomas Jakway, 11/23/14
 */
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "rand.h"

/*number of times to run the simulation*/
#define COUNT 100

#define SWITCH TRUE //do we switch doors or not?

randctx ctx;

ub4 rand_lim(ub4 limit);
ub4 get_rand_not(ub4 limit, ub4 not[], ub4 not_size);

ub4 main()
{
	ctx.randa=ctx.randb=ctx.randc=(ub4)0;
	for (ub4 i=0; i<256; ++i) ctx.randrsl[i]=(ub4)0;
	
	randinit(&ctx, FALSE); // randinit calls isaac at the end
	isaac(&ctx);
	
	/*num_success = # of correct guesses (got the car)
	 * num_failure = # of incorrect guesses (got the goat)*/
	ub4 num_success=0, num_failure=0;
	
	for(ub4 i = 0; i < COUNT; i++)
	{
		//TRUE = has the car, FALSE = has the goat
		isaac(&ctx);
		ub4 doors[] = {0, 0, 0};
		
		ub4 car_index = rand_lim(2); //pick a random door to put the car behind
		doors[car_index] = TRUE; 
		
		//get a 2nd random number that is not the car--this is the player's pick
		//get a random number between 0 and 2 (since the problem presents 3 doors)
		ub4 not_size = 1;
		ub4 not[] = { car_index };
		ub4 players_pick = get_rand_not(2, not, not_size); 
		assert(players_pick < 3);
		
		//get a random ub4eger that is not the player's choice and is not the car
		ub4 not_reveal_size = 2;
		ub4 not_reveal[] = { car_index, players_pick };
		ub4 montys_pick = get_rand_not(2, not_reveal, not_reveal_size);
		
		//monty revealed the door--do you switch?
		if(SWITCH)
		{
			//switching means you pick whatever monty didn't pick
			ub4 new_pick_not_size = 1;
			ub4 new_pick_not[] = { montys_pick };
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
ub4 get_rand_not(ub4 limit, ub4 not[], ub4 not_size)
{
	ub4 pick = 10000;
	while(TRUE)
	{
		pick = rand_lim(limit);
		
		//this is an inversion of the usual pattern
		//instead of checking for membership in an array, check for not membership in an array
		//if any value matches pick, this pick value is bad--run the loop again
		for(ub4 i = 0; i < not_size; i++)
		{
			if(pick == not[i])
				continue;
		}
	}
	
	//pick is an array index and rand_lim should return between 0 and limit inclusive
	assert(pick < 3);
}

/**
 * see http://stackoverflow.com/questions/2999075/generate-a-random-number-within-range
 * generate random number without skew
 */
ub4 rand_lim(ub4 limit) {
/* return a random number between 0 and limit inclusive.
 */
	
	isaac(&ctx); // and here it's called a second time before reading from randrsl!
	/*UB4MAXVAL= RAND_MAX*/
	double divisor = UB4MAXVAL/(limit+1);
	ub4 retval;
	
	do {
		retval = rand(&ctx) / divisor; //ISAAC redefines rand so this is OK
	} while (retval > limit);

	return retval;
}