#include <stdio.h>
#include <wiringPi.h> 
#include <gtk/gtk.h> 
#include <math.h> 
#include <cairo.h> 

#define ECHO 28
#define TRIG 29


float distanceHCSR;
int width ;
int height ;
int ChangeDirection = -1;
int ChangeDirection2 = -1;
int tempData;

GtkBuilder      *builder; 
GtkWidget       *window;
GtkLabel	*label;
GtkWidget       *area;

void setup(void);
gboolean time_handler(GtkLabel *label);

float Read(void);

PI_THREAD (hcsr) 
{
	while(1){
		distanceHCSR = Read();
		delay(2000);
		//printf("%.1fcm\n",distanceHCSR);
		GtkAllocation* alloc = g_new(GtkAllocation, 1);
		gtk_widget_get_allocation(area, alloc);
		g_free(alloc);
		width = alloc->width;
		height = alloc-> height;
	}
}

gboolean on_area_draw(GtkDrawingArea *widget,cairo_t *cr){
	
	if(distanceHCSR>tempData){
		ChangeDirection = -1;
		ChangeDirection2 = -1;
		cairo_set_source_rgb (cr, 0, 1, 0);
	}
	else{
		ChangeDirection = 1;
		ChangeDirection2 = 1;
		cairo_set_source_rgb (cr, 1, 0, 0);
	}
	
	
	cairo_set_line_width (cr, 40.96);
	cairo_move_to (cr,width/5, height /2);
	cairo_rel_line_to (cr, 50, 50*ChangeDirection);
	cairo_rel_line_to (cr, 50, -50*ChangeDirection2);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_MITER); /* default */
	cairo_stroke (cr);
	tempData = distanceHCSR;
	return FALSE;
}


int main(int argc, char *argv[])
{	
	setup();
    
	
    gtk_init(&argc, &argv);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder, "window.glade", NULL); 
    window = GTK_WIDGET(gtk_builder_get_object(builder, "win")); 
    label = GTK_LABEL(gtk_builder_get_object(builder, "read_label")); 
    area = GTK_WIDGET(gtk_builder_get_object(builder, "area")); 
   
  
    
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);
    gtk_widget_show(window);
    g_timeout_add(2000, (GSourceFunc)time_handler,label);
         
    gtk_main();
    return 0;
}

void setup(void){ 
	wiringPiSetup(); 
	piThreadCreate(hcsr); 
	pinMode(ECHO,INPUT); 
	pinMode(TRIG,OUTPUT);
	digitalWrite(TRIG,LOW);
	delay(100);
}

gboolean time_handler(GtkLabel *label){
	
	char convert[50];
	sprintf(convert,"Uzaklık : %.1f cm",distanceHCSR);
	gtk_label_set_text(label,convert); 
	gtk_widget_queue_draw(area); 
	
    return TRUE;
}

float Read(void){
	digitalWrite(TRIG,HIGH);
	delayMicroseconds(10);
	digitalWrite(TRIG,LOW);
	long startTime;
	long stopTime;
	while(digitalRead(ECHO)==0){startTime = micros();}
	while(digitalRead(ECHO)==1){stopTime = micros();}
	long travelTime = stopTime - startTime;
	float distance = (travelTime * 34300) / 2;
	return distance/1000000;
}
