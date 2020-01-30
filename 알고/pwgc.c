#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph(int graph[][16], int num);

// 주어진 상태 state의 마지막 4비트를 파일에 출력 (화면 출력은 stdout)
void bit_print(FILE *fp, int state);

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
void get_pwgc(int state, int *p, int *w, int *g, int *c);

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph(char *filename, int graph[][16], int num);

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
int is_dead_end(int state);

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
int is_possible_transition(int state1, int state2);

// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix(int graph[][16]);

// 상태 변경: 농부 이동
// return value : 새로운 상태
int changeP(int state);

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태
int changePW(int state);

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태
int changePG(int state);

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태
int changePC(int state);

// 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
int is_visited(int *visited, int level, int state);

// 방문한 상태들을 차례로 화면에 출력
void print_states(int *visited, int count);

// recursive function
void dfs_main(int state, int goal_state, int level, int *visited);

// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search(int init_state, int goal_state);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	int graph[16][16] = { 0, };

	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력
	//print_graph( graph, 16);

	// .net 파일 만들기 pajek
	save_graph( "pwgc.net", graph, 16);

	//////////////////////////////////////////
	depth_first_search( 0, 15); // initial state, goal state

	return 0;
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph(int graph[][16], int num) {

	for (int i = 0;i < num;i++) {
		for (int j = 0;j < num;j++) {
			printf("%d\t", graph[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

// 주어진 상태 state의 마지막 4비트를 파일에 출력 (화면 출력은 stdout)
// 1이랑 & 했을때 0이면 원래도 0, 1이면 원래도 1
void bit_print(FILE *fp, int state) {
	int p, w, g, c;
	
	get_pwgc(state, &p, &w, &g, &c);
	
	fprintf(fp, "\"%d%d%d%d\"\n", p, w, g, c);
}

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
void get_pwgc(int state, int *p, int *w, int *g, int *c) {

	*p = (1 << 3) & state ? 1 : 0;
	*w = (1 << 2) & state ? 1 : 0;
	*g = (1 << 1) & state ? 1 : 0;
	*c = (1 << 0) & state ? 1 : 0;
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph(char *filename, int graph[][16], int num) {
	FILE* fp;

	fp = fopen(filename, "w");

	fprintf(fp, "*Vertices %d\n", num);

	for (int i = 0; i < num;i++) {
		fprintf(fp, " %2d ", i + 1);
		bit_print(fp, i);
	}

	fprintf(fp, "*Edges\n");

	for (int i = 0;i < num / 2;i++) {
		for (int j = 0;j < num;j++) {
			if (graph[i][j] == 1) {
				fprintf(fp, "  %d  %d\n", i + 1, j + 1);
			}
		}
	}
	
	fclose(fp);
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 0110(6), 0111(7), 1001(9), 1000(8)/ 농부없이 염소와 양배추가 같이 있는 경우 0011(3), 1100(12)
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
int is_dead_end(int state) {
	int p, w, g, c;

	get_pwgc(state, &p, &w, &g, &c);
	
	if (w == g && p != w) return 1;
	else if (g == c && p != g) return 1;
	else return 0;
}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용/ 농부만 flip, 농부랑 다른 하나만 flip
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
int is_possible_transition(int state1, int state2) {
	int p1, w1, g1, c1;
	int p2, w2, g2, c2;

	get_pwgc(state1, &p1, &w1, &g1, &c1);
	get_pwgc(state2, &p2, &w2, &g2, &c2);

	if (is_dead_end(state1) || is_dead_end(state2)) return 0;
	else if (p1 == p2) return 0;
	else if (w1 != w2 && g1 != g2) return 0;
	else if (w1 != w2 && c1 != c2) return 0;
	else if (c1 != c2 && g1 != g2) return 0;
	else return 1;
}

// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix(int graph[][16]) {

	for (int i = 0;i < 16;i++) {
		for (int j = 0;j < 16;j++) {
			graph[i][j] = is_possible_transition(i, j);
		}
	}
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
int changeP(int state) {
	state ^= 8; //1000
	return state;
}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태
int changePW(int state) {
	state ^= 12; //1100
	return state;
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태
int changePG(int state) {
	state ^= 10; //1010
	return state;
}

// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태
int changePC(int state) {
	state ^= 9; //1001
	return state;
}

// 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
int is_visited(int *visited, int level, int state){
	for (int i = 0;i < level;i++) {
		if (state == visited[i]) return 1;
	}
	return 0;
}

// 방문한 상태들을 차례로 화면에 출력
void print_states(int *visited, int count){
	int p, w, g, c;

	for (int i = 0;i < count;i++) {
		get_pwgc(visited[i], &p, &w, &g, &c);
		printf("%d%d%d%d\n", p, w, g, c);
	}
	printf("\n");
}

// recursive function
void dfs_main(int state, int goal_state, int level, int *visited){
	if (state == goal_state) {
		visited[level] = state;
		printf("Goal-state found!\n");
		print_states(visited, level + 1);
		return;
	}
	
	visited[level] = state;

	for (int i = 0;i < 16;i++) {
		if (is_possible_transition(state, i)) {
			if(is_visited(visited, level, i)) continue;
			dfs_main(i, goal_state, level + 1, visited);
		}
	}
	return;
}

// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search(int init_state, int goal_state){
	int visited[16] = { 0, };
	dfs_main(init_state, goal_state, 0, visited);
}

