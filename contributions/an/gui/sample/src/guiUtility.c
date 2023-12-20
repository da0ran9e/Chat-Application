#include <gtk/gtk.h>

void showMessageDialog (gpointer data) {
    GtkWidget* dialog;

    dialog = GTK_WIDGET(data);
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    return 0;
}

void messageDialog (GtkWidget* relative, char *mesg, int type, int inOtherThread) {
    GtkWidget *dialog;
    GtkWidget *tmplbl;
    GtkWidget *tmpcontainer;
    GtkWindow* topContainer;
    GtkDialogFlags flags;
    
    flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

    if (relative != NULL) {
        topContainer = GTK_WINDOW(gtk_widget_get_toplevel(relative));
    } else {
        topContainer = NULL;
    }
    dialog = gtk_message_dialog_new(topContainer, flags, type,
                                         GTK_BUTTONS_CLOSE, NULL);
    gtk_widget_set_size_request(dialog, 450, 150);
    tmplbl = gtk_label_new(mesg);
    gtk_label_set_xalign(GTK_LABEL(tmplbl), 0);
    tmpcontainer = gtk_message_dialog_get_message_area(GTK_DIALOG(dialog));
    gtk_container_add(GTK_CONTAINER(tmpcontainer), tmplbl);
    if (inOtherThread) {
        g_idle_add(G_SOURCE_FUNC(showMessageDialog), (gpointer)dialog);
    } else {
        gtk_widget_show_all(dialog);
        gtk_dialog_run(dialog);
        gtk_widget_destroy(dialog);
    }
}

void errDialog (GtkWidget* relative, char *mesg) {
    messageDialog(relative, mesg, GTK_MESSAGE_ERROR, 1);
}