#include <pebble.h>
#include <ctype.h>


static Window *s_main_window;

static GFont s_custom_font_time;
static GFont s_custom_font_date;

static GBitmap *s_box_bitmap;
static BitmapLayer *s_box_bitmap_layer; 
	
static TextLayer *s_time_layer;
static TextLayer *s_weekday_layer;
static TextLayer *s_month_layer;
static TextLayer *s_date_layer;


char* strupr(char* s )
{
	char* p = s;
	while ((*p = toupper((unsigned char)*p))) p++;
	return s;
}


char* ltrim(char *string, char junk)
{
    char* original = string;
    char *p = original;
    int trimmed = 0;
    
	do
    {
        if (*original != junk || trimmed)
        {
            trimmed = 1;
            *p++ = *original;
        }
    }
    while (*original++ != '\0');
	
    return string;
}


static void minute_tick(struct tm* tick_time, TimeUnits units_changed) {
	//time
	static char s_time_text[] = "00:00";
	if (clock_is_24h_style() == true) {
		//24 hour
		strftime(s_time_text, sizeof(s_time_text), "%H:%M", tick_time);
	} else {
		//12 hour
		strftime(s_time_text, sizeof(s_time_text), "%I:%M", tick_time);
	}
	text_layer_set_text(s_time_layer, ltrim(s_time_text, '0'));
	
	//weekday
	static char s_weekday_text[] = "WEDNESDAY";
	strftime(s_weekday_text, sizeof(s_weekday_text), "%A", tick_time);
	text_layer_set_text(s_weekday_layer, strupr(s_weekday_text));
	
	//month
	static char s_month_text[] = "SEPTEMBER";	
	strftime(s_month_text, sizeof(s_month_text),"%B", tick_time);
	text_layer_set_text(s_month_layer, strupr(s_month_text));
	
	//day & year
	static char s_date_text[] = "10TH / 2000";
	strftime(s_date_text, sizeof(s_date_text), "%dTH / %Y", tick_time);
	
	if (s_date_text[1] == '1' && s_date_text[0] != '1') {
		s_date_text[2] = 'S';
		s_date_text[3] = 'T';
	} else if (s_date_text[1] == '2' && s_date_text[0] != '1') {
		s_date_text[2] = 'N';
		s_date_text[3] = 'D';
	} else if (s_date_text[1] == '3' && s_date_text[0] != '1') {
		s_date_text[2] = 'R';
		s_date_text[3] = 'D';
	} else {
		s_date_text[2] = 'T';
		s_date_text[3] = 'H';		
	}
	
	text_layer_set_text(s_date_layer, ltrim(s_date_text, '0'));
}


static void main_window_load(Window *window) {
	//main window & bounds
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
		
	//load background "box" img
	s_box_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BOX_WHITE);
	s_box_bitmap_layer = bitmap_layer_create(GRect(11, 57, 124, 122));
	bitmap_layer_set_bitmap(s_box_bitmap_layer, s_box_bitmap);
	layer_add_child(window_layer, bitmap_layer_get_layer(s_box_bitmap_layer)); 
 
 	//custom fonts
	s_custom_font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ARCHISTICO_BOLD_46));
	s_custom_font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FLUXARCHITECT_REGULAR_13));
	
	//time
	s_time_layer = text_layer_create(GRect(2, 11, bounds.size.w-2, 90));
	text_layer_set_text_color(s_time_layer, GColorWhite);
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_font(s_time_layer, s_custom_font_time);
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	//3 date lines
	s_weekday_layer = text_layer_create(GRect(2, 91, (bounds.size.w - 2), 110));
	text_layer_set_text_color(s_weekday_layer, GColorWhite);
	text_layer_set_background_color(s_weekday_layer, GColorClear);
	text_layer_set_font(s_weekday_layer, s_custom_font_date);
	text_layer_set_text_alignment(s_weekday_layer, GTextAlignmentCenter);
	
	s_month_layer = text_layer_create(GRect(2, 109, (bounds.size.w - 2), 130));
	text_layer_set_text_color(s_month_layer, GColorWhite);
	text_layer_set_background_color(s_month_layer, GColorClear);	
	text_layer_set_font(s_month_layer, s_custom_font_date);
	text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
	
	s_date_layer = text_layer_create(GRect(2, 127, (bounds.size.w - 2), 150));
	text_layer_set_text_color(s_date_layer, GColorWhite);
	text_layer_set_background_color(s_date_layer, GColorClear);
	text_layer_set_font(s_date_layer, s_custom_font_date);
	text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);

	//setup timer & initial run to ensure immediate display
	time_t now = time(NULL);
	struct tm *current_time = localtime(&now);
	minute_tick(current_time, MINUTE_UNIT);
	tick_timer_service_subscribe(MINUTE_UNIT, minute_tick);

	//actually add the text layers
	layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_weekday_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_month_layer));
	layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
}


static void main_window_unload(Window *window) {
	tick_timer_service_unsubscribe();

	bitmap_layer_destroy(s_box_bitmap_layer); 
	gbitmap_destroy(s_box_bitmap);
	
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_weekday_layer);
	text_layer_destroy(s_month_layer);
	text_layer_destroy(s_date_layer);

	fonts_unload_custom_font(s_custom_font_time); 
	fonts_unload_custom_font(s_custom_font_date);
}


static void init(void) {
	s_main_window = window_create();

	window_set_background_color(s_main_window, GColorBlack);
	
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload,
	});
	
	window_stack_push(s_main_window, true);
}


static void deinit(void) {
	window_destroy(s_main_window);
}


int main(void) {
	init();
	app_event_loop();
	deinit();
}
