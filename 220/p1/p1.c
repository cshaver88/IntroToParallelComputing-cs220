/* File:    p1.c
 * Author:  Cayla Shaver
 * Purpose: Calculate the temperature at gridpoints along an
 *			insulated metal rod with ice at each end, at various times.
 * 			*Compute Hot Dog Rule (Heat Equation).
 *
 * Input:   m = number of segments on the metal bar
 *			n = number of segments of time
 *			m + 1 = starting temperatures 
 * Output:  
 *
 * Compile: gcc -g -Wall -o p1 p1.c
 * Run:     ./p1
 *
 * Note:    Various times and temperatures may be different than 
 * 			they appear.
 * 			*The Hot Dog is strong in this code.
 */

 # include <stdio.h>
 # include <math.h>

void printValues (double new_u, double t_j);
//double copyValues(double old_u, double new_u);

int main(void) {
	const int MAX = 101; //max number of points
	int m; //number of segments between 0-1 (bar)
	int n; //number of segments between 0-1 (time)
	double d; //1.0/n  
	double h; //1.0/m
	int i; //counts through bar segments
	int j; //counts through time segments

	double old_u[MAX]; //previous line of info
	double new_u[MAX]; //current line of info

	double t_j; //segment loop below is currently working with

	printf("Please enter a value for m: ");
	scanf("%d", &m);

	printf("Please enter a value for n: ");
	scanf("%d", &n);

	d = 1.0/n; //1.0/n  
	h = 1.0/m; //1.0/m

	printf("Please enter some values for the initial temperatures: \n");

	old_u[0] = old_u[m] = 0.0;
	for (i = 0; i < m; i++) {
		scanf("%lf", &old_u[i]);
	}


	for (j = 1; j <= n; j ++){
		t_j = j * d;
		printf("%.3f--", t_j);
		new_u[0] = new_u[m] = 0.0;
		for (i = 1; i < m; i ++){
			new_u[i] = old_u[i] + d / (h * h) * (old_u[i - 1] - 2 * old_u[i] 
				+ old_u[i + 1]);
		}
		
		for (i = 0; i <= m; i ++){
			printf("%.3f ", new_u[i]);
			old_u[i] = new_u[i];
		}
		printf("\n");
	}
	   return 0;
}  /* main */