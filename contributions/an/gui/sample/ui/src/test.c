
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <time.h>
#include <ctype.h>
#include <sys/mman.h>

// Make them global

GtkWidget	*window;
GtkWidget	*fixed1;
GtkWidget	*grid1; 
GtkWidget	*view1;
GtkWidget	*scroll1;
GtkWidget	*button[8];
GtkBuilder	*builder; 

void		on_destroy(); 
void		on_row(GtkButton *);

char		tmp[1024],tmp1[1024]; // general use
int		row;
char		*rowText[1000];
int		buttonContent[8]; // which row is in the button?

int main(int argc, char *argv[]) {

	gtk_init(&argc, &argv); // init Gtk

//---------------------------------------------------------------------
// establish contact with xml code used to adjust widget settings
//---------------------------------------------------------------------
 
	builder = gtk_builder_new_from_file ("glade/test.glade");
 
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	g_signal_connect(window, "destroy", G_CALLBACK(on_destroy), NULL);

        gtk_builder_connect_signals(builder, NULL);

	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	view1 = GTK_WIDGET(gtk_builder_get_object(builder, "view1"));
	grid1 = GTK_WIDGET(gtk_builder_get_object(builder, "grid1"));
	scroll1 = GTK_WIDGET(gtk_builder_get_object(builder, "scroll1"));

	FILE *f1 = fopen("functions.txt", "r");

	if (f1 == NULL ) {
		printf("File finctions.txt not found\n");
		return EXIT_FAILURE;
		}

	row = 0;
	while (1) {
		if (fgets(tmp1, 1024, f1) == NULL) {
			fclose(f1);
			break;
			}
		tmp1[strlen(tmp1)-1] = 0; // remove newline byte
		sprintf(tmp, "%d: %s", row, tmp1);
		rowText[row] = (char *) malloc (strlen(tmp) + 1);
		strcpy(rowText[row], tmp);

//		The following code will populate the grid with clickable buttons.

		if (row < 8 ) { // populate first 8 rows
			gtk_grid_insert_row (GTK_GRID(grid1), row);
			button[row] = gtk_button_new_with_label (rowText[row]);
			gtk_button_set_alignment (GTK_BUTTON(button[row]), 0.0, 0.5); // hor left, ver center
			gtk_grid_attach (GTK_GRID(grid1), button[row], 1, row, 1, 1);
			g_signal_connect(button[row], "clicked", G_CALLBACK(on_row), NULL);
			buttonContent[row] = row; // current button content
			}
		row ++;
		if (row >= 1000 ) {
			printf("Too many rows\n");
			row --;
			break;
			}
		}

//	row points to one higher than the last element

	printf("There are %d rows\n",row);

	gtk_range_set_range (GTK_RANGE(scroll1), 0, row ); // establish range

//-----------------------------------

	gtk_widget_show_all(window);

	gtk_main();

	return EXIT_SUCCESS;
	}

//......................................................................................


void	on_row(GtkButton *b) {
	printf("You selected: %s\n", gtk_button_get_label (b));
	}

void	on_destroy() { 
		gtk_main_quit();
		}

void	on_scroll1_value_changed(GtkRange *r) {
	int i,j;

	i = (int) gtk_range_get_value (r); // returns a gdouble - scroll location

	if (i >= row) i = row - 8; // last page

	for (j = i; j < i + 8; j++) {
		if (j < row) 
			gtk_button_set_label (GTK_BUTTON( button[j - i] ), (const gchar* ) rowText[j]);
		else
			gtk_button_set_label (GTK_BUTTON( button[j - i] ), (const gchar* ) " ");
		buttonContent[j-i] = j;
		}
	}
