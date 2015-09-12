#include <pebble.h>

#define HOUR_OUTER_SEGMENT_RATIO 0.69;

static Window *s_window;
static struct tm *s_time;
static int s_hour_inner_radius;

// max width or height we can make our shapes fit in
static int s_base_size;

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  s_time = localtime(&temp);

  s_time->tm_hour = 3;

  layer_mark_dirty(window_get_root_layer(s_window));
}

static int32_t get_segment_angle(int segment) {
  return segment * 15 * TRIG_MAX_ANGLE / 360;
}

static void draw_background(Layer *layer, GContext *ctx) {
  // @todo move this to config
  GColor color = GColorFromHEX(0xAAFFFF);

  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void draw_hour_outer_segment(GContext *ctx, int start, int end) {
  graphics_fill_radial(ctx, GPoint(s_base_size, s_base_size), s_hour_inner_radius, s_base_size,
                       get_segment_angle(start), get_segment_angle(end));
}

static void draw_hour_inner_segment(GContext *ctx, int start, int end) {
  graphics_fill_radial(ctx, GPoint(s_base_size, s_base_size), 0, s_base_size, get_segment_angle(start),
                       get_segment_angle(end));
}

static void draw_hour(Layer *layer, GContext *ctx) {
  // @todo move this to config
  GColor color = GColorFromHEX(0xAAFFFF);

  int hour = s_time->tm_hour;

  graphics_context_set_fill_color(ctx, GColorFromHEX(0x0055AA));

  switch (hour) {
    case 1 :
    case 13 :
      draw_hour_outer_segment(ctx, 1, 11);
      break;

    case 2 :
    case 14 :
      draw_hour_outer_segment(ctx, -3, 4);
      draw_hour_outer_segment(ctx, 9, 16);
      draw_hour_inner_segment(ctx, 2, 4);
      draw_hour_inner_segment(ctx, 14, 16);
      break;

    case 3 :
    case 15 :
      draw_hour_outer_segment(ctx, -3, 15);
      draw_hour_inner_segment(ctx, 5, 7);
      break;

    case 4 :
    case 16 :
      draw_hour_outer_segment(ctx, -3, 15);
      draw_hour_inner_segment(ctx, 5, 7);
      break;
  }

}

static void update_layer(Layer *layer, GContext *ctx) {


  draw_background(layer, ctx);
  draw_hour(layer, ctx);

  // graphics_fill_circle(ctx, GPoint(center_y, center_x), 89);
//  APP_LOG(APP_LOG_LEVEL_INFO, "size.h = %d", thing);


}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_base_size = (window_bounds.size.w < window_bounds.size.h ? window_bounds.size.w : window_bounds.size.h) / 2;

  s_hour_inner_radius = s_base_size * HOUR_OUTER_SEGMENT_RATIO;
  APP_LOG(APP_LOG_LEVEL_INFO, "s_hour_inner_radius: %d", s_hour_inner_radius);

  layer_set_update_proc(window_get_root_layer(window), update_layer);
}

static void main_window_unload(Window *window) {

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void init() {
  // Create main Window element and assign to pointer
  s_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_window, true);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Make sure the time is displayed from the start
  update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
