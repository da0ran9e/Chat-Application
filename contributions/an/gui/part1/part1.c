#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <ctype.h>

GtkWidget 	*window;
GtkWidget	*fixed1;
GtkWidget	*button1;
GtkWidget 	*label1;
GtkBuilder 	*builder;

int main(int argc, char *argv[])
{
	/* code */
	gtk_init(&argc, &argv);

	builder = gtk_builder_new_from_file("part1.glade");

	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_builder_connect_signals(builder, NULL);

	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
	button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
	label1 = GTK_WIDGET(gtk_builder_get_object(builder, "label1"));

	gtk_widget_show(window);

	gtk_main();

	return EXIT_SUCCESS;
}

void button_clicked (GtkButton *b){
	gtk_label_set_text (GTK_LABEL(label1), (const gchar* ) "Hello!");
}