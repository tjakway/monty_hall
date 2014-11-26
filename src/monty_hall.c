/**
 * Copyright 2014 Thomas Jakway
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * Monty Hall problem simulator using the public domain ISAAC crypto RNG
 */
 

/**
 * ************INCLUDES************
 */
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include "rand.h"

/**
 * ************GLOBALS************
 */
static struct option long_options[] = {
	{"count",	required_argument, 0, 'c' }
};

struct{
	//number of times to run the simulation
	unsigned int count;
} opt;

//used by ISAAC
randctx ctx;

/**
 * ************DEFINES************
 */

/*number of times to run the simulation*/
#define DEFAULT_COUNT 2000000

#define HELP_STR "Pass --count <number> to run the simulation <number>"\
			   "first with the player switching choices, then without.";

#define SWITCH TRUE //do we switch doors or not?

#define NUM_DOORS 3U


/**
 * ************PROTOTYPES************
 */
ub4 get_host_door(ub4 doors[], ub4 player_choice);
ub4 rand_lim(ub4 limit);
ub4 get_rand_not(ub4 limit, ub4 not[], ub4 not_size);
ub4 get_other_door(ub4 doors[], ub4 firstdoor, ub4 seconddoor);


/**
 * ************FUNCTIONS************
 */

int main(int argc, char **argv)
{
	//set defaults--will be overwritten by getopt if there are any args
	opt.count = DEFAULT_COUNT;
	
	int long_index = 0;
	int c;
	while((c = getopt_long(argc, argv, "c:"), long_options, &long_index) != -1)
	{
		switch(c)
		{
		case 'c':
			opt.count = optarg;
			break;
		case '?':
			printf(HELP_STR);
			exit(1);
		default:
			printf(HELP_STR);
			exit(1);
		}
	}
	
	
	//initialize ISAAC
	ctx.randa=ctx.randb=ctx.randc=(ub4)0;
	for (ub4 i=0; i<256; ++i) ctx.randrsl[i]=(ub4)0;
	randinit(&ctx, FALSE); // randinit calls isaac at the end
	isaac(&ctx); //run again just in case--
				 //the authors do this in their paper
	
	
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
		ub4 players_pick = rand_lim(NUM_DOORS-1); //NUM_DOORS - 1 because C counts up from 0
		
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
	ub4 host_pick = rand_lim(NUM_DOORS); //DOORS_LIMIT == 2 because C counts up from 0
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

	//loop over NUM_DOORS (which is 3) NOT (NUM_DOORS-1) or else we cut off the last run of the for loop!
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
 * return a random number between 0 and limit inclusive.
 * see http://stackoverflow.com/questions/2999075/generate-a-random-number-within-range
 * generates a random number without skew
 */
ub4 rand_lim(ub4 limit) 
{
	//UB4MAXVAL= RAND_MAX
	double divisor = UB4MAXVAL/(limit+1);
	ub4 retval;
	
	do {
		retval = rand(&ctx) / divisor; //ISAAC redefines rand
	} while (retval > limit);

	return retval;
}
