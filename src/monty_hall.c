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
#define COUNT 2000000

#define SWITCH TRUE //do we switch doors or not?

#define NUM_DOORS 3U
#define DOORS_LIMIT 2U

randctx ctx;

ub4 get_host_door(ub4 doors[], ub4 player_choice);
ub4 rand_lim(ub4 limit);
ub4 get_rand_not(ub4 limit, ub4 not[], ub4 not_size);
ub4 get_other_door(ub4 doors[], ub4 firstdoor, ub4 seconddoor);

int main()
{
	ctx.randa=ctx.randb=ctx.randc=(ub4)0;
	for (ub4 i=0; i<256; ++i) ctx.randrsl[i]=(ub4)0;
	
	randinit(&ctx, FALSE); // randinit calls isaac at the end
	isaac(&ctx);
	//srand(time(NULL));
	
	
	/*num_success = # of correct guesses (got the car)
	 * num_failure = # of incorrect guesses (got the goat)*/
	ub4 num_success=0, num_failure=0;
	
	for(ub4 i = 0; i < COUNT; i++)
	{
		//TRUE = has the car, FALSE = has the goat
		
		ub4 doors[] = {0, 0, 0};
		
		ub4 car_index = rand_lim(2); //pick a random door to put the car behind
		doors[car_index] = TRUE; 
		
		//the player picks a random door
		ub4 players_pick = rand_lim(DOORS_LIMIT);
		
		//get the host's pick
		//it will be a door that isn't yours that reveals a goat
		ub4 host_pick = get_host_door(doors, players_pick);
		
		//do you switch to the one that isn't yours and the host hasn't revealed?
		if(SWITCH)
		{
			players_pick = get_other_door(doors, players_pick, host_pick);
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
	printf("\nNUM_SUCCESS = %lu", num_success);
	printf("\nNUM_FAILURE = %lu", num_failure);
	printf("\nPercent correct guesses (GOT THE CAR): %f", ((double)num_success)/COUNT);
	printf("\nPercent incorrect guesses (GOT THE GOAT): %f\n", ((double)num_failure)/COUNT);
	return 0;
}

ub4 get_host_door(ub4 doors[], ub4 player_choice)
{
	//generate a completely random door and assert it meets these conditions
	ub4 host_pick = rand_lim(DOORS_LIMIT); //DOORS_LIMIT == 2 because C counts up from 0
	//recurse & try again if the generated number is the player choice or the car
	//the host must open a door that isn't the player's and has a goat
	if((host_pick == player_choice) || (doors[host_pick] == TRUE))
	{
		return get_host_door(doors, player_choice);
	}
	else
	{
		return host_pick;
	}
}

/**
 * get a random number between 0 and limit inclusive that is not any of the values in not
 */
ub4 get_rand_not(ub4 limit, ub4 not[], ub4 not_size)
{
	ub4 pick = rand_lim(limit);
	
	for(ub4 i = 0; i < not_size; i++)
	{
		//if this set isn't good, recurse until we get a matching one
		if(pick == not[i])
			return get_rand_not(limit, not, not_size);
	}
	assert(pick < limit);
	return pick;
}

ub4 get_other_door(ub4 doors[], ub4 firstdoor, ub4 secondoor)
{
	assert(firstdoor <= DOORS_LIMIT);
	assert(secondoor <= DOORS_LIMIT);
	assert(doors[0] != doors[1] != doors[2]);

	//loop over NUM_DOORS == 3, it's the not inclusive version of DOORS_LIMIT
	ub4 i;
	for(i = 0; i < NUM_DOORS; i++)
	{
		//assert(firstdoor > -1); assert(firstdoor < 3);
		//assert(secondoor > -1); assert(secondoor < 3);
		//test i not doors[i] because firstdoor and seconddoor are indices
		if((i != firstdoor) && (i != secondoor))
		{
			return i;
		}
	}
	//error--return an absurd value
	perror("ERROR in get_other_door: no 3rd door!");
	return 500;
}

/**
 * see http://stackoverflow.com/questions/2999075/generate-a-random-number-within-range
 * generate random number without skew
 */
ub4 rand_lim(ub4 limit) {
/* return a random number between 0 and limit inclusive.
 */
	/*UB4MAXVAL= RAND_MAX*/
	double divisor = UB4MAXVAL/(limit+1);
	ub4 retval;
	
	do {
		retval = rand(&ctx) / divisor; //ISAAC redefines rand so this is OK
	} while (retval > limit);

	return retval;
}
