#include <pebble.h>

const int SEGMENT_SIZE = 15;
const int OUTER_SEGMENT_RATIO = 6890; // divided by 1000 later
const int INNER_SEGMENT_RATIO = 4445; // divided by 1000 later
const int INNER_BLOCK_RATIO = 2223;

typedef void (*AnimationAllPhasesCompleteCallback)(void);

struct Colors {
  GColor background;
  GColor hour;
  GColor min10;
  GColor min0;
};

static Window *s_window;
static struct tm *s_time;

static int s_hour_inner_radius;
static int s_minute_inner_radius;
static int s_minute_block_size;
static GPoint s_center;

static Animation *s_animation;
static AnimationImplementation s_animation_implementation;
static int s_animation_progress;
static bool s_animation_running = false;
static int s_animation_phase = 0;
static int s_animation_target_phase = 0;
static AnimationAllPhasesCompleteCallback s_animation_callback;
static int s_animation_phase_timing[2] = {
  400,
  400
};









static int s_hour_animation_scale[12] = {
  450,
  700,
  450,
  425,
  400,
  450,
  300,
  750,
  300,
  300,
  150,
  150
};

static struct Colors s_colors;

// max width or height we can make our shapes fit in
static int s_base_size;
static int s_demo_time = 0;

// DECLARATIONS
static void next_animation(bool reverse);

static int32_t get_segment_angle(int segment) {
  return segment * SEGMENT_SIZE * TRIG_MAX_ANGLE / 360;
}

static int animation_distance(int64_t start, int64_t end) {
  return start
    + (s_animation_progress - ANIMATION_NORMALIZED_MIN)
    * (end - start)
    / (ANIMATION_NORMALIZED_MAX - ANIMATION_NORMALIZED_MIN);
}

static void draw_background(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, s_colors.background);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
}

static void draw_outer_expand(GContext *ctx) {
  graphics_context_set_fill_color(ctx, s_colors.hour);
  graphics_fill_circle(ctx, s_center, animation_distance(0, s_base_size));
}

static void draw_outer_reveal(GContext *ctx) {
  graphics_context_set_fill_color(ctx, s_colors.hour);
  graphics_fill_radial(
    ctx, s_center,
    animation_distance(0, s_hour_inner_radius),
    s_base_size + 1,
    get_segment_angle(0),
    get_segment_angle(24)
  );
}

static void draw_hour_outer_segment(GContext *ctx, int start, int end, int anim_start, bool swap_direction) {
  graphics_context_set_fill_color(ctx, s_colors.hour);

  if (swap_direction) {
    graphics_fill_radial(
      ctx,
      s_center,
      s_hour_inner_radius,
      s_base_size + 1,
      animation_distance(get_segment_angle(anim_start), get_segment_angle(start)),
      get_segment_angle(end)
    );
  } else {
    graphics_fill_radial(
      ctx,
      s_center,
      s_hour_inner_radius,
      s_base_size + 1,
      get_segment_angle(start),
      animation_distance(get_segment_angle(anim_start), get_segment_angle(end))
    );
  }
}

static void draw_hour_inner_segment(GContext *ctx, int start, int end) {
  graphics_context_set_fill_color(ctx, s_colors.hour);
  graphics_fill_radial(
    ctx,
    s_center,
    0,
    s_base_size + 1,
    get_segment_angle(start),
    get_segment_angle(end)
  );
}

static void _draw_min_outer_segment(GContext *ctx, GColor color, int start, int end) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_radial(
    ctx,
    s_center,
    s_minute_inner_radius,
    s_hour_inner_radius,
    get_segment_angle(start),
    get_segment_angle(end)
  );
}

static void draw_min0_outer_segment(GContext *ctx, int start, int end) {
  _draw_min_outer_segment(ctx, s_colors.min0, start, end);
}

static void draw_min10_outer_segment(GContext *ctx, int start, int end) {
  _draw_min_outer_segment(ctx, s_colors.min10, start, end);
}

static void _draw_min_inner_block(GContext *ctx, GColor color, GRect rect) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, rect, 0, GCornerNone);
}

static void draw_min0_inner_block_from_center(GContext *ctx, int width, int height) {
  _draw_min_inner_block(ctx, s_colors.min0, GRect(s_center.x, s_center.y, width, height));
}

static void draw_min10_inner_block_from_center(GContext *ctx, int width, int height) {
  _draw_min_inner_block(ctx, s_colors.min10, GRect(s_center.x, s_center.y, width, height));
}

static void draw_min0_inner_block_custom(GContext *ctx,  GRect rect) {
  _draw_min_inner_block(ctx, s_colors.min0, rect);
}

static void draw_min10_inner_block_custom(GContext *ctx,  GRect rect) {
  _draw_min_inner_block(ctx, s_colors.min10, rect);
}

static void draw_outer(GContext *ctx) {

  switch (s_time->tm_hour) {
    case 1 :
      draw_hour_outer_segment(ctx, 1, 11, 25, false);
      break;

    case 2 :
      draw_hour_outer_segment(ctx, -3, 4, -9, true);
      draw_hour_outer_segment(ctx, 9, 16, 3, true);
      break;

    case 3 :
      draw_hour_outer_segment(ctx, -3, 15, 21, false);
      break;

    case 4 :
      draw_hour_outer_segment(ctx, 5, 7, 1, true);
      draw_hour_outer_segment(ctx, 11, 13, 7, true);
      draw_hour_outer_segment(ctx, 17, 25, 13, true);
      break;

    case 5 :
      draw_hour_outer_segment(ctx, -4, 3, 10, false);
      draw_hour_outer_segment(ctx, 9, 15, 20, false);
      break;

    case 6 :
      draw_hour_outer_segment(ctx, 7, 27, 31, false);
      break;

    case 7 :
      draw_hour_outer_segment(ctx, -3, 3, -9, true);
      draw_hour_outer_segment(ctx, 13, 15, 3, true);
      break;

    case 8 :
      draw_hour_outer_segment(ctx, -4, 4, -8, true);
      draw_hour_outer_segment(ctx, 8, 16, 4, true);
      break;

    case 9 :
      draw_hour_outer_segment(ctx, -5, 15, 19, false);
      break;

    case 10 :
      draw_hour_outer_segment(ctx, -6, -1, 1, false);
      draw_hour_outer_segment(ctx, 13, 18, 11, true);
      draw_hour_outer_segment(ctx, 1, 11, 11, false);
      break;

    case 11 :
      draw_hour_outer_segment(ctx, 1, 11, 13, false);
      draw_hour_outer_segment(ctx, 13, 23, 25, false);
      break;

    case 0 :
      draw_hour_outer_segment(ctx, -2, 3, -3, true);
      draw_hour_outer_segment(ctx, 16, 21, 15, true);
      draw_hour_outer_segment(ctx, 9, 15, 3, true);
      break;
  }
}

static void draw_inner(GContext *ctx) {
  switch (s_time->tm_min % 10) {
    case 0 :
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, s_minute_inner_radius);
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, -s_minute_inner_radius);
      draw_min0_outer_segment(ctx, 0, 12);
      break;

    case 1 :
      draw_min0_outer_segment(ctx, 1, 11);
      break;

    case 2 :
      draw_min0_outer_segment(ctx, 0, 6);
      draw_min0_outer_segment(ctx, 8, 12);
      draw_min0_inner_block_from_center(ctx, s_minute_inner_radius, -s_minute_block_size);
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, s_minute_inner_radius);
      break;

    case 3 :
      draw_min0_outer_segment(ctx, 1, 11);
      draw_min0_inner_block_custom(ctx, GRect(
        s_center.x + s_minute_inner_radius,
        s_center.y - s_minute_block_size / 2,
        -s_minute_block_size,
        s_minute_block_size
      ));
      break;

    case 4 :
      draw_min0_outer_segment(ctx, 3, 10);
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, -s_minute_inner_radius);
      draw_min0_inner_block_custom(ctx, GRect(
        s_center.x,
        s_center.y - s_minute_block_size / 5,
        s_minute_inner_radius,
        s_minute_block_size
      ));
      break;

    case 5 :
      draw_min0_outer_segment(ctx, 0, 4);
      draw_min0_outer_segment(ctx, 6, 12);
      draw_min0_inner_block_from_center(ctx, s_minute_inner_radius, s_minute_block_size);
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, -s_minute_inner_radius);
      break;

    case 6 :
      draw_min0_outer_segment(ctx, 6, 12);
      draw_min0_inner_block_from_center(ctx, s_minute_inner_radius, s_minute_block_size);
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, -s_minute_inner_radius);
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, s_minute_inner_radius);
      break;

    case 7 :
      draw_min0_inner_block_custom(ctx, GRect(
        s_center.x + s_minute_inner_radius,
        s_center.y,
        -s_minute_block_size,
        -s_minute_block_size
      ));
      draw_min0_outer_segment(ctx, 0, 10);
      break;

    case 8 :
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, s_minute_inner_radius);
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, -s_minute_inner_radius);
      draw_min0_inner_block_custom(ctx, GRect(
        s_center.x,
        s_center.y - s_minute_block_size / 2,
        s_minute_inner_radius,
        s_minute_block_size
      ));
      draw_min0_outer_segment(ctx, 0, 12);
      break;

    case 9 :
      draw_min0_inner_block_from_center(ctx, s_minute_block_size, -s_minute_inner_radius);
      draw_min0_inner_block_custom(ctx, GRect(
        s_center.x,
        s_center.y - s_minute_block_size / 2,
        s_minute_inner_radius,
        s_minute_block_size
      ));
      draw_min0_outer_segment(ctx, 0, 12);
      break;

  }

  switch (s_time->tm_min / 10) {
    case 0 :
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, s_minute_inner_radius);
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, -s_minute_inner_radius);
      draw_min10_outer_segment(ctx, -12, 0);
      break;

    case 1 :
      draw_min10_outer_segment(ctx, -11, -1);
      break;

    case 2 :
      draw_min10_inner_block_from_center(ctx, -s_minute_inner_radius, s_minute_block_size);
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, -s_minute_inner_radius);
      draw_min10_outer_segment(ctx, -4, 0);
      draw_min10_outer_segment(ctx, -12, -6);
      break;

    case 3 :
      draw_min10_inner_block_custom(ctx, GRect(
        s_center.x - s_minute_block_size,
        s_center.y - s_minute_block_size / 2,
        -s_minute_block_size,
        s_minute_block_size
      ));
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, s_minute_inner_radius);
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, -s_minute_inner_radius);
      draw_min10_outer_segment(ctx, -4, 0);
      draw_min10_outer_segment(ctx, 12, 16);
      break;

    case 4 :
      draw_min10_inner_block_custom(ctx, GRect(
        s_center.x,
        s_center.y - s_minute_block_size / 5,
        -s_hour_inner_radius + s_minute_block_size / 5,
        s_minute_block_size
      ));
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, s_minute_inner_radius);
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, -s_minute_inner_radius);
      draw_min10_outer_segment(ctx, -7, -3);
      break;

    case 5 :
      draw_min10_inner_block_from_center(ctx, -s_minute_inner_radius, -s_minute_block_size);
      draw_min10_inner_block_from_center(ctx, -s_minute_block_size, s_minute_inner_radius);
      draw_min10_outer_segment(ctx, -6, 0);
      draw_min10_outer_segment(ctx, -12, -8);
      break;

  }

  switch (s_time->tm_hour) {
    case 1 :
      break;

    case 2 :
      draw_hour_inner_segment(ctx, 2, 4);
      draw_hour_inner_segment(ctx, 14, 16);
      break;

    case 3 :
      draw_hour_inner_segment(ctx, 5, 7);
      break;

    case 4 :
      draw_hour_inner_segment(ctx, -1, 1);
      draw_hour_inner_segment(ctx, 5, 7);
      draw_hour_inner_segment(ctx, 11, 13);
      draw_hour_inner_segment(ctx, 17, 19);
      break;

    case 5 :
      draw_hour_inner_segment(ctx, -4, -2);
      draw_hour_inner_segment(ctx, 8, 10);
      break;

    case 6 :
      draw_hour_inner_segment(ctx, 7, 9);
      draw_hour_inner_segment(ctx, 15, 17);
      break;

    case 7 :
      draw_hour_inner_segment(ctx, 1, 3);
      draw_hour_inner_segment(ctx, 13, 15);
      break;

    case 8 :
      draw_hour_inner_segment(ctx, -4, -2);
      draw_hour_inner_segment(ctx, 2, 4);
      draw_hour_inner_segment(ctx, 14, 16);
      draw_hour_inner_segment(ctx, 8, 10);
      break;

    case 9 :
      draw_hour_inner_segment(ctx, -5, -3);
      draw_hour_inner_segment(ctx, 3, 5);
      break;

    case 10 :
      draw_hour_inner_segment(ctx, 1, 3);
      draw_hour_inner_segment(ctx, 9, 11);
      break;

    case 0 :
      draw_hour_inner_segment(ctx, 1, 3);
      draw_hour_inner_segment(ctx, 13, 15);
      break;
  }
}


static void update_layer(Layer *layer, GContext *ctx) {
  draw_background(layer, ctx);

  switch (s_animation_phase) {
    case 0 :
      draw_outer_expand(ctx);
      break;

    case 1 :
      draw_inner(ctx);
      draw_outer_reveal(ctx);
      break;

    case 2 :
      draw_inner(ctx);
      draw_outer(ctx);
      break;
  }
}

static void update_animation(Animation *animation, AnimationProgress progress) {
  s_animation_progress = progress;
  layer_mark_dirty(window_get_root_layer(s_window));
}

static void on_animation_stopped(Animation *animation, bool stopped, void *ctx) {
  if(s_animation_phase < s_animation_target_phase) {
    s_animation_progress = ANIMATION_NORMALIZED_MIN;
    ++s_animation_phase;
    next_animation(false);
  } else if(s_animation_phase > s_animation_target_phase) {
    s_animation_progress = ANIMATION_NORMALIZED_MAX;
    --s_animation_phase;
    next_animation(true);
  } else {
    s_animation_running = false;
    if(s_animation_callback) {
      s_animation_callback();
    }
  }
}

static int get_animation_timing() {
  if(s_animation_phase == 2) {
    return s_hour_animation_scale[s_time->tm_hour];
  } else {
    return s_animation_phase_timing[s_animation_phase];
  }
}

static void next_animation(bool reverse) {
  s_animation = animation_create();
  s_animation_implementation.update = update_animation;
  animation_set_duration(s_animation, get_animation_timing());
  animation_set_reverse(s_animation, reverse);
  animation_set_curve(s_animation, AnimationCurveEaseInOut);
  animation_set_implementation(s_animation, &s_animation_implementation);
  animation_set_handlers(s_animation, (AnimationHandlers) {
    .stopped = on_animation_stopped
  }, NULL);

  animation_schedule(s_animation);
}

static void animate_to_phase(int phase, AnimationAllPhasesCompleteCallback callback) {
  if(s_animation_running) {
    return;
  }
  s_animation_running = true;
  s_animation_callback = callback;
  s_animation_target_phase = phase;
  next_animation(phase < s_animation_phase);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

  s_base_size = (window_bounds.size.w < window_bounds.size.h ? window_bounds.size.w : window_bounds.size.h) / 2;
  s_hour_inner_radius = s_base_size * OUTER_SEGMENT_RATIO / 10000;
  s_minute_inner_radius = s_base_size * INNER_SEGMENT_RATIO / 10000;
  s_minute_block_size = s_base_size * INNER_BLOCK_RATIO / 10000;
  s_center = GPoint(s_base_size, s_base_size);

  APP_LOG(APP_LOG_LEVEL_INFO, "s_minute_block_size: %d", s_minute_block_size);

  layer_set_update_proc(window_get_root_layer(window), update_layer);
}

static void main_window_unload(Window *window) {

}

static void on_time_change_hide_done(void) {
  s_demo_time = s_demo_time + 11;
  // Get a tm structure
  time_t temp = time(NULL);
  s_time = localtime(&temp);
  s_time->tm_hour = s_demo_time % 12;
  s_time->tm_min = s_demo_time % 60;
  // s_time->tm_hour = s_time->tm_hour % 12;
  animate_to_phase(2, NULL);
}

static void update_time() {
  if(s_animation_phase == 2) {
     animate_to_phase(1, on_time_change_hide_done);
  } else {
    time_t temp = time(NULL);
    s_time = localtime(&temp);
    s_time->tm_hour = s_demo_time % 12;
    s_time->tm_min = s_demo_time % 60;
    animate_to_phase(2, NULL);
  }

}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  if(tick_time->tm_sec % 5 == 0) {
    update_time();
  }
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
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  // Make sure the time is displayed from the start
  update_time();

  s_colors.background = GColorFromHEX(0xAAFFFF);
  s_colors.hour = GColorFromHEX(0x0055AA);
  s_colors.min10 = GColorFromHEX(0x00AAFF);
  s_colors.min0 = GColorFromHEX(0x55AAFF);
}

static void deinit() {
  // Destroy Window
  APP_LOG(APP_LOG_LEVEL_INFO, "DESTROY deinit %d", (int)s_animation);
  if(s_animation) {
    animation_destroy(s_animation);
    s_animation = NULL;
  }

  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
