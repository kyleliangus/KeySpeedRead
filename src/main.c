#include <pebble.h>
#include <time.h>

#define KEY_ABSTRACT 0
#define KEY_TITLE 1
//#define KEY_DATA 5

Window *window;
static TextLayer *window_layer;
MenuLayer *menu_layer;

Window *splash_window;
static TextLayer *splash_layer;

Window *word_window;
static TextLayer *title_layer;
static TextLayer *word_layer;


// window showing key speed read
static void word_window_load(Window *window)
{
  word_layer = text_layer_create(GRect(0, 50, 144, 50));
  text_layer_set_background_color(word_layer, GColorClear);
  text_layer_set_text_color(word_layer, GColorWhite);
  text_layer_set_text_alignment(word_layer, GTextAlignmentCenter);
  text_layer_set_text(word_layer, "Loading...");
  
  title_layer = text_layer_create(GRect(0, 10, 144, 30));
  text_layer_set_background_color(title_layer, GColorClear);
  text_layer_set_text_color(title_layer, GColorWhite);
  text_layer_set_text_alignment(title_layer, GTextAlignmentCenter);
  text_layer_set_text(title_layer, "Loading...");
  text_layer_set_font(title_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
}

static void word_window_unload(Window *window)
{
  text_layer_destroy(word_layer);
  text_layer_destroy(title_layer);
}
// window showing key speed read


// get input from Javascript
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Read first item
  Tuple *t = dict_read_first(iterator);

  static char abstract[32];
  static char title[32];
  
  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
    case KEY_ABSTRACT:
          snprintf(abstract, sizeof(abstract), "%s", t->value->cstring);
      break;
    case KEY_TITLE:
          snprintf(title, sizeof(title), "%s", t->value->cstring);
      break;
    default:
      APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }
    text_layer_set_text( word_layer, abstract );
    text_layer_set_text( title_layer, title );
    psleep( 100*10*( strlen(abstract) ) );
    // Look for next item
    t = dict_read_next(iterator);
  }
  window_stack_pop(true);  
}


static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}



// Menu Layout
void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{
    //Which row is it?
    switch(cell_index->row)
    {
    case 0:
        menu_cell_basic_draw(ctx, cell_layer, "", "1. Science and Tech", NULL);
        break;
    case 1:
        menu_cell_basic_draw(ctx, cell_layer, "", "2. Sports", NULL);
        break;
    case 2:
        menu_cell_basic_draw(ctx, cell_layer, "", "3. Entertainment", NULL);
        break;
    case 3:
        menu_cell_basic_draw(ctx, cell_layer, "", "4. World", NULL);
        break;
    }
}

uint16_t num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
    return 4;
}

void select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
    //Get which row
    int which = cell_index->row;
 
    //The array that will hold the on/off vibration times
    uint32_t segments[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
    //Build the pattern (milliseconds on and off in alternating positions)
    for(int i = 0; i < which + 1; i++)
    {
        segments[2 * i] = 200;
        segments[(2 * i) + 1] = 100;
    }
 
    //Create a VibePattern data structure
    VibePattern pattern = {
        .durations = segments,
        .num_segments = 16
    };
 
    //Do the vibration pattern!
    vibes_enqueue_custom_pattern(pattern);
  
    psleep( 1000 );
  
    // create window to show
    word_window = window_create(); 
    window_set_window_handlers(word_window, (WindowHandlers) {
       .load = word_window_load,
       .unload = word_window_unload,
    });
    window_stack_push(word_window, true);  
    // create window to show
  
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
  
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
// Menu Layout


// This is the timer
void timer_callback(void *data) {
 window_stack_pop(true);
}


// splash window
static void splash_window_load(Window *window)
{
  app_timer_register(1000, (AppTimerCallback) timer_callback, NULL);
  splash_layer = text_layer_create(GRect(0, 55, 144, 50));
  text_layer_set_background_color(splash_layer, GColorClear);
  text_layer_set_text_color(splash_layer, GColorBlack);
  text_layer_set_text(splash_layer, "News");
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(splash_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(splash_layer, GTextAlignmentCenter);
  
  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(splash_layer));
}

static void splash_window_unload(Window *window)
{
  text_layer_destroy(splash_layer);
}
// splash window


void window_load(Window *window)
{
  // Create time TextLayer
  window_layer = text_layer_create(GRect(0, 15, 144, 50));
  text_layer_set_background_color(window_layer, GColorClear);
  text_layer_set_text_color(window_layer, GColorBlack);
  text_layer_set_text(window_layer, "Article Topics");
  
  // Improve the layout to be more like a watchface
  text_layer_set_font(window_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  text_layer_set_text_alignment(window_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(window_layer));
  
  
  // Menu Layer
  //Create it - 12 is approx height of the top bar
  menu_layer = menu_layer_create(GRect(0, 55, 144, 113 - 16));
 
  //Let it receive clicks
  menu_layer_set_click_config_onto_window(menu_layer, window);
 
  //Give it its callbacks
  MenuLayerCallbacks callbacks = {
      .draw_row = (MenuLayerDrawRowCallback) draw_row_callback,
      .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) num_rows_callback,
      .select_click = (MenuLayerSelectCallback) select_click_callback
  };
  menu_layer_set_callbacks(menu_layer, NULL, callbacks);
  //Add to Window
  layer_add_child(window_get_root_layer(window), menu_layer_get_layer(menu_layer));
  // Menu Layer
  
}
 
void window_unload(Window *window)
{
  // Destroy TextLayer
  text_layer_destroy(window_layer);
}
 
// initialize main window
void init()
{
  window = window_create();
  splash_window = window_create();
  // create some text
  WindowHandlers handlers = {
    .load = window_load,
    .unload = window_unload
  };
  window_set_window_handlers(window, handlers);
  
  window_set_window_handlers(splash_window, (WindowHandlers) {
     .load = splash_window_load,
     .unload = splash_window_unload,
  });
  window_stack_push(window, true);
  window_stack_push(splash_window, true);
}
 
// delete the window pointer
void deinit(Window *window)
{
  window_destroy(window);
}

/*
*
* SPLASH WINDOW
*
*/
/*


void splash_init()
{
  splash_window = window_create();
  // create some text
  WindowHandlers splash_handlers = {
    .load = splash_window_load,
    .unload = splash_window_unload
  };
  window_set_window_handlers(splash_window, splash_handlers);
 
  window_stack_push(window, true);
}
*/



int main(void)
{
  // splash init
  //splash_init(splash_window);
  //psleep(3000);
  init();
  app_event_loop();
  //deinit(splash_window);
  deinit(window);
}