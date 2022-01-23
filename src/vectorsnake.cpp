#include <ncurses.h>
#include <ctime>
#include <random>
#include <chrono>
#include <stdio.h>
#include <vector>
#define LENGTH 1000000
struct body
{
	int x;
	int y;
};
auto begin = std::chrono::steady_clock::now();
auto end = std::chrono::steady_clock::now();
int buf[40];
body field;
body food;
std::vector<body> snake;
int offset = 0;
int way_current = 0;
bool game = 1;

bool check( const int &x, const int &y )
{
	if ( y < 1 || y > (field.y - 4) || x < 2 || x > (field.x - 3 - (field.x%2)) )
		return 1;
	for ( int i = 1; i < (int)snake.size(); i++)
	{
		if ( y == snake[i].y && x == snake[i].x )
			return 1;
	}
	return 0;
}

void SnakeInit()
{
	body var;
	var.x = field.x/2;
	var.y = (field.y-2)/2;
	if ( !(var.x%2) )
		var.x++;
	for ( int i = 0; i < LENGTH; i++ )
	{
		snake.push_back( var );
	}
	snake.shrink_to_fit();
}

void DropFood()
{
	bool crossing;
	do
	{
		crossing = 0;
		food.x = ( rand() % ( field.x / 2 - 2 ) + 1 ) * 2 + 1;
		food.y = rand() % ( field.y - 4 ) + 1;
		for ( int i = 0; i < (int)snake.size(); i++ )
		{
			if ( snake[i].x == food.x && snake[i].y == food.y)
			{
				crossing = 1;
				break;
			}
		}
	} while ( crossing );
}

void Crawl()
{
	for ( int i = snake.size() - 1; i > 0; i-- )
	{
		snake[i].x=snake[i-1].x;
		snake[i].y=snake[i-1].y;
	}
}

void Eating()
{
	begin = std::chrono::steady_clock::now();
	snake.push_back( snake[0] );
	Crawl();
	switch ( way_current )
	{
		case KEY_UP:
			snake[0].y--;
			break;
		case KEY_DOWN:
			snake[0].y++;
			break;
		case KEY_LEFT:
			snake[0].x-=2;
			break;
		case KEY_RIGHT:
			snake[0].x+=2;
			break;
	}
	DropFood();
	end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::nanoseconds>( end - begin );
	buf[offset] = elapsed_ms.count();
	offset++;
}

void MvSnake( int key )
{
	switch ( key )
	{
		case KEY_UP:
			if ( way_current != KEY_DOWN )
			{
				way_current = key;
				if ( snake[0].y - 1 == food.y && snake[0].x == food.x )
					Eating();
				else if ( check( snake[0].x, snake[0].y - 1 ) )
					game=0;
				else
				{
					Crawl();
					snake[0].y--;
				}
			}
			else
				MvSnake( way_current );
			break;
		case KEY_DOWN:
			if ( way_current != KEY_UP )
			{
				way_current = key;
				if ( snake[0].y + 1 == food.y && snake[0].x == food.x )
					Eating();
				else if ( check( snake[0].x, snake[0].y + 1 ) )
					game=0;
				else
				{
					Crawl();
					snake[0].y++;
				}
			}
			else
				MvSnake( way_current );
			break;
		case KEY_LEFT:
			if ( way_current != KEY_RIGHT )
			{
				way_current = key;
				if ( snake[0].y == food.y && snake[0].x - 2 == food.x )
					Eating();
				else if ( check( snake[0].x - 2, snake[0].y ) )
					game=0;
				else
				{
					Crawl();
					snake[0].x-=2;
				}
			}
			else
				MvSnake( way_current );
			break;
		case KEY_RIGHT:
			if ( way_current != KEY_LEFT )
			{
				way_current = key;
				if ( snake[0].y == food.y && snake[0].x + 2 == food.x )
					Eating();
				else if ( check( snake[0].x + 2, snake[0].y ) )
					game=0;
				else
				{
					Crawl();
					snake[0].x+=2;
				}
			}
			else
				MvSnake( way_current );
			break;
		case 0:
			break;
		case 'q':
			game = 0;
			break;
		default:
			MvSnake( way_current );
			break;
	}
}

int Str_length()
{
	int value = 4 + (field.x%2);
	if ( snake[0].x/2 > 99 )
		value+=3;
	else if ( snake[0].x/2 > 9 )
		value+=2;
	else
		value++;
	if ( snake[0].y > 99 )
		value+=3;
	else if ( snake[0].y > 9 )
		value+=2;
	else
		value++;
	return value;
}

int main()
{
	srand( time( NULL ) );
	initscr();
	curs_set(0);
	noecho();
	nodelay( stdscr, 1 );
	keypad( stdscr, true );
	cbreak();
	timeout(500);
	getmaxyx( stdscr, field.y, field.x );
	if ( field.y < 6 || field.x < 40)
		game = 0;
	SnakeInit();
	DropFood();
	while ( game )
	{
		flushinp();
		clear();
		for (int i = 0 ; i < field.x ; i++ )
		{
			mvaddch( 0, i, '#' | A_STANDOUT );
			mvaddch( field.y - 1, i, '#' | A_STANDOUT );
			mvaddch( field.y - 3, i, '#' | A_STANDOUT );
		}
		attron( A_STANDOUT );
		for ( int i = 1 ; i < field.y - 1 ; i++ )
		{
			mvprintw( i, 0, "##" );
			mvprintw( i, field.x - ( field.x%2 ? 3 : 2 ) , "###" );
		}
		for ( int i = 1 ; i < (int)snake.size() ; i++ )
		{
			mvprintw( snake[i].y, snake[i].x - 1, "  " );
		}
		attron( A_REVERSE | A_DIM );
		mvprintw( snake[0].y, snake[0].x - 1, "  " );
		attroff( A_STANDOUT | A_REVERSE | A_DIM );
		attron( A_BLINK );
		mvprintw( food.y, food.x - 1, "@@" );
		attroff( A_BLINK );
		mvprintw( field.y - 2, 3, "SCORE:%d", snake.size() - LENGTH );
		mvprintw( field.y - 2, field.x - Str_length() - 1, "%dX%d", snake[0].x/2, snake[0].y );
		MvSnake( getch() );
		refresh();
	}
	getch();
	endwin();
	int min = buf[0], max = buf[0], avg = buf[0];
	for (int i = 1; i < offset; i++)
	{
		if ( buf[i] < min )
			min = buf [i];
		else if ( buf[i] > max )
			max = buf[i];
		avg += buf[i];
	}
	printf( "Min = %d\nMax = %d\nAvg = %d\n", min, max, ( offset!=0 ? avg/offset : avg ) );		
	for (int i = 0; i < offset; i++)
	{
		printf( "%d %d\n", i, buf[i] );
	}
	return 0;
}