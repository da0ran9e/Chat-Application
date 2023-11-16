#include <gtk/gtk.h>
#include <stdio.h>
#include <stdbool.h>

static void activate(GtkApplication*, gpointer);
static void button_clicked(GtkWidget*, gpointer);
static bool check_win(int, int, int);
static bool check_draw();
static void show_result_dialog(const char*);
void restart_game();
void quit_application();
static void setcolor(GtkWidget***, gpointer);

GtkWidget*** buttons;
int N;
int currentPlayer = 1;
GtkApplication *app;
int restart_count = 0;


int main(int argc, char** argv) {
    int status;

    app = gtk_application_new("com.example.tic_tac_toe", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    for (int i = 0; i < N; i++) {
        free(buttons[i]);
    }
    free(buttons);

    g_object_unref(app);

    return status;
}


static void button_clicked(GtkWidget *widget, gpointer data) {
    int row = GPOINTER_TO_INT(data) / N;
    int col = GPOINTER_TO_INT(data) % N;

    if (gtk_button_get_label(GTK_BUTTON(widget)) == NULL) {
        if(restart_count){
            restart_count = 0;
            currentPlayer = 1;
        }
        if (currentPlayer == 1) {
            gtk_button_set_label(GTK_BUTTON(widget), "X");
            if (check_win(1, row, col)) {
                show_result_dialog("Player 1 (X) wins!");
            } else if (check_draw()) {
                show_result_dialog("Draw!\n");
            }
            currentPlayer = 2;
        } else {
            gtk_button_set_label(GTK_BUTTON(widget), "O");
            if (check_win(2, row, col)) {
                show_result_dialog("Player 2 (O) wins!\n");
            } else if (check_draw()) {
                show_result_dialog("Draw!\n");
            }
            currentPlayer = 1;
        }
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget* window;
    GtkWidget* grid;

    printf("Enter the board size (N): ");
    scanf("%d", &N);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Tic-Tac-Toe");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    buttons = (GtkWidget ***)malloc(N * sizeof(GtkWidget **));
    for (int i = 0; i < N; i++) {
        buttons[i] = (GtkWidget **)malloc(N * sizeof(GtkWidget *));
        for (int j = 0; j < N; j++) {
            buttons[i][j] = gtk_button_new_with_label(NULL);
            gtk_widget_set_size_request(buttons[i][j], 100, 100);
            gtk_widget_set_name(buttons[i][j], "button");
            setcolor(buttons, NULL);
            g_signal_connect(buttons[i][j], "clicked", G_CALLBACK(button_clicked), GINT_TO_POINTER(i * N + j));
            gtk_grid_attach(GTK_GRID(grid), buttons[i][j], i, j, 1, 1);
        }
    }

    gtk_widget_show_all(window);
}


static bool check_win(int player, int row, int col) {
    gchar playerSymbol = (player == 1) ? 'X' : 'O';

    // Check horizontally
    int count = 0;
    for (int i = 0; i < N; i++) {
        GtkWidget *button = buttons[row][i];
        const gchar *label_text = gtk_button_get_label(GTK_BUTTON(button));
        if (label_text != NULL && label_text[0] == playerSymbol) {
            count++;
            if (count == 3) {
                return true;
            }
        } else {
            count = 0;
        }
    }

    // Check vertically
    count = 0;
    for (int i = 0; i < N; i++) {
        GtkWidget *button = buttons[i][col];
        const gchar *label_text = gtk_button_get_label(GTK_BUTTON(button));
        if (label_text != NULL && label_text[0] == playerSymbol) {
            count++;
            if (count == 3) {
                return true;
            }
        } else {
            count = 0;
        }
    }

    // Check diagonals
    // Diagonal from top-left to bottom-right
    count = 0;
    for (int i = -N + 1; i < N; i++) {
        int r = row + i;
        int c = col + i;
        if (r >= 0 && r < N && c >= 0 && c < N) {
            GtkWidget *button = buttons[r][c];
            const gchar *label_text = gtk_button_get_label(GTK_BUTTON(button));
            if (label_text != NULL && label_text[0] == playerSymbol) {
                count++;
                if (count == 3) {
                    return true;
                }
            } else {
                count = 0;
            }
        }
    }

    // Diagonal from top-right to bottom-left
    count = 0;
    for (int i = -N + 1; i < N; i++) {
        int r = row - i;
        int c = col + i;
        if (r >= 0 && r < N && c >= 0 && c < N) {
            GtkWidget *button = buttons[r][c];
            const gchar *label_text = gtk_button_get_label(GTK_BUTTON(button));
            if (label_text != NULL && label_text[0] == playerSymbol) {
                count++;
                if (count == 3) {
                    return true;
                }
            } else {
                count = 0;
            }
        }
    }
    return false;
}


static bool check_draw() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            const gchar *label_text = gtk_button_get_label(GTK_BUTTON(buttons[i][j]));
            if (label_text == NULL) {
                return false;
            }
        }
    }
    return true;
}

static void show_result_dialog(const char *message) {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_NONE, message);
    gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog), "Choose an option:");

    gtk_dialog_add_button(GTK_DIALOG(dialog), "Restart", 1);
    gtk_dialog_add_button(GTK_DIALOG(dialog), "Quit", 2);

    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    int result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    if (result == 1) {
        restart_game();
        currentPlayer = 1;
    } else if (result == 2) {
        quit_application();
    }
}

void restart_game() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            GtkWidget *button = GTK_WIDGET(buttons[i][j]);
            gtk_button_set_label(GTK_BUTTON(button), "");
            gtk_button_set_label(GTK_BUTTON(button), NULL);
            gtk_widget_set_name(buttons[i][j], "button");
        }
    }
    setcolor(buttons, NULL);
    restart_count = 1;
    currentPlayer = 1;
}



void quit_application() {
    g_application_quit(G_APPLICATION(app));
}

static void setcolor(GtkWidget*** widgets, gpointer user_data) {
    GtkCssProvider *css_provider = gtk_css_provider_new();
    GError *error = NULL;
    
    if (!gtk_css_provider_load_from_path(css_provider, "tictactoe_style.css", &error)) {
        g_error("Error loading CSS file: %s", error->message);
        g_clear_error(&error);
    }

    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                        GTK_STYLE_PROVIDER(css_provider),
                        GTK_STYLE_PROVIDER_PRIORITY_USER);
}


