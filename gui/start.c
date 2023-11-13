#include <gtk/gtk.h>

static void activate (GtkApplication *app, gpointer user_data){
	// gtk code comde here
}

int main (int argv, char **argv){

	GtkApplication *app;

	int ret;

	app = gtk_application_new ("in.start", G_APPLICATION_FLAGS_NONE);

	g_signal_connet (app, "activate", G_CALLBACK(activate), NULL);

	ret = g_application_run (G_APPLICATION(app), argc, argv);

	g_object_unref(app);

	return ret;
}