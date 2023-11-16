#ifndef APP_H
#define APP_H

#include <gtk/gtk.h>
#include <stdbool.h>

extern GtkWidget*** buttons;
extern int N, M;
extern int currentPlayer;
extern GtkApplication* app;
extern int restartCount;

void activate(GtkApplication* app, gpointer user_data);
void buttonClicked(GtkWidget* widget, gpointer data);
bool checkWin(int player, int row, int col);
bool checkDraw();
void showResultDialog(const char* message);
void showInfoDialog(const char* infoMessage);
void restartGame();
void quitApplication();
void setColor(GtkWidget*** widgets, gpointer user_data);
void cleanup();

#endif // APP_H
