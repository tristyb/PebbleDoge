#include <pebble.h>

static Window *s_main_window;
static Layer *s_canvas_layer;
static TextLayer *s_time_layer;
static TextLayer *s_wow_layer;
static GFont s_time_font;
static GFont s_wow_font;
static int s_random;

static GDrawCommandImage *s_command_image;

static void wow_text(){
	s_random = rand() % 3;
	
	if(s_random == 0){
		text_layer_set_text(s_wow_layer,"such time");
		text_layer_set_text_color(s_wow_layer, GColorFashionMagenta);
	} else if (s_random == 1){
		text_layer_set_text(s_wow_layer,"very pebble");
		text_layer_set_text_color(s_wow_layer, GColorCyan);
	} else if (s_random == 2){
		text_layer_set_text(s_wow_layer,"much watch");
		text_layer_set_text_color(s_wow_layer, GColorGreen);
	}
}

static void update_time(){
	
	// get a tm structire
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);
	
	// create a long-lived buffer
	static char buffer[] = "00:00";
	
	// write the current hours and minutes into the buffer
	// use 24 hour format
	strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	
	// display this time on the textlayer
	text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	update_time();
	wow_text();
}

static void update_proc(Layer *layer, GContext *ctx) {
  // Place image in the center of the Window
  GSize img_size = gdraw_command_image_get_bounds_size(s_command_image);
  GPoint origin = GPoint(72 - (img_size.w / 3), 84 - (img_size.h / 3));

  // If the image was loaded successfully...
  if (s_command_image) {
    // Draw it
    gdraw_command_image_draw(ctx, s_command_image, origin);
  }
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

  	// Load the image and check it was succcessful
  	s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_DOGE_PDC);
  	
	if (!s_command_image){
    	APP_LOG(APP_LOG_LEVEL_ERROR, "Image is NULL!");
    }

    // Create canvas Layer and set up the update procedure
    s_canvas_layer = layer_create(bounds);
    layer_set_update_proc(s_canvas_layer, update_proc);
    layer_add_child(window_layer, s_canvas_layer);
	
	// Create time TextLayer
 	s_time_layer = text_layer_create(GRect(0, 0, 144, 50));
  	text_layer_set_background_color(s_time_layer, GColorClear);
  	text_layer_set_text_color(s_time_layer, GColorBlack  );
  	text_layer_set_text(s_time_layer, "00:00");

  	// create gfont
  	s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_NEUE_BOLD_38));
	
  	// Improve the layout to be more like a watchface
  	text_layer_set_font(s_time_layer, s_time_font);
  	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	
	// Improve the layout to be more like a watchface
  	text_layer_set_font(s_time_layer, s_time_font);

  	// Add it as a child layer to the Window's root layer
  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	
	// Create wow layer
	s_wow_layer = text_layer_create(GRect(10,136,124,50));
	text_layer_set_background_color(s_wow_layer, GColorClear);
  	text_layer_set_text_color(s_wow_layer, GColorBlack  );
	
	// create gfont
  	s_wow_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_COMIC_NEUE_BOLD_20));
	
	// Improve the layout to be more like a watchface
  	text_layer_set_font(s_wow_layer, s_wow_font);
	
	// Add it as a child layer to the Window's root layer
  	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_wow_layer));
}

static void window_unload(Window *window) {
    // Destroy canvas Layer
    layer_destroy(s_canvas_layer);

    // Destroy the image
    gdraw_command_image_destroy(s_command_image);
	
	// destroy text layer
	text_layer_destroy(s_time_layer);
	text_layer_destroy(s_wow_layer);
	
	// unload gfont
	fonts_unload_custom_font(s_time_font);
	fonts_unload_custom_font(s_wow_font);
}

static void init() {
    // Set up main Window
    s_main_window = window_create();
    window_set_background_color(s_main_window, GColorMayGreen);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
	
    window_stack_push(s_main_window, true);
	
	// make sure the time is displayed from the start
	update_time();
	wow_text();
	
	// register with ticktimerservice
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
	//tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
}

static void deinit() {
    // Destroy main Window
    window_destroy(s_main_window);
}

int main() {
    init();
    app_event_loop();
    deinit();
}