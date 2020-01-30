#include <stdlib.h> // atoi, rand
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> //time

#define RANGE 100

typedef struct
{
	int x;
	int y;
} point;

typedef struct
{
	point from;
	point to;
} line_segment;

////////////////////////////////////////////////////////////////////////////////
void print_header(char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer(void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
void print_points( point *p, int n);

// [input] p : set of points
// [input] n : number of points
// [output] l : set of line segments that form the convex hull
// return value : number of line segments
int convex_hull( point *p, int n, line_segment *l);

void print_line_segments( line_segment *l, int num_line);

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int n; // number of points
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	n = atoi( argv[1]);
	if (n <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	point p[n];
	line_segment l[n];

	fprintf( stderr, "Create %d points\n", n);

	// making n points
	srand( time(NULL));
	for (int i = 0; i < n; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
		
		p[i].x = x;
		p[i].y = y;
 	}

	print_header( "convex.png");
	
	print_points( p, n);
	
	int num_line = convex_hull( p, n, l);

	print_line_segments( l, num_line);
	
	print_footer();
	
	return 0;
}


void print_points(point *p, int n) {
	printf("\n#points\n");

	for (int i = 0;i < n;i++) {
		printf("points(%d, %d)\n", p[i].x, p[i].y);
	}
}

// [input] p : set of points
// [input] n : number of points
// [output] l : set of line segments that form the convex hull
// return value : number of line segments
int convex_hull(point *p, int n, line_segment *l) {

	int a, b, c;
	int side = 0, curSide = 0;
	int num = 0;

	for (int i = 0;i < n - 1;i++) {
		for (int j = i + 1;j < n;j++) {
			a = p[j].y - p[i].y;
			b = p[i].x - p[j].x;
			c = p[i].x*p[j].y - p[i].y*p[j].x;

			int flag = 0;

			for (int k = 0;k < n && flag != -1;k++) {
				if (k == i || k == j) continue;

				curSide = a * p[k].x + b * p[k].y - c;

				if (curSide != 0 && flag == 0) {
					side = curSide;
					flag = 1;
				}
				else if (curSide != 0) {
					if (curSide * side < 0) {
						flag = -1;
					}
				}
				else continue;
			}
			if (flag != -1) {
				l[num].from = p[i];
				l[num].to = p[j];
				num++;
			}
		}
	}

	return num;
}

void print_line_segments(line_segment *l, int num_line) {
	printf("\n#line segments\n");

	for (int i = 0;i < num_line;i++) {
		printf("segments(%d, %d, %d, %d)\n", l[i].from.x, l[i].from.y, l[i].to.x, l[i].to.y);
	}
}

