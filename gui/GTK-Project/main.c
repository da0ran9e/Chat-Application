#include <gtk/gtk.h>
#include "app.h"

int main(int argc, char** argv) {
    int status;

    app = gtk_application_new("com.example.tic_tac_toe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    cleanup();

    g_object_unref(app);

    return status;
}
