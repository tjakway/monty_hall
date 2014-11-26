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
#include <stdlib.h>
#include <getopt.h> //for getopt_long
#include <stdbool.h>
#include "rand.h" //for ISAAC

/**
 * ************GLOBALS************
 */
static struct option long_options[] = {
	{"count",	required_argument, 0, 'c' },
	{"help", 	no_argument, 	   0, 'h' }
};

struct{
	//number of times to run the simulation
	unsigned long int count;
} opt;

//used by ISAAC
randctx ctx;

/**
 * ************DEFINES************
 */

/*number of times to run the simulation*/
#define DEFAULT_COUNT 2000000

#define HELP_STR "Pass --count <number> to run the simulation <number>"\
			   "first with the player switching choices, then without.\n"\
			   "The paradox, quotingfrom http://en.wikipedia.org/wiki/Monty_Hall_problem#The_paradox:\n"\
			   "Suppose you're on a game show, and you're given the choice"\
			   " of three doors:"\
			   " Behind one door is a car; behind the others, goats."\
			   " You pick a door, say No. 1, and the host, who knows "\
			   "what's behind the doors, opens another door, say No. 3, "\
			   "which has a goat. He then says to you, \"Do you want to "\
			   "pick door No. 2?\" Is it to your advantage to switch"\
			   " your choice? (Whitaker, 1990, as quoted by vos Savant 1990a)"

#define NUM_DOORS 3U


/**
 * ************PROTOTYPES************
 */
unsigned int get_host_door(unsigned int doors[], unsigned int player_choice);
unsigned int rand_lim(unsigned int limit);
unsigned int get_rand_not(unsigned int limit, unsigned int not[], unsigned int not_size);
unsigned int get_other_door(unsigned int doors[], unsigned int firstdoor, unsigned int seconddoor);
void run_simulation(unsigned int count, bool switch_doors);

/**
 * ************FUNCTIONS************
 */

int main(int argc, char **argv)
{
	//set defaults--will be overwritten by getopt if there are any args
	opt.count = DEFAULT_COUNT;
	
	int long_index = 0;
	int c;
	while((c = getopt_long(argc, argv, "hc:", long_options, &long_index)) != -1)
	{
		switch(c)
		{
		case 'c':
			opt.count = strtoul(optarg, NULL, 10);
			break;
		case 'h':
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
	
	//run the simulation, first with switching doors and then without switching doors
	printf("\n**********MONTY HALL SIMULATION RESULTS**********\n");
	run_simulation(opt.count, TRUE);
	printf("\n*************************************************");
	run_simulation(opt.count, FALSE);
	
}

void run_simulation(unsigned int count, bool switch_doors)
{
	/*num_success = # of correct guesses (got the car)
	 * num_failure = # of incorrect guesses (got the goat)*/
	unsigned long num_success=0, num_failure=0;
	
	for(unsigned int i = 0; i < count; i++)
	{
		//TRUE = has the car, FALSE = has the goat
		
		unsigned int doors[NUM_DOORS] = {0, 0, 0};
		
		unsigned int car_index = rand_lim(2); //pick a random door to put the car behind
		doors[car_index] = TRUE; 
		
		//the player picks a random door
		unsigned int players_pick = rand_lim(NUM_DOORS-1); //NUM_DOORS - 1 because C counts up from 0
		
		//get the host's pick
		//it will be a door that isn't yours that reveals a goat
		unsigned int host_pick = get_host_door(doors, players_pick);
		
		//do you switch to the one that isn't yours and the host hasn't revealed?
		if(switch_doors)
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
	
	if(switch_doors)
		printf("\nThe player chose TO switch.");
	else
		printf("\nThe player chose NOT to switch.");
	printf("\nCOUNT = %d (number of times the simulation was run)", count);
	printf("\nNUM_SUCCESS = %lu", num_success);
	printf("\nNUM_FAILURE = %lu", num_failure);
	printf("\nPercent correct guesses (GOT THE CAR): %f", ((double)num_success)/count);
	printf("\nPercent incorrect guesses (GOT THE GOAT): %f\n", ((double)num_failure)/count);
}

unsigned int get_host_door(unsigned int doors[], unsigned int player_choice)
{
	//generate a completely random door and assert it meets these conditions
	unsigned int host_pick = rand_lim(NUM_DOORS); //DOORS_LIMIT == 2 because C counts up from 0
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
unsigned int get_rand_not(unsigned int limit, unsigned int not[], unsigned int not_size)
{
	unsigned int pick = rand_lim(limit);
	
	for(unsigned int i = 0; i < not_size; i++)
	{
		//if this set isn't good, recurse until we get a matching one
		if(pick == not[i])
			return get_rand_not(limit, not, not_size);
	}
	assert(pick < limit);
	return pick;
}

/**
 * get the door in doors[] that isn't firstdoor or seconddoor
 */
unsigned int get_other_door(unsigned int doors[], 
						unsigned int firstdoor, unsigned int seconddoor)
{
	assert(firstdoor <= DOORS_LIMIT);
	assert(seconddoor <= DOORS_LIMIT);
	//the parameter doors is only used in debug builds for static
	//assertion
	assert(doors[0] != doors[1] != doors[2]);

	//loop over NUM_DOORS (which is 3) NOT (NUM_DOORS-1) or else we cut off the last run of the for loop!
	for(unsigned int i = 0; i < NUM_DOORS; i++)
	{
		assert(firstdoor > -1); assert(firstdoor < 3);
		assert(secondoor > -1); assert(secondoor < 3);
		//test i not doors[i] because firstdoor and seconddoor are indices
		if((i != firstdoor) && (i != seconddoor))
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
unsigned int rand_lim(unsigned int limit) 
{
	//UB4MAXVAL= RAND_MAX
	double divisor = UB4MAXVAL/(limit+1);
	ub4 retval;
	
	do {
		retval = rand(&ctx) / divisor; //ISAAC redefines rand
	} while (retval > limit);

	return retval;
}
