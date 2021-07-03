#include <stdlib.h> // atoi, rand, qsort, malloc
#include <stdio.h>
#include <assert.h> // assert
#include <time.h> //time

#define RANGE 100.0

typedef struct
{
	float x;
	float y;
} point;

typedef struct
{
	point from;
	point to;
} line_segment;

////////////////////////////////////////////////////////////////////////////////
// function declaration
// convex hull 함수 (재귀호출)
int convex_hull_main(point *p, int n, point p1, point pn, line_segment *l, int *num_l);

// 직선(ax+by-c=0)과 주어진 점 p 간의 거리
// distance = | ( ax+by-c ) / sqrt(a^2 + b^2) |
float distance(float a, float b, float c, point p);

// 두 점(from, to)을 연결하는 직선(ax + by - c = 0)으로 n개의 점들의 집합(p)를 s1(점의 수 n1)과 s2(점의 수 n2)로 분리하는 함수
// p1 : 직선의 upper(left)에 속한 점들은 ax+by-c가 minus임
// p2 : lower(right)에 속한 점들은 ax+by-c가 plus임
void separate_points(point *p, int n, point from, point to, point *s1, point *s2, int *n1, int *n2);

////////////////////////////////////////////////////////////////////////////////
void print_header(char *filename)
{
	printf("#! /usr/bin/env Rscript\n");
	printf("png(\"%s\", width=700, height=700)\n", filename);

	printf("plot(1:%.3f, 1:%.3f, type=\"n\")\n", RANGE, RANGE);
}

////////////////////////////////////////////////////////////////////////////////
void print_footer(void)
{
	printf("dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
// qsort를 위한 비교 함수
int cmp_x(const void *p1, const void *p2)
{
	point *p = (point *)p1;
	point *q = (point *)p2;

	float diff = p->x - q->x;

	return ((diff >= 0.0) ? ((diff > 0.0) ? +1 : 0) : -1);
}

////////////////////////////////////////////////////////////////////////////////
void print_points(point *p, int n)
{
	int i;
	printf("\n#points\n");

	for (i = 0; i < n; i++)
		printf("points(%.3f,%.3f)\n", p[i].x, p[i].y);
}

////////////////////////////////////////////////////////////////////////////////
void print_line_segments(line_segment *l, int num_line)
{
	int i;

	printf("\n#line segments\n");

	for (i = 0; i < num_line; i++)
		printf("segments(%.3f,%.3f,%.3f,%.3f)\n", l[i].from.x, l[i].from.y, l[i].to.x, l[i].to.y);
}

////////////////////////////////////////////////////////////////////////////////
// [input] p : set of points
// [input] n : number of points
// [output] l : set of line segments that forms the convex hull
// return value : number of line segments
int convex_hull(point *p, int n, int min_index, int max_index, line_segment *l)
{
	int num_l = 0;
	int n1, n2;
	
	if(n == 1) return 0;
	
	point *s1 = (point *)malloc(sizeof(point) * n);
	assert(s1 != NULL);

	point *s2 = (point *)malloc(sizeof(point) * n);
	assert(s2 != NULL);

	separate_points( p, n, p[min_index], p[max_index], s1, s2, &n1, &n2);
	//print_points(s1, n1);
	//print_points(s2, n2);
	
	convex_hull_main( s1, n1, p[min_index], p[max_index], l, &num_l);
	convex_hull_main( s2, n2, p[max_index], p[min_index], l, &num_l);

	free(s1);
	free(s2);

	return num_l;
}

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	float x, y;
	int n; // number of points

	if (argc != 2)
	{
		printf("%s number_of_points\n", argv[0]);
		return 0;
	}

	n = atoi(argv[1]);
	if (n <= 0)
	{
		printf("The number of points should be a positive integer!\n");
		return 0;
	}

	point *p;
	p = (point *)malloc(sizeof(point) * n);
	assert(p != NULL);

	line_segment *l;
	l = (line_segment *)malloc(sizeof(line_segment) * n);
	assert(l != NULL);

	fprintf(stderr, "Create %d points\n", n);

	float min_x = RANGE;
	float max_x = 0;
	int min_index = 0;
	int max_index = 0;

	// making n points
	srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		x = (float)rand() / (float)(RAND_MAX)* RANGE; // 0 ~ RANGE random float number
		y = (float)rand() / (float)(RAND_MAX)* RANGE; // 0 ~ RANGE random float number

		p[i].x = x;
		p[i].y = y;

		if (x < min_x) { min_x = x; min_index = i; }
		if (x > max_x) { max_x = x; max_index = i; }
	}

	// sort by x-axis
	//qsort( p, n, sizeof(point), cmp_x);

	print_header("convex.png");

	print_points(p, n);

	// convex hull algorithm
	int num_line = convex_hull( p, n, min_index, max_index, l);

	print_line_segments( l, num_line);

	print_footer();

	free(p);
	free(l);

	return 0;
}


////////////////////////////////////////////////////////////////////////////////
// function declaration
// convex hull 함수 (재귀호출)
int convex_hull_main(point *p, int n, point p1, point pn, line_segment *l, int *num_l) {
	if (n == 0) {
		l[*num_l].from = p1;
		l[*num_l].to = pn;
		(*num_l)++;
		return 0;
	}
   
	float a = pn.y - p1.y;
	float b = p1.x - pn.x;
	float c = p1.x * pn.y - p1.y * pn.x;
   
	float dmax = 0;
	int max_i = 0;
   
	for (int i = 0;i < n;i++) { //pmax찾기
		int d = distance(a, b, c, p[i]); // int로 안바꾸면 에러남, 100을 곱한 값이어서 int로 바꿔도 큰 오차 없음
		if (dmax > d) { //왼쪽 중 d가 가장 작은 값이 가장 멀리 있는 값
			dmax = d;
			max_i = i;
		}
    }   
	
    int n1, n2, n0; //n0, s0 버리는 값
	point *s1 = (point *)malloc(sizeof(point) * n);
	assert(s1 != NULL);
	point *s2 = (point *)malloc(sizeof(point) * n);
	assert(s2 != NULL);
	point *s0 = (point *)malloc(sizeof(point) * n);
	assert(s0 != NULL);
	
	separate_points(p, n, p1, p[max_i], s1, s0, &n1, &n0);
	separate_points(p, n, p[max_i], pn, s2, s0, &n2, &n0);

	convex_hull_main(s1, n1, p1, p[max_i], l, num_l);
	convex_hull_main(s2, n2, p[max_i], pn, l, num_l);
	
	free(s1);
	free(s2);
	free(s0);
   
	return 0;
}

// 직선(ax+by-c=0)과 주어진 점 p 간의 거리
// distance = | ( ax+by-c ) / sqrt(a^2 + b^2) |
float distance(float a, float b, float c, point p) {
	float d = (a * p.x + b * p.y - c); 
	return d * 100; //소수점 두자리 남기기 위해 100을 곱함
}

// 두 점(from, to)을 연결하는 직선(ax + by - c = 0)으로 n개의 점들의 집합(p)를 s1(점의 수 n1)과 s2(점의 수 n2)로 분리하는 함수
// p1 : 직선의 upper(left)에 속한 점들은 ax+by-c가 minus임
// p2 : lower(right)에 속한 점들은 ax+by-c가 plus임
void separate_points(point *p, int n, point from, point to, point *s1, point *s2, int *n1, int *n2) {
	float a, b, c;
	int j = 0;
	int k = 0;
   
	a = to.y - from.y;
	b = from.x - to.x;
	c = from.x * to.y - from.y * to.x;
   
	for(int i = 0;i < n;i++){
		int d = distance ( a, b, c, p[i]); // int로 안바꾸면 에러남, 100을 곱한 값이어서 int로 바꿔도 큰 오차 없음
		if (d < 0){
			s1[j] = p[i];
			j++;
		}
		if (d > 0){
			s2[k] = p[i];
			k++;
		}
	}
	*n1 = j;
	*n2 = k;
   
	return;
}
