/*
	main.c
	Algorithms and Analysis: Assignment 2

	Pathfinding
	A-star Search

	You can copy across your uniform-cost search as a starting point.
*/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>

/* needed for the curses animation */
#include <curses.h>
#include <unistd.h>

#include "pq.h"

#define PROGRAM_NAME	"mappath"


/*
	Bit flags are used by the animation, can be used elsewhere.
	Set a bit flag to true with: flags |= BIT_FLAG
	Set a bit flag to false with: flags &= ~BIT_FLAG
	Check the value with flags & BIT_FLAG
*/
#define SQ_FLAG_VISITED    0x01
#define SQ_FLAG_ENQUEUED   0x02
#define SQ_FLAG_IMPASSABLE 0x04
#define SQ_FLAG_START      0x08
#define SQ_FLAG_GOAL       0x10
#define SQ_FLAG_PATH       0x20

struct square {
	unsigned int flags;	/* bit flags */
	char glyph;		/* symbol to display this square */
	int cost;/* cost for the edges to this square */
	int running_cost; /*total cost from start point */
	int path; /* the previous square */
	int x, y; /* square's x, y coordinate */
};
struct map {
	struct square *grid;	/* 2D grid of squares as an array */
	int width;		/* width of the map */
	int height;		/* height of the map */
	int *path;
	int path_count;
};

/* global for time interval and also whether to use curses animation */
int g_curses_interval = 0;



/* initialises the map structure */
struct map *map_init(int width, int height);

/* clears the map structure and frees any memory */
void map_destroy(struct map *map);

/* loads the map text file into a new map structure */
struct map *load_map(char *fname);

/* display the map to stdout */
void print_map(struct map *map);

/* sets the values for the grid square */
void set_square(struct square *sq, char c);

/* calculate a lowest cost path from (x0, y0) to (x1, y1) */
void make_path(struct map *map,
	int x0, int y0, int x1, int y1);
/* output the lowest cost path to stdout */
void print_path(struct map *map,
	int x0, int y0, int x1, int y1);

/* a naive path function that you need to replace */
void naive_path(struct map *map,
	int x0, int y0, int x1, int y1);
/* a naive path display function that you need to replace */
void print_naive_path(struct map *map,
	int x0, int y0, int x1, int y1);


/* curses animation display functions */
void init_curses(void);
void end_curses(void);
/* call this to draw the map after each step */
void curses_draw_map(struct map *map);

/* You may and are encouraged to add your own functions. */



/**************************************************************/
/* find the lowest cost path and mark it on the map */
void make_path(struct map *map,
	int x0, int y0, int x1, int y1)
{
	struct square *new_sq;
	struct priority_queue *pq;
	int y, x, position;
	int val, prio;
	int cost;
	int h_cost;
	
	/* check that the input start and finish squares are valid */
	if (map->grid[y0*map->width+x0].flags & SQ_FLAG_IMPASSABLE)
	{
		print_map(map);
		printf("Invalid start\n");
		return;
	}
	if (map->grid[y1*map->width+x1].flags & SQ_FLAG_IMPASSABLE)
	{
		print_map(map);
		printf("Invalid destination\n");
		return;
	}
	
	/* initialize all squares to unvisited flag and assign each square on the
	grid its x and y coordinate for future reference */
	for (x = 0; x < map->width; x++)
	{
		for (y = 0; y < map->height; y++)
		{
			map->grid[y*map->width+x].flags &= ~SQ_FLAG_VISITED;
			map->grid[y*map->width+x].x = x;
			map->grid[y*map->width+x].y = y;
		}
	}
	
	/* initialize all values to the start to begin traversing */
	y = y0;
	x = x0;
	val = 0;
	prio = 0;
	position = y0*map->width+x0;
	cost = 0;
	pq = pq_create();
	
	/* set start and destination squares with appropriate flags */
	map->grid[y0*map->width+x0].flags |= SQ_FLAG_START;
	pq_enqueue(pq, y0*map->width+x0, 0);
	map->grid[y0*map->width+x0].flags |= SQ_FLAG_ENQUEUED;
	map->grid[y0*map->width+x0].flags |= SQ_FLAG_VISITED;
	map->grid[y1*map->width+x1].flags |= SQ_FLAG_GOAL;
	curses_draw_map(map);
	
	curses_draw_map(map);
	
	/* check that priority queue is started */
	if (pq == NULL)
	{
		fprintf(stderr, "pq is empty");
		return;
	}
	
	/* until the destination square is reached, put all adjacent and valid squares 
	(x, y+1), (x, y-1), (x+1, y) and (x-1, y) into the priority queue with its 
	running cost as the priority, then set its flag as enqueued so that we will only 
	enqueue it again if the cost is less than its previously enqueued cost.
	set its path as the currently visited square so that we can go back when 
	completed */
	while (position != y1*map->width+x1)
	{
		new_sq = &(map->grid[(y+1)*map->width+x]);
		if (!(new_sq->flags & SQ_FLAG_IMPASSABLE) && !(new_sq->flags & SQ_FLAG_VISITED))
		{
			h_cost = abs(new_sq->x - x1) + abs (new_sq->y - y1);
			if ((new_sq->flags & SQ_FLAG_ENQUEUED &&\
				new_sq->running_cost > new_sq->cost+cost+h_cost) ||\
				!(new_sq->flags & SQ_FLAG_ENQUEUED))
			{
				new_sq->path = y*map->width+x;
				new_sq->running_cost = new_sq->cost+cost+h_cost;
				pq_enqueue(pq, (y+1)*map->width+x, new_sq->running_cost);
				new_sq->flags |= SQ_FLAG_ENQUEUED;
				curses_draw_map(map);
			}
		}
		new_sq = &(map->grid[(y-1)*map->width+x]);
		if (!(new_sq->flags & SQ_FLAG_IMPASSABLE) && !(new_sq->flags & SQ_FLAG_VISITED))
		{
			h_cost = abs(new_sq->x - x1) + abs (new_sq->y - y1);
			if ((new_sq->flags & SQ_FLAG_ENQUEUED &&\
				new_sq->running_cost > new_sq->cost+cost+h_cost) ||\
				!(new_sq->flags & SQ_FLAG_ENQUEUED))
			{
				new_sq->path = y*map->width+x;
				new_sq->running_cost = new_sq->cost+cost+h_cost;
				pq_enqueue(pq, (y-1)*map->width+x, new_sq->running_cost);
				new_sq->flags |= SQ_FLAG_ENQUEUED;
				curses_draw_map(map);
			}
		}
		new_sq = &(map->grid[y*map->width+x+1]);
		if (!(new_sq->flags & SQ_FLAG_IMPASSABLE) && !(new_sq->flags & SQ_FLAG_VISITED))
		{
			h_cost = abs(new_sq->x - x1) + abs (new_sq->y - y1);
			if ((new_sq->flags & SQ_FLAG_ENQUEUED &&\
				new_sq->running_cost > new_sq->cost+cost+h_cost) ||\
				!(new_sq->flags & SQ_FLAG_ENQUEUED))
			{
				new_sq->path = y*map->width+x;
				new_sq->running_cost = new_sq->cost+cost+h_cost;
				pq_enqueue(pq, y*map->width+x+1, new_sq->running_cost);
				new_sq->flags |= SQ_FLAG_ENQUEUED;
				curses_draw_map(map);
			}
		}
		new_sq = &(map->grid[y*map->width+x-1]);
		if (!(new_sq->flags & SQ_FLAG_IMPASSABLE) && !(new_sq->flags & SQ_FLAG_VISITED))
		{
			h_cost = abs(new_sq->x - x1) + abs (new_sq->y - y1);
			if ((new_sq->flags & SQ_FLAG_ENQUEUED &&\
				new_sq->running_cost > new_sq->cost+cost+h_cost) ||\
				!(new_sq->flags & SQ_FLAG_ENQUEUED))
			{
				new_sq->path = y*map->width+x;
				new_sq->running_cost = new_sq->cost+cost+h_cost;
				pq_enqueue(pq, y*map->width+x-1, new_sq->running_cost);
				new_sq->flags |= SQ_FLAG_ENQUEUED;
				curses_draw_map(map);
			}
		}
		
		/* dequeue the lowest cost square and that is the next square visited and
		update the total cost */
		pq_dequeue(pq, &val, &prio);
		position = val;
		new_sq = &(map->grid[position]);
		new_sq->flags |= SQ_FLAG_VISITED;
		y = new_sq->y;
		x = new_sq->x;
		cost = cost + new_sq->cost;
		curses_draw_map(map);
	}
	
	/* starting from the destination, go back to each square's path to determine
	the optimal path from start to destination */
	while (position != y0*map->width+x0)
	{
		map->path[map->path_count] = position;
		map->path_count++;
		position = new_sq->path;
		new_sq = &(map->grid[position]);
		new_sq->flags |= SQ_FLAG_PATH;
		curses_draw_map(map);
	}
}	

/* print the lowest cost path to stdout*/
void print_path(struct map *map, int x0, int y0, int x1, int y1)
{
	int cost = 0;
	int x, y, position, i;
	struct square *sq;
	
	/* if start or destination squares are impassable, print 'no path found' */
	if ((map->grid[y0*map->width+x0].flags & SQ_FLAG_IMPASSABLE) ||\
		(map->grid[y1*map->width+x1].flags & SQ_FLAG_IMPASSABLE))
	{
		printf("No path found\n");
		return;
	}
	
	/* mark an 'o' on squares on the optimal path */
	for (x = 0; x < map->width; x++)
	{
		for (y = 0; y < map->height; y++)
		{
			if (map->grid[y*map->width+x].flags & SQ_FLAG_PATH)
			{
				map->grid[y*map->width+x].glyph = 'o';
			}
		}
	}
	
	/* mark start with an 'A' and destination with a 'B' */
	map->grid[y0*map->width+x0].glyph = 'A';
	map->grid[y1*map->width+x1].glyph = 'B';
	
	print_map(map);
	
	/* from start to destination, print out the x, y coordinate of the optimal path and 
	calculate the total cost */
	position = y0*map->width+x0;
	sq = &(map->grid[position]);
	x = sq->x;
	y = sq->y;
	
	printf("(%d, %d) ", x, y);
	
	for (i = map->path_count-1; i >= 0; i--)
	{
		position = map->path[i];
		sq = &(map->grid[position]);
		cost = cost + sq->cost;
		x = sq->x;
		y = sq->y;
		printf("-> (%d, %d) ", x, y);
	}
	printf("\nPath Cost: %d\n", cost);
}

/* sets the values for the grid square */
void set_square(struct square *sq, char c)
{
	sq->glyph = c;
	sq->flags = 0;
	switch (c) {
	case '.':
		sq->cost = 1;
		break;
	case ':':
		sq->cost = 2;
		break;
	case '=':
		sq->cost = 5;
		break;	
	case '+':
		sq->cost = 12;
		break;	
	case '#':
	default:
		/* impassable */
		sq->cost = -1;	/* set to invalid */
		sq->flags |= SQ_FLAG_IMPASSABLE;
		break;
	}
	/*
		If you add more variables to the square struct, 
		initialise them here.
	*/
}
/**************************************************************/

/* initialises the map structure */
struct map *map_init(int width, int height)
{
	struct map *new_map;
	struct square *sq;
	int i;

	new_map = malloc(sizeof(struct map));
	if (!new_map) {	return NULL; }

	new_map->grid = malloc(sizeof(struct square) * width * height);
	if (!new_map->grid) {return NULL;}

	new_map->width = width;
	new_map->height = height;

	new_map->path = malloc(sizeof(int) * width * height);
	if (!new_map->path) {return NULL;}
	
	new_map->path_count = 0;
	
	sq = new_map->grid;
	for (i = 0; i < width * height; i++) {
		set_square(sq, '#');	/* impassable by default */
		sq++;
	}

	return new_map;
}

/* clears the map structure and frees any memory */
void map_destroy(struct map *map)
{
	if (map) {
		free(map->grid);
		free(map);
	}
}

/* loads the map text file into a new map structure */
struct map *load_map(char *fname)
{
	struct map *map = NULL;
	struct square *sq;
	FILE *fp;
	int width, height;
	int n;
	char c;

	/* You should not need to have to alter this function. */

	if ((fp = fopen(fname, "r")) == NULL) {
		fprintf(stderr,"Error: cannot open file %s.\n",	fname);
		exit(EXIT_FAILURE);
	}

	/* first pass: calcuate width and height of map */
	width = height = n = 0;
	while ((c = getc(fp)) != EOF) {
		if (c == '\n') {
			height++;
			if (n > width) { width = n; }
			n = 0;
		} else {
			n++;
		}
	}

	/* rewind file */
	if (fseek(fp, SEEK_SET, 0)) { return NULL; }

	/* pad out the map to make a sentinel border around the edge */
	width += 2;
	height += 2;

	/* now we can allocate memory to the map structure */
	map = map_init(width, height);
	if (!map) {
		fprintf(stderr, "Cannot allocate memory for map structure.\n");
		exit(EXIT_FAILURE);
	}

	/* second pass: load in the data */
	sq = map->grid + width + 1;	/* skip first row as a border */
	while ((c = getc(fp)) != EOF) {
		if (c == '\n') {
			sq += 2;
		} else {
			set_square(sq, c);
			sq++;
		}
	}

	fclose(fp);

	return map;
}

/* display the map to stdout */
void print_map(struct map *map)
{
	int x, y, i;
	struct square *sq;

	if (!map) { return; }

	i = map->width;	/* skip the sentinel border */
	for (y = 1; y < map->height - 1; y++) {
		i++;	/* skip the left edge */
		for (x = 1; x < map->width - 1; x++) {
			sq = map->grid + i;
			putchar(sq->glyph);
			i++;
		}
		putchar('\n');
		i++;	/* skip the right edge */
	}
}


/*
	Naive Path Code: placeholders to give demo output

	This is placeholder code that does not find the
	lowest cost path. You will need to not call this function
	and instead write make_path with your own implementation
	of the algorithm.

	The code here is extremely simple just to produce output.
	This is not an accurate guide for how your algorithm should
	work!
*/
void naive_path(struct map *map,
	int x0, int y0, int x1, int y1)
{
	int i, j;

	/* set flags for the goal and start squares */
	map->grid[y0*map->width+x0].flags |= SQ_FLAG_START;
	map->grid[y1*map->width+x1].flags |= SQ_FLAG_GOAL;

	curses_draw_map(map);

	i = x0;
	j = y0;
	while ((i != x1) || (j != y1)) {
		if (i > x1) { i--; }
		else if (i < x1) { i++; }
		else if (j > y1) { j--; }
		else if (j < y1) { j++; }
		map->grid[j*map->width+i].flags |= SQ_FLAG_ENQUEUED;
		curses_draw_map(map);
		map->grid[j*map->width+i].flags &= ~SQ_FLAG_ENQUEUED;
		map->grid[j*map->width+i].flags |= SQ_FLAG_VISITED;
	}

	/* finally mark the "path" */
	i = x0;
	j = y0;
	while ((i != x1) || (j != y1)) {
		if (i > x1) { i--; }
		else if (i < x1) { i++; }
		else if (j > y1) { j--; }
		else if (j < y1) { j++; }
		map->grid[j*map->width+i].flags |= SQ_FLAG_PATH;
	}
	curses_draw_map(map);


}
/*
	This is also a placeholder that does not print the
	correct lowest cost path. You will need to write your own
	in print_path that outputs the correct path in the same
	format.
	Part of this is coding a method for storing the path.
*/
void print_naive_path(struct map *map, int x0, int y0, int x1, int y1)
{
	int cost = 0;
	int i, j;
	int width = map->width;
	struct square *sq;

	/*
	note: this naive path is all one function, but your own
	path algorithm may be easier to write as multiple functions
	*/

	/* for our naive path, head west/east first then north/south */
	i = x0;
	j = y0;
	while ((i != x1) || (j != y1)) {
		if (i > x1) { i--; }
		else if (i < x1) { i++; }
		else if (j > y1) { j--; }
		else if (j < y1) { j++; }
		sq = map->grid + j * width + i;
		sq->glyph = 'o';
		cost += sq->cost;
	}

	/* mark the start and end positions */
	map->grid[y0*width+x0].glyph = 'A';
	map->grid[y1*width+x1].glyph = 'B';

	/* you will need to calculate the path cost. */

	/* output the map */
	print_map(map);

	/* for our naive path, head west/east first then north/south */
	i = x0;
	j = y0;
	while ((i != x1) || (j != y1)) {
		printf("(%d, %d) -> ", i, j);
		if (i > x1) { i--; }
		else if (i < x1) { i++; }
		else if (j > y1) { j--; }
		else if (j < y1) { j++; }
	}

	printf("(%d, %d)\n", x1, y1);	

	if (cost >= 0) {
		printf("Path cost: %d\n", cost);
	} else {
		printf("No path found.\n");
	}
}

/*****************************
  curses animation functions
*****************************/

/* Initialise curses, set the colour pairs */
void init_curses(void)
{
	if (!g_curses_interval) { return; }
	initscr();
	start_color();
	init_pair(0, COLOR_WHITE, COLOR_BLACK); /* normal */
	init_pair(1, COLOR_RED, COLOR_BLACK);   /* visited */
	init_pair(2, COLOR_CYAN, COLOR_BLACK);  /* enqueued */
	init_pair(3, COLOR_GREEN, COLOR_BLACK); /* start */
	init_pair(4, COLOR_YELLOW, COLOR_BLACK); /* goal */
	init_pair(5, COLOR_BLUE, COLOR_BLUE); /* impassable */
	init_pair(6, COLOR_GREEN, COLOR_BLACK); /* on path */

	atexit((void(*)())endwin);
}

/* Do any cleanup needed, provide a system pause at the end */
void end_curses(void)
{
	if (!g_curses_interval) { return; }
	getchar();	/* pause at end */
	/* endwin will be called at exit */
}

/* Call this function to draw the map after every step */
void curses_draw_map(struct map *map)
{
	int i,j;
	struct square *sq;
	if (!g_curses_interval) { return; }
	if (map == NULL) { return; }
	sq = map->grid + map->width;
	for (j = 1; j < map->height-1; j++) {
		move(j-1, 0);
		sq++;
		for (i = 1; i < map->width-1; i++) {
			char ch = sq->glyph;
			int color = 0;

			if (sq->flags & SQ_FLAG_IMPASSABLE) {
				color = 5;
			}
			if (sq->flags & SQ_FLAG_VISITED) {
				color = 1;
			}
			if (sq->flags & SQ_FLAG_ENQUEUED) {
				color = 2;
				ch = 'o';
			}
			if (sq->flags & SQ_FLAG_PATH) {
				color = 6;
				ch = 'o';
			}
			if (sq->flags & SQ_FLAG_START) {
				color = 3;
				ch = 'A';
			}
			if (sq->flags & SQ_FLAG_GOAL) {
				color = 4;
				ch = 'B';
			}

			if (sq->flags & ~SQ_FLAG_IMPASSABLE) { attron(A_BOLD); }

			attron(COLOR_PAIR(color));
			addch(ch);
			attroff(COLOR_PAIR(color));

			if (sq->flags & ~SQ_FLAG_IMPASSABLE) { attroff(A_BOLD); }

			sq++;
		}
		sq++;
	}
	move(j, 0);
	refresh();
	usleep(g_curses_interval);
}

/*****************************
  main function
*****************************/
int main(int argc, char **argv)
{
	struct map *map;
	int x0, y0, x1, y1;

	/* parse args */
	if (argc < 6) {
		fprintf(stderr, "%s needs at least 5 arguments (%d were given)\n",
			PROGRAM_NAME, argc-1);
		fprintf(stderr, "usage: %s mapfile x0 y0 x1 y1 (time_interval - optional)\n",
			PROGRAM_NAME);
		exit(EXIT_FAILURE);
	}
	x0 = atoi(argv[2]);
	y0 = atoi(argv[3]);
	x1 = atoi(argv[4]);
	y1 = atoi(argv[5]);
	g_curses_interval = 0;
	if (argc > 6) { g_curses_interval = atoi(argv[6]); }

	map = load_map(argv[1]);
	if (!map) {
		fprintf(stderr, "Error loading map.\n");
		exit(EXIT_FAILURE);
	}
	if ((x0 <= 0 || x0 >= map->width-1) ||
		(x1 <= 0 || x1 >= map->width-1) ||
		(y0 <= 0 || y0 >= map->height-1) ||
		(y1 <= 0 || y1 >= map->height-1)) {
		fprintf(stderr, "Coordinates are not in range.\n");
		exit(EXIT_FAILURE);		
	}

	/* initialise the curses animation if we're using that */
	if (g_curses_interval > 0) {
		init_curses();
	}

	make_path(map, x0, y0, x1, y1);

	if (g_curses_interval > 0) {
		end_curses();
	} else {
		print_path(map, x0, y0, x1, y1);
	}

	map_destroy(map);

	return EXIT_SUCCESS;
}



