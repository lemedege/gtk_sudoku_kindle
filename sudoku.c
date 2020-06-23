/*
	Copyright ©2016 sguerrini97
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define DEBUG
#ifdef BUILD_KINDLE
#define PUZZLE_PATH "/mnt/us/extensions/sudoku/data/puzzles"
#define SAVE_PATH "/mnt/us/extensions/sudoku/data/saved_sudoku.txt"
#define BG_PATH "/mnt/us/extensions/sudoku/data/marks/"
#define GRID_X 60
#define GRID_Y 60
#define CMD_X 47
#define CMD_Y 43
#define TOOL_X 88
#define TOOL_Y 50
#else
#ifdef BUILD_PC
#define PUZZLE_PATH "data/puzzles"
#define SAVE_PATH "data/saved_sudoku.txt"
#define BG_PATH "data/marks/"
#define GRID_X 50
#define GRID_Y 50
#define CMD_X 43
#define CMD_Y 43
#define TOOL_X 94
#define TOOL_Y 50
#else
#error "WTF are you building for"
#endif
#endif

#define APPLICATION_TITLE "- Kindle Sudoku -"
#define QUIT_BUTTON_TEXT "Quit"
#define LOAD_BUTTON_TEXT "Load"
#define SAVE_BUTTON_TEXT "Save"
#define NEW_BUTTON_TEXT "New"
#define MARK_BUTTON_TEXT "Mark"

#define PANGO_SCALE_SMALL    ((double)0.8333333333333)

#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>

//const int* mask[9] =   {0b100000000,0b010000000,0b001000000,0b000100000,0b000010000,0b000001000,0b000000100,0b000000001,0b000000000}; 


GtkWidget *window;
GtkWidget *lTitle;	// title lable
static GtkWidget *sudokuw[9][9];



GtkWidget *BG;

int sudoku_total_files = 0;
char *sudoku_files[256];
char *BG_images[10];
int markmode = 0;
int *markdata[9][9][10];


int char2int (char *array, size_t n)
{    
    int number = 0;
    int mult = 1;

    n = (int)n < 0 ? -n : n;       /* quick absolute value check  */

    /* for each character in array */
    while (n--)
    {
        /* if not digit or '-', check if number > 0, break or continue */
        if ((array[n] < '0' || array[n] > '9') && array[n] != '-') {
            if (number)
                break;
            else
                continue;
        }

        if (array[n] == '-') {      /* if '-' if number, negate, break */
            if (number) {
                number = -number;
                break;
            }
        }
        else {                      /* convert digit to numeric value   */
            number += (array[n] - '0') * mult;
            mult *= 10;
        }
    }

    return number;
}

// grid cell to act on, current sudoku file
int selected_x = 0, selected_y = 0, current_sudoku = -1;


// change grid cell
static void selection( GtkWidget *widget, gpointer data )
{
	GdkColor color;
	gdk_color_parse( "#999999", &color );
	
	gtk_widget_modify_bg( sudokuw[selected_x][selected_y], GTK_STATE_PRELIGHT, NULL );

	selected_x = (int)((int*)data)[0];
	selected_y = (int)((int*)data)[1];
	
	gtk_widget_modify_bg( sudokuw[selected_x][selected_y], GTK_STATE_PRELIGHT, &color );
	#ifdef DEBUG
	g_print("New selection: %i, %i\n", selected_x, selected_y);
	#endif
}

// put number in grid cell
static void put_number( GtkWidget *widget, gpointer data )
{
	int selectedimg[10] = {0};
	int count = 0;
	char *string = (char*)data;
	if( string[0] == 'X' )
		string[0] = 0;

	if(markmode == 0)
	{
	gtk_button_set_label( GTK_BUTTON( sudokuw[selected_x][selected_y] ), string );
	gtk_button_set_image (GTK_BUTTON (sudokuw[selected_x][selected_y]), NULL);
	}

	if(markmode == 1)
	{
 		gtk_button_set_label( GTK_BUTTON( sudokuw[selected_x][selected_y] ), NULL );

		// X resets the markdata and remove the numbers
		if(atoi(string) == 0)
		{
		gtk_button_set_image (GTK_BUTTON (sudokuw[selected_x][selected_y]), NULL);
		for( int i=0;i<10;i++)		
		{
		markdata[selected_x][selected_y][i] = 0;
		}
		}

		else if(markdata[selected_x][selected_y][atoi(string)] == 0){
		markdata[selected_x][selected_y][atoi(string)] = 1;
		}
		else{
		markdata[selected_x][selected_y][atoi(string)] = 0;
		}
		

		//compare the array to se which marks are selected

		for(int i=0;i<10;i++)
		{
		if(markdata[selected_x][selected_y][10-i] == 1)
		{
		selectedimg[9-count] = 10-i;
		count++;
		}
		}

		if(selectedimg[9] ==0) return;
		if(count >= 4) return;

		else{

		char name[14];

    		for(int i=0;i<9;i++) name[i] = '0'+selectedimg[i+1];

    		name[9]='.';
    		name[10]='p';
    		name[11]='n';
   		name[12]='g';
    		name[13]='\0';

    		//printf("%s\n",name);

		

		char *bgpath = BG_PATH;

		char* name_with_extension;
		name_with_extension = malloc(strlen(bgpath)+14); /* make space for the new string (should check the return value ...) */
		strcpy(name_with_extension, bgpath); /* copy name into the new var */
		strcat(name_with_extension, name); /* add the extension */

    		printf("%s\n",name_with_extension);

		BG = gtk_image_new_from_file (name_with_extension);
		gtk_button_set_image (GTK_BUTTON (sudokuw[selected_x][selected_y]), BG);
		}


		//g_print("%d\n",s);

		#ifdef DEBUG
		//g_print("%s\n", BG_images[atoi(string)]);
		//g_print("%",	sudokuw[selected_x][selected_y]);
		//for( int i=1;i<10;i++)		
		//{
		//g_print("%i", selectedimg[i]);
		//}
		//g_print("\n");
		//g_print("%d\n",atoi(selectedimg));
	
		
		#endif
	}
	
}

// save current puzzle progress
static void save( GtkWidget *widget, gpointer data )
{
	FILE *savefile = NULL;
	int i,j;

	if( current_sudoku == -1 )	// no puzzle to save
		return;
	
	savefile = fopen( SAVE_PATH, "w" );
	if( !savefile )
		return;
	
	fprintf( savefile, "%s\n0", sudoku_files[current_sudoku] );
	
	for( i=0; i<9; i++ )
	{
		for( j=0; j<9; j++ )
		{
			if( gtk_widget_get_sensitive(sudokuw[i][j]) == TRUE )	// if the cell is editable
			{
				if( strcmp( "", gtk_button_get_label(GTK_BUTTON(sudokuw[i][j])) ) != 0 )	// if user wrote something
				{
					fprintf( savefile, "%s", gtk_button_get_label( GTK_BUTTON(sudokuw[i][j])) );
					continue;
				}
			}
			fprintf( savefile, "." );
		}
	}
	fclose( savefile );
}

// start a new random sudoku
static void new( GtkWidget *widget, gpointer data )
{
	FILE *sudoku_file = NULL;
	char path[256];
	int i,j;
	char n[2] = { 0, '\0' };
	char c;
	char *name;
	
	GdkColor color;
	gdk_color_parse( "#eeeeee", &color );
	
	int file = -1;
	
	if( data == NULL )
	{
		do
		{
			file = rand()%sudoku_total_files;
		}while( file == current_sudoku ); // different from the current one
	}
	else
	{
		file = (int)data;
	}

	current_sudoku = file;
	#ifdef DEBUG
	g_print("New sudoku: %i: %s\n", file, sudoku_files[file]);
	#endif

	name = sudoku_files[file];
	
	gtk_label_set_text (GTK_LABEL(lTitle),name+39);
	
	// fill the grid
	sudoku_file = fopen( sudoku_files[file], "r" );
	for( i = 0; i < 9; i++ )
	{
		for( j = 0; j < 9; j++ )
		{
			c = fgetc( sudoku_file );
			if( c != '.' )
			{
				n[0] = c;
				gtk_widget_set_sensitive(sudokuw[i][j], FALSE);
				gtk_button_set_image (GTK_BUTTON (sudokuw[i][j]), NULL);
			}
			else
			{
				n[0] = '\0';
				gtk_widget_set_sensitive(sudokuw[i][j], TRUE);
				gtk_widget_modify_bg( sudokuw[i][j], GTK_STATE_NORMAL, &color );
				gtk_button_set_image (GTK_BUTTON (sudokuw[i][j]), NULL);
			}

			gtk_button_set_label( GTK_BUTTON(sudokuw[i][j]), n );
			//gtk_button_set_always_show_image (GTK_BUTTON (sudokuw[i][j]), TRUE);
			
		}
	}
	fclose(sudoku_file);
}

// load saved puzzle progress
static void load( GtkWidget *widget, gpointer data )
{
	int i,j, file = -1;
	char buffer[256];
	char c = 0, label[2] = { 0, '\0' };
	FILE *savefile = fopen( SAVE_PATH, "r" );
	
	if( !savefile )
		return;
		
	fscanf( savefile, "%s", buffer );
	
	for( i=0; i<sudoku_total_files; i++ )
		if( strcmp( sudoku_files[i], buffer ) == 0 )
		{
			file = i;
			break;
		}
		
	new( NULL, (gpointer)file );
	
	while( c != '0' )
		c = fgetc( savefile );
	
	for( i=0; i<9; i++ )
		for( j=0; j<9; j++ )
		{
			c = fgetc(savefile);
			if( c != '.' )
			{
				label[0] = c;
				#ifdef DEBUG
				g_print( "c = %c - pos = %i, %i\n", c, i, j );
				#endif
				gtk_button_set_label( GTK_BUTTON(sudokuw[i][j]), label );
			}
		}
}

// quit
static void quit( GtkWidget *widget, gpointer data )
{
	#ifdef DEBUG
	g_print("Quitting.. Bye\n");
	#endif
	gtk_main_quit();
}

static void mark( GtkWidget *widget, gpointer data )
{
	if( markmode == 0)
	{	
		markmode = 1;
	}
	else markmode = 0;	
	#ifdef DEBUG
	g_print("Markmode: %d\n",markmode);
	#endif
}

static gboolean delete_event( GtkWidget *widget, GdkEvent *event, gpointer data )
{
	return FALSE; // quit
}

static void destroy( GtkWidget *widget, gpointer data )
{
	gtk_main_quit();
}


int main( int argc, char *argv[] )
{

	GtkWidget *bQuit, *bLoad, *bSave, *bNew, *bMark; 						// toolbox buttons
	GtkWidget *vbox, *hbox, *separator, *cmdbox, *toolbox;			// boxes
	GtkWidget *numbers[10];										// numbers buttons

	GtkSettings *default_settings = gtk_settings_get_default();
	g_object_set(default_settings, "gtk-button-images", TRUE, NULL); 

	int i,j;
	int *data = NULL;
	char num[2] = { 0, '\0' };
	char *n = NULL;
	
	DIR *dir;
	struct dirent *ep;
	

	//create mark images array
	
	for(i=0; i<10; i++ )
	{
	BG_images[i] = (char*)malloc(sizeof(char)*256);
	sprintf( BG_images[i], "%s/%d.png", BG_PATH,i);
	#ifdef DEBUG
	g_print("%s\n", BG_images[i]);
	#endif
	}

	// count sudoku files and save paths to them
	dir = opendir(PUZZLE_PATH);
	while( ep = readdir(dir) )
	{
		if( strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0 )
			continue;
		sudoku_files[sudoku_total_files] = (char*)malloc(sizeof(char)*256);
		sprintf( sudoku_files[sudoku_total_files], "%s/%s", PUZZLE_PATH, ep->d_name );
		#ifdef DEBUG
		g_print("%s\n", sudoku_files[sudoku_total_files]);
		#endif
		sudoku_total_files++;
	}
	closedir(dir);

	// random seed
	srand(time(NULL));

	gtk_init( &argc, &argv );

	window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
	g_signal_connect( window, "delete-event", G_CALLBACK(delete_event), NULL );
	g_signal_connect( window, "destroy", G_CALLBACK(destroy), NULL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 10);

	lTitle = gtk_label_new( APPLICATION_TITLE );
	
	bQuit = gtk_button_new_with_label( QUIT_BUTTON_TEXT );
	gtk_widget_set_size_request( bQuit, TOOL_X, TOOL_Y );
	g_signal_connect (bQuit, "clicked", G_CALLBACK(quit), NULL);

	bLoad = gtk_button_new_with_label( LOAD_BUTTON_TEXT );
	gtk_widget_set_size_request( bLoad, TOOL_X, TOOL_Y );
	g_signal_connect (bLoad, "clicked", G_CALLBACK(load), NULL);

	bSave = gtk_button_new_with_label( SAVE_BUTTON_TEXT );
	gtk_widget_set_size_request( bSave, TOOL_X, TOOL_Y );
	g_signal_connect (bSave, "clicked", G_CALLBACK(save), NULL);

	bNew = gtk_button_new_with_label( NEW_BUTTON_TEXT );
	gtk_widget_set_size_request( bNew, TOOL_X, TOOL_Y );
	g_signal_connect (bNew, "clicked", G_CALLBACK(new), NULL);

	bMark = gtk_toggle_button_new_with_label( MARK_BUTTON_TEXT );
	gtk_widget_set_size_request( bMark, TOOL_X, TOOL_Y );
	g_signal_connect (bMark, "clicked", G_CALLBACK(mark), NULL);

	vbox = gtk_vbox_new( FALSE, 0 );
	gtk_box_pack_start(GTK_BOX(vbox), lTitle, 0,0,5);
	
	// prepare the grid (with buttons)
	for( i=0; i<9; i++ )
	{
		hbox = gtk_hbox_new( FALSE, 0 );
		for( j=0; j<9; j++ )
		{
			sudokuw[i][j] = gtk_button_new();
			gtk_widget_set_sensitive(sudokuw[i][j], FALSE);
			gtk_widget_set_size_request( sudokuw[i][j], GRID_X, GRID_Y );
			data = (int*) malloc(sizeof(int)*2);
			data[0] = i; data[1] = j;
			g_signal_connect (sudokuw[i][j], "clicked", G_CALLBACK(selection), data);
			gtk_box_pack_start(GTK_BOX(hbox), sudokuw[i][j], 0,0,0);
			
			//markdata[i][j] = (int*) malloc(sizeof(int)*9);
			for(int k=0;k<10;k++)
			{
			markdata[i][j][k] = 0;
			}		

			if( (j+1)%3 == 0 && j<8 )
			{
				separator = gtk_vseparator_new();
				gtk_box_pack_start(GTK_BOX(hbox), separator, 0, 0, 5);
			}
		}
		gtk_box_pack_start(GTK_BOX(vbox), hbox, 0, 0, 0);
		if( (i+1)%3 == 0 && i<8 )
		{
			separator = gtk_hseparator_new();
			gtk_box_pack_start(GTK_BOX(vbox), separator, 0, 0, 5);
		}
	}
	
	// prepare the number buttons
	cmdbox = gtk_hbox_new( FALSE, 0 );
	for( i = 0; i < 10; i++ )
	{
		if( i == 9 )
			num[0] = 'X';
		else
			num[0] = 0x31+i;
		numbers[i] = gtk_button_new_with_label( num );
		gtk_widget_set_size_request( numbers[i], CMD_X, CMD_Y );
		gtk_box_pack_start( GTK_BOX(cmdbox), numbers[i], 0, 0, 5 );
		n = (char*)malloc(sizeof(char)*2);
		n[1] = '\0';
		n[0] = num[0];
		g_signal_connect (numbers[i], "clicked", G_CALLBACK(put_number), n);
	}
	
	
	gtk_box_pack_start(GTK_BOX(vbox), cmdbox, 0, 0, 10 );
	gtk_container_add(GTK_CONTAINER(window), vbox);
	
	// prepare the toolbox
	toolbox = gtk_hbox_new( FALSE, 20 );
	gtk_box_pack_start(GTK_BOX(toolbox), bNew, 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(toolbox), bSave, 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(toolbox), bLoad, 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(toolbox), bQuit, 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(toolbox), bMark, 0, 0, 5);
	
	gtk_box_pack_start(GTK_BOX(vbox), toolbox, 0, 0, 10 );

	gtk_window_set_title( GTK_WINDOW(window), "L:A_N:application_ID:sudoku");
	gtk_widget_show_all(window);

	gtk_main();
	return 0;
}
