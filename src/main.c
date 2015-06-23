#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;
static GFont s_time_font;
#ifdef PBL_COLOR
	static Layer *s_canvas_layer;
	static int s_random = 4;
	static int temp_random;
	static GDrawCommandImage *s_command_image;
#elif PBL_BW
	static BitmapLayer *s_background_layer;
	static GBitmap *s_background_bitmap;
#endif

#ifdef PBL_COLOR
	static void update_background(){

		if(s_random == 4){
			s_random = 0;
		} else {

			temp_random = rand() % 3;

			while(temp_random == s_random){
			    temp_random = rand() % 3;
		    }

		    s_random = temp_random;

		    if(s_random == 0){
			    window_set_background_color(s_main_window, GColorTiffanyBlue);
		    } else if(s_random == 1){
			    window_set_background_color(s_main_window, GColorFolly);
		    } else if(s_random == 2){
			    window_set_background_color(s_main_window, GColorChromeYellow);
		    }
		}
	}
#endif

static void update_time(){

	// get a tm structire
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	// create a long-lived buffer
	static char buffer[] = "00:00";

	// write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
	    // Use 24 hour format
	    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
	} else {
	    // Use 12 hour format
		strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
	}

	// display this time on the textlayer
	text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
	update_time();

	#ifdef PBL_COLOR
		update_background();
	#endif
}

#ifdef PBL_COLOR
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
#endif

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	#ifdef PBL_COLOR
	  	// Load the image and check it was succcessful
	  	s_command_image = gdraw_command_image_create_with_resource(RESOURCE_ID_DOGE_PDC);

		if (!s_command_image){
	    	APP_LOG(APP_LOG_LEVEL_ERROR, "Image is NULL!");
	    }

	    // Create canvas Layer and set up the update procedure
	    s_canvas_layer = layer_create(bounds);
	    layer_set_update_proc(s_canvas_layer, update_proc);
	    layer_add_child(window_layer, s_canvas_layer);
	#elif PBL_BW
		// Create GBitmap, then set to created BitmapLayer
		s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_DOGE_DITHERED);
		s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
		bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
		layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
	#endif

	// Create time TextLayer
 	s_time_layer = text_layer_create(GRect(0, 0, 144, 50));
  	text_layer_set_background_color(s_time_layer, GColorClear);
	#ifdef PBL_COLOR
  		text_layer_set_text_color(s_time_layer, GColorWhite);
	#elif PBL_BW
		text_layer_set_text_color(s_time_layer, GColorBlack);
	#endif
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
}

static void window_unload(Window *window) {
	#ifdef PBL_COLOR
    	// Destroy canvas Layer
    	layer_destroy(s_canvas_layer);

    	// Destroy the image
    	gdraw_command_image_destroy(s_command_image);
	#elif PBL_BW
		// Destroy GBitmap
		gbitmap_destroy(s_background_bitmap);

		// Destroy BitmapLayer
		bitmap_layer_destroy(s_background_layer);
	#endif

	// destroy text layer
	text_layer_destroy(s_time_layer);

	// unload gfont
	fonts_unload_custom_font(s_time_font);
}

static void init() {
    // Set up main Window
    s_main_window = window_create();
	#ifdef PBL_COLOR
    	window_set_background_color(s_main_window, GColorFolly);
	#endif
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    window_stack_push(s_main_window, true);

	// make sure the time is displayed from the start
	update_time();

	// register with ticktimerservice
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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
