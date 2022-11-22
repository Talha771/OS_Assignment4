/*
 * Street synchronization problem code
 */

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/* Constants for simulation */

#define ALLOWED_CARS 3          	/* Number of cars allowed on street at a time */
#define USAGE_LIMIT 7   			/* Number of times the street can be used before repair */
#define MAX_CARS 1000       		/* Maximum number of cars in the simulation */

#define INCOMING "Incoming"
#define OUTGOING "Outgoing"

/* Add your synchronization variables here */

/* These obvious variables are at your disposal. Feel free to remove them if you want */
static int cars_on_street;   		/* Total numbers of cars currently on the street */
static int incoming_onstreet;      	/* Total numbers of cars incoming on the street */
static int outgoing_onstreet;      	/* Total numbers of cars outgoing on the street */
static int cars_since_repair;		/* Total numbers of cars entered since the last repair */


typedef struct {
          int arrival_time;  	// time between the arrival of this car and the previous car
          int travel_time; 		// time the car takes to travel on the street
          char car_direction [20];
          int car_id;
  } car;

/* Called at the starting of simulation.  Initialize all synchronization
 * variables and other global variables that you add.
 */
static int
initialize(car *arr, char *filename) {

	cars_on_street = 0;
	incoming_onstreet = 0;
	outgoing_onstreet = 0;
	cars_since_repair = 0;

	/* Initialize your synchronization variables (and 
         * other variables you might use) here
	 */

        /* Read in the data file and initialize the car array */
        FILE *fp;

        if((fp=fopen(filename, "r")) == NULL) {
          printf("Cannot open input file %s for reading.\n", filename);
          exit(1);
        }
        int i =0;
        while ( (fscanf(fp, "%d%d%s\n", &(arr[i].arrival_time), &(arr[i].travel_time), arr[i].car_direction)!=EOF) && i < MAX_CARS ) {
             i++;
        }
        fclose(fp);
        return i;
}

/* Code executed by street on the event of repair 
 * Do not add anything here.  
 */
static void 
repair_street() {
	printf("The street is being repaired now.\n");
	sleep(5);
}

/* Code for the street which repairs it when necessary and is cyclic. Needs to be synchronized
 * with the cars. See the comments within the function for details.
 */
void *street_thread(void *junk) {

		/* YOUR CODE HERE. */
	printf("The street is ready to use\n");

	/* Loop while waiting for cars to arrive. */
	while (1) {

		/* YOUR CODE HERE. */
                /* Currently the body of the loop is empty. There's		*/
                /* no communication between street and cars, i.e. all	*/
                /* cars are admitted without regard of the allowed		*/ 
                /* limit, which direction a car is going, and whether	*/
                /* the street needs to be repaired          			*/

		repair_street();

	}

	pthread_exit(NULL);
}


/* Code executed by an incoming car to enter the street.
 * You have to implement this.
 */
void
incoming_enter() {
        /* You might want to add synchronization for the simulations variables	*/
	/* 
	 *  YOUR CODE HERE. 
	 */
}

/* Code executed by an outgoing car to enter the street.
 * You have to implement this.
 */
void
outgoing_enter() {
        /* You might want to add synchronization for the simulations variables	*/
	/* 
	 *  YOUR CODE HERE. 
	 */
}

/* Code executed by a car to simulate the duration for travel
 * You do not need to add anything here.  
 */
static void 
travel(int t) {
	sleep(t);
}


/* Code executed by an incoming car when leaving the street.
 * You need to implement this.
 */
static void 
incoming_leave() {
	/* 
	 *  YOUR CODE HERE. 
	 */
}


/* Code executed by an outgoing car when leaving the street.
 * You need to implement this.
 */
static void 
outgoing_leave() {
	/* 
	 *  YOUR CODE HERE. 
	 */
}

/* Main code for incoming car threads.  
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void*
incoming_thread(void *arg) {
	car *car_info = (car*)arg;

	/* enter street */
	incoming_enter();
	
        /* Car travel --- do not make changes to the 3 lines below*/
	printf("Incoming car %d has entered and travels for %d minutes\n", car_info->car_id, car_info->travel_time);
	travel(car_info->travel_time);
	printf("Incoming car %d has travelled and prepares to leave\n", car_info->car_id);

	/* leave street */
	incoming_leave();  

	pthread_exit(NULL);
}

/* Main code for outgoing car threads.  
 * You do not need to change anything here, but you can add
 * debug statements to help you during development/debugging.
 */
void*
outgoing_thread(void *arg) {
	car *car_info = (car*)arg;

	/* enter street */
	outgoing_enter();
	
        /* Car travel --- do not make changes to the 3 lines below*/
	printf("Outgoing car %d has entered and travels for %d minutes\n", car_info->car_id, car_info->travel_time);	
	travel(car_info->travel_time);
	printf("Outgoing car %d has travelled and prepares to leave\n", car_info->car_id);

	/* leave street */
	outgoing_leave();

	pthread_exit(NULL);
}

/* Main function sets up simulation and prints report
 * at the end.
 */
int main(int nargs, char **args) {
	int i;
	int result;
	int num_cars;
        void *status;
        pthread_t street_tid;
        pthread_t car_tid[MAX_CARS];
        car car_info[MAX_CARS];

	if (nargs != 2) {
		printf("Usage: traffic <name of inputfile>\n");
		return EINVAL;
	}

	num_cars = initialize(car_info, args[1]);
	if (num_cars > MAX_CARS || num_cars <= 0) {
		printf("Error:  Bad number of car threads. Maybe there was a problem with your input file?\n");
		return 1;
	}

	printf("Beginning traffic simulation with %d cars ...\n",
		num_cars);

	result = pthread_create(&street_tid, NULL, street_thread, NULL);
	if (result) {
		printf("traffic:  pthread_create failed for street: %s\n", strerror(result));
                exit(1);
	}

	for (i=0; i < num_cars; i++) {

		car_info[i].car_id = i;
		sleep(car_info[i].arrival_time);

		if (strcmp (car_info[i].car_direction, INCOMING)==0)
			result = pthread_create(&car_tid[i], NULL, incoming_thread, (void *)&car_info[i]);
		else // car is outgoing
			result = pthread_create(&car_tid[i], NULL, outgoing_thread, (void *)&car_info[i]);

		if (result) {
			printf("traffic: thread_fork failed for car %d: %s\n", 
			      i, strerror(result));
                        exit(1);
		}

	}

	/* wait for all car threads to finish */
	for (i = 0; i < num_cars; i++) 
		pthread_join(car_tid[i], &status);

	/* terminate the street thread. */
	pthread_cancel(street_tid);

	printf("Traffic simulation complete.\n");

	return 0;
}
