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
#include <pthread.h>


char* account = "test";
char* pass = "test";

int count =0;

char* choosenAcc ="";

GtkBuilder 	*builder; 
GtkBuilder 	*builder1; 
GtkWidget 	*loginWindow;
GtkWidget 	*listWindow;
GtkWidget 	*chatWindow;
GtkWidget 	*chatWindow1;


GtkWidget 	*confirmBtn;
GtkWidget 	*usnEntry;
GtkWidget 	*pwdEntry;




GtkWidget	*fixChat;
GtkWidget	*chatGrid; 
GtkWidget	*chatView;
GtkWidget	*lblChat[1000]; //test label chat 
GtkWidget 	*chatBtn;
GtkWidget 	*chatEntry;



GtkWidget	*fixList;
GtkScrolledWindow *onlWindow;
GtkVBox		*onlVbox;
// GtkTreeView	*onLst; 
GtkWidget	*listView;
GtkWidget	*button[1000];
GtkWidget	*lbl[1000]; //test label chat 






char		tmp[1024];
int			row;



char 		tmp1[1024];
int			rowChat;
char		*rowChatText[1000];
int			labelChatContent[13];



void		on_destroy(); 
void		on_row(GtkButton *);
void 		on_chatBtn_clicked();
void 		on_chatEntry_changed();
void 		gen_thread(GtkBuilder *builder, int threads);

int main(int argc, char *argv[])
{
	int count = 0;

    gtk_init(&argc, &argv);



    builder = gtk_builder_new_from_file("glade/GUI.glade");
    builder1 = gtk_builder_new_from_file("glade/GUI.glade");


    loginWindow = GTK_WIDGET(gtk_builder_get_object(builder, "loginWindow"));
	listWindow = GTK_WIDGET(gtk_builder_get_object(builder, "listWindow"));
	chatWindow = GTK_WIDGET(gtk_builder_get_object(builder, "chatWindow"));
	chatWindow1 = GTK_WIDGET(gtk_builder_get_object(builder1, "chatWindow"));
	gtk_window_set_default_size(GTK_WINDOW (chatWindow), 400, 400);
	gtk_window_set_title(GTK_WINDOW (chatWindow), "DEF");
	gtk_window_set_default_size(GTK_WINDOW (chatWindow1), 400, 400);
	gtk_window_set_title(GTK_WINDOW (chatWindow1), "ABC");
	


	

	g_signal_connect(loginWindow,"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(listWindow,"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(chatWindow,"destroy",G_CALLBACK(gtk_main_quit),NULL);
	g_signal_connect(chatWindow1,"destroy",G_CALLBACK(gtk_main_quit),NULL);



	gtk_builder_connect_signals(builder, NULL);
	gtk_builder_connect_signals(builder1, NULL);


	//Objects in loginWindow
	confirmBtn = GTK_WIDGET(gtk_builder_get_object(builder,"confirmBtn"));
	usnEntry = GTK_WIDGET(gtk_builder_get_object(builder,"usnEntry"));
	pwdEntry = GTK_WIDGET(gtk_builder_get_object(builder,"pwdEntry"));
	gtk_entry_set_visibility(GTK_ENTRY(pwdEntry), FALSE);

	//Objects in listWindow
	fixList = GTK_WIDGET(gtk_builder_get_object(builder, "fixList"));
	// listView = GTK_WIDGET(gtk_builder_get_object(builder, "listView"));
	// listGrid = GTK_WIDGET(gtk_builder_get_object(builder, "listGrid"));
	onlWindow = GTK_WIDGET(gtk_builder_get_object(builder, "scrolledWindow2"));



	//Object in chatWindow
	fixChat = GTK_WIDGET(gtk_builder_get_object(builder, "fixChat"));
	chatView = GTK_WIDGET(gtk_builder_get_object(builder, "chatView"));
	chatGrid = GTK_WIDGET(gtk_builder_get_object(builder, "chatGrid"));
	chatBtn = GTK_WIDGET(gtk_builder_get_object(builder,"chatBtn"));
	chatEntry = GTK_WIDGET(gtk_builder_get_object(builder,"chatEntry"));






	//Get data for listWindow
	FILE *f1 = fopen("src/onlist.txt", "r");

	if (f1 == NULL ) {
		printf("File not found\n");
		return EXIT_FAILURE;
		}

	row = 0;
	rowChat = 0;

	//handle window chat
	// for(rowChat = 0; rowChat< 1000; rowChat++){
	// 	gtk_grid_insert_row (GTK_GRID(chatGrid), rowChat);
	// 	lblChat[rowChat] = gtk_label_new (tmp1);
	// 	gtk_label_set_justify (GTK_LABEL(lblChat[rowChat]), GTK_JUSTIFY_LEFT);
	// 	gtk_label_set_xalign (GTK_LABEL(lblChat[rowChat]), 0.0);
	// 	gtk_grid_attach (GTK_GRID(chatGrid), lblChat[rowChat], 1, rowChat, 1, 1);
	// }

	onlVbox = gtk_vbox_new(FALSE, 1);
	gtk_container_add(GTK_CONTAINER(onlWindow), GTK_WIDGET(onlVbox));

	while (1) {
		if (fgets(tmp, 1024, f1) == NULL) {
			fclose(f1);
			break;
			}
		tmp[strlen(tmp)-1] = 0; // remove newline byte

		lbl[row] = gtk_label_new(tmp);
		gtk_label_set_xalign (GTK_LABEL(lbl[row]), 0.0);
		button[row] = gtk_button_new();
		gtk_container_add(GTK_CONTAINER(button[row]),lbl[row]);
		gtk_box_pack_start(GTK_BOX(onlVbox), button[row], FALSE, FALSE, 0);
		g_signal_connect(button[row], "clicked", G_CALLBACK(on_row), NULL);
		++row;
	}



    g_object_unref(builder);
    g_object_unref(builder1);

    gtk_widget_show_all(loginWindow);                
	
    gtk_main();

    return 0;
}

//called when change usnEntry
void on_usnEntry_changed(GtkEntry *e){
	char e1[128];
	sprintf(e1,"%s",gtk_entry_get_text(e));
}

//called when change pwdEntry
void on_pwdEntry_changed(GtkEntry *e){
	char e2[128];
	sprintf(e2,"%s",gtk_entry_get_text(e));
}

void* destroyButton (void* none) {
	sleep(3);
	gtk_widget_destroy(button[2]);
}

void errDialog (char *mesg) {
	GtkWidget	*loginDialog;
	GtkWidget	*tmplbl;
	GtkWidget	*tmpcontainer;
	
	GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
	loginDialog = gtk_message_dialog_new(GTK_WINDOW(gtk_widget_get_toplevel(loginWindow)),flags,GTK_MESSAGE_ERROR,GTK_BUTTONS_CLOSE,NULL);
	gtk_widget_set_size_request(loginDialog,450,150);
	tmplbl = gtk_label_new(mesg);
	gtk_label_set_xalign(GTK_LABEL(tmplbl),0);
	tmpcontainer = gtk_message_dialog_get_message_area(GTK_DIALOG(loginDialog));
	gtk_container_add(GTK_CONTAINER(tmpcontainer),tmplbl);
	gtk_widget_show_all(loginDialog);
	gtk_dialog_run(loginDialog);
	gtk_widget_destroy(loginDialog);
}

//called when click button
void on_confirmBtn_clicked(GtkButton *b){

	// gtk_editable_delete_text(GTK_EDITABLE(usnEntry),0,-1);
	char e1[128];
	sprintf(e1,"%s",gtk_entry_get_text(usnEntry));

	char e2[128];
	sprintf(e2,"%s",gtk_entry_get_text(pwdEntry));

	if(strcmp(e1,account)==0 && strcmp(e2,pass)==0) {
		printf("login\n");
		gtk_widget_hide(loginWindow);
		gtk_widget_show_all(listWindow);
		pthread_t tid;
		pthread_create(&tid, NULL, &destroyButton, NULL);
		
	}
	else {
		printf("wrong usn or pass!\n");

		errDialog("WRONG USERNAME OR PASSWORD");		
	}
	
}

void on_confirmBtn_enter_notify_event(){
	//TODO
}




//test multithread for multiwindow
void debugMesg(int *ptr) {
//------------------------------
    printf("A thread with ID: %i was created\n", ptr);
    //windowUpdate(ptr);  
    //prime(ptr);

}

void genThreads(GtkBuilder *builder, int threads) {
//------------------------------
    XInitThreads();
	//GtkBuilder		*stressBuilder;
    //stressBuilder = builder;    //points to the builder used in main()

//------------------------------
    GtkWidget       *winStress;     
//------------------------------

    winStress = GTK_WIDGET(gtk_builder_get_object(builder, "chatWindow"));
    gtk_widget_queue_draw(winStress);
    gtk_widget_show(winStress);

    int proc[threads];
    pthread_t prime[threads];
    char* id;

    for(int i = 0; i < threads; i++) {
        id = (char) i;
        proc[i] = pthread_create(&prime[i], NULL, debugMesg, (int *) id);
    }
}

void* cancelDialog (GtkDialog* dialog) {
	sleep(4);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void on_row(GtkButton *b) {
		pthread_t tid;
    	GtkWidget * lbltmp = gtk_bin_get_child(GTK_BIN(b));
    	choosenAcc = gtk_label_get_text(lbltmp);
    	printf("%s\n", choosenAcc);


		GtkWidget	*waitingDialog;
		GtkWidget	*answerDialog;
		GtkWidget	*waitingLbl;
		GtkWidget	*waitingContainer;
		GtkWidget	*answerLbl;
		GtkWidget	*answerContainer;


		GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
		answerDialog = gtk_message_dialog_new (listWindow,flags,GTK_MESSAGE_INFO,GTK_BUTTONS_OK_CANCEL,NULL);
		gtk_widget_set_size_request(answerDialog,450,100);

		char tmp[128] = "Request from ";
		strcat(tmp,choosenAcc);
		answerLbl = gtk_label_new(tmp);
		gtk_label_set_xalign(GTK_LABEL(answerLbl),0.2);
		answerContainer = gtk_message_dialog_get_message_area(GTK_DIALOG(answerDialog));
		gtk_container_add(GTK_CONTAINER(answerContainer),answerLbl);
		
		// waitingDialog = gtk_message_dialog_new(listWindow,flags,GTK_MESSAGE_INFO,GTK_BUTTONS_CANCEL,NULL);
		// gtk_widget_set_size_request(waitingDialog,450,100);


		// waitingLbl = gtk_label_new("WAITING FOR ANSWER!");
		// gtk_label_set_xalign(GTK_LABEL(waitingLbl),0.2);
		// waitingContainer = gtk_message_dialog_get_message_area(GTK_DIALOG(waitingDialog));
		// gtk_container_add(GTK_CONTAINER(waitingContainer),waitingLbl);


		
		// gtk_widget_show_all(waitingDialog);
		gtk_widget_show_all(answerDialog);
		
		int answerRes = gtk_dialog_run(answerDialog);
		// pthread_create(&tid, NULL, cancelDialog, waitingDialog);
		// int waitingRes = gtk_dialog_run(waitingDialog);

		// switch (waitingRes)
		// {
		// case GTK_RESPONSE_CANCEL:
		// 	printf("Cancel request,go back online list \n");
		// 	gtk_widget_destroy(waitingDialog);
		// 	break;
		
		// default:
		// 	break;
		// }

		switch (answerRes)
		{
		case GTK_RESPONSE_OK:
			printf("Request accepted, open chat\n");
			gtk_widget_destroy(answerDialog);			
			gtk_widget_show_all(chatWindow);
			// gtk_widget_show(chatWindow1);
			printf("checkpoint 331\n");
			gtk_widget_show_all(chatWindow1);
			break;
		
		case GTK_RESPONSE_CANCEL:
			printf("Request rejected, go back online list\n");
			gtk_widget_destroy(answerDialog);			
			break;
		default:
			break;
		}
		
		
	}


void on_chatBtn_clicked(GtkButton *b){

	char mess[128];
	char tmp[128] = " : ";
	sprintf(mess,"%s",gtk_entry_get_text(chatEntry));
	if(strcmp(mess,tmp1)!=0){
		strcat(tmp,mess);
		strcpy(mess,choosenAcc);
		strcat(mess,tmp);
		gtk_label_set_text(GTK_LABEL(GTK_LABEL(lblChat[count])),mess);
		count ++;
		gtk_entry_set_text(chatEntry,"");
	}
}

void on_chatEntry_changed(){
	//TODO
}

// called when window is closed
void	on_destroy() { 
		//gtk_main_quit();
		}