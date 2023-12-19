#ifndef GUI_UTILITY_H
#define GUI_UTILITY_H

void errDialog (GtkWidget* relative, char* mesg);
void messageDialog (GtkWidget* relative, char* mesg, int type, int inOtherThread);

#endif // !GUI_UTILITY_H
