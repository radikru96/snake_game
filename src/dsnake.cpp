#include <ncurses.h>
#include <ctime>
#include <random>
#define LENGTH 5

struct body
{
	int x;
	int y;
};
body field;
body food;
body *snake;
int length = LENGTH;
int way_current = 0;
bool game = 1;

bool check( const int &x, const int &y )
{
	if ( y < 1 || y > (field.y - 4) || x < 2 || x > (field.x - 3 - (field.x%2)) )
		return 1;
	for ( int i = 1; i < length; i++)
	{
		if ( y == snake[i].y && x == snake[i].x )
			return 1;
	}
	return 0;
}

void SnakeInit()
{
	snake[0].x = field.x/2;
	snake[0].y = (field.y-2)/2;
	if ( !(snake[0].x%2) )
		snake[0].x++;
	for ( int i = 1; i < length; i++ )
	{
		snake[i].x = snake[i-1].x;
		snake[i].y = snake[i-1].y;
	}
}

void DropFood()
{
	bool crossing;
	do
	{
		crossing = 0;
		food.x = ( rand() % ( field.x / 2 - 2 ) + 1 ) * 2 + 1;
		food.y = rand() % ( field.y - 4 ) + 1;
		for ( int i = 0; i < length; i++ )
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
	for ( int i = length - 1; i > 0; i-- )
	{
		snake[i].x=snake[i-1].x;
		snake[i].y=snake[i-1].y;
	}
}

void Eating()
{
	body *nSnake = new body[length+1];
	for ( int i = length; i > 0; i-- )
	{
		nSnake[i].x = snake[i-1].x;
		nSnake[i].y = snake[i-1].y;
	}
	switch ( way_current )
	{
		case KEY_UP:
			nSnake[0].x = snake[0].x;
			nSnake[0].y = snake[0].y - 1;
			break;
		case KEY_DOWN:
			nSnake[0].x = snake[0].x;
			nSnake[0].y = snake[0].y + 1;
			break;
		case KEY_LEFT:
			nSnake[0].x = snake[0].x - 2;
			nSnake[0].y = snake[0].y;
			break;
		case KEY_RIGHT:
			nSnake[0].x = snake[0].x + 2;
			nSnake[0].y = snake[0].y;
			break;
	}
	delete [] snake;
	length++;
	snake = nSnake;
	DropFood();
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
	snake = new body[length];
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
		for ( int i = 1 ; i < length ; i++ )
		{
			mvprintw( snake[i].y, snake[i].x - 1, "  " );
		}
		attron( A_REVERSE | A_DIM );
		mvprintw( snake[0].y, snake[0].x - 1, "  " );
		attroff( A_STANDOUT | A_REVERSE | A_DIM );
		attron( A_BLINK );
		mvprintw( food.y, food.x - 1, "@@" );
		attroff( A_BLINK );
		mvprintw( field.y - 2, 3, "SCORE:%d", length - LENGTH );
		mvprintw( field.y - 2, field.x - Str_length() - 1, "%dX%d", snake[0].x/2, snake[0].y );
		MvSnake( getch() );
		refresh();
	}
	delete [] snake;
	getch();
	endwin();
	return 0;
}