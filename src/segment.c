#include <pebble.h>

#define HOUR_OUTER_SEGMENT_RATIO 0.69;

typedef void (*AnimationAllPhasesCompleteCallback)(void);

const int SEGMENT_SIZE = 15;

struct Colors {
  GColor background;
  GColor hour;
  GColor min1;
  GColor min0;
};

static Window *s_window;
static struct tm *s_time;
static int s_hour_inner_radius;

static Animation *s_animation;
static AnimationImplementation s_animation_implementation;
static int s_animation_progress;
static bool s_animation_running = false;
static int s_animation_phase = 0;
static int s_animation_target_phase = 0;
static AnimationAllPhasesCompleteCallback s_animation_callback;
static int s_animation_phase_timing[3] = {
  400,
  400,
  500
};

static struct Colors s_colors;

// max width or height we can make our shapes fit in
static int s_base_size;
static int s_demo_time = 1;

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

static void draw_hour_outer_segment(GContext *ctx, int start, int end, int anim_start, bool swap_direction) {
  graphics_context_set_fill_color(ctx, s_colors.hour);

  if (swap_direction) {
    graphics_fill_radial(
      ctx,
      GPoint(s_base_size, s_base_size),
      s_hour_inner_radius,
      s_base_size,
      animation_distance(get_segment_angle(anim_start), get_segment_angle(start)),
      get_segment_angle(end)
    );
  } else {
    graphics_fill_radial(
      ctx,
      GPoint(s_base_size, s_base_size),
      s_hour_inner_radius,
      s_base_size,
      get_segment_angle(start),
      animation_distance(get_segment_angle(anim_start), get_segment_angle(end))
    );
  }
}

static void draw_hour_inner_segment(GContext *ctx, int start, int end) {
  graphics_context_set_fill_color(ctx, s_colors.hour);
  graphics_fill_radial(ctx, GPoint(s_base_size, s_base_size), 0, s_base_size, get_segment_angle(start),
                       get_segment_angle(end));
}


static void draw_outer_expand(GContext *ctx) {
  graphics_context_set_fill_color(ctx, s_colors.hour);
  graphics_fill_circle(ctx, GPoint(s_base_size, s_base_size), animation_distance(0, s_base_size));
}

static void draw_outer_reveal(GContext *ctx) {
  graphics_context_set_fill_color(ctx, s_colors.hour);
//  APP_LOG(APP_LOG_LEVEL_INFO, "animation_distance_linear(0, s_hour_inner_radius): %d", animation_distance_linear(0, s_hour_inner_radius));
  graphics_fill_radial(
    ctx, GPoint(s_base_size, s_base_size),
    animation_distance(0, s_hour_inner_radius),
    s_base_size,
    get_segment_angle(0),
    get_segment_angle(24)
  );
}


static void draw_outer(GContext *ctx) {
  switch (s_time->tm_hour) {
    case 1 :
    case 13 :
      draw_hour_outer_segment(ctx, 1, 11, 25, false);
      break;

    case 2 :
    case 14 :
      draw_hour_outer_segment(ctx, -3, 4, -9, true); // top
      draw_hour_outer_segment(ctx, 9, 16, 3, true); // bottom
      break;

    case 3 :
    case 15 :
      draw_hour_outer_segment(ctx, -3, 15, 21, false);
      break;

    case 4 :
    case 16 :
//      draw_hour_outer_segment(ctx, -3, 15);
//      draw_hour_inner_segment(ctx, 5, 7);
      break;
  }
}

static void draw_inner(GContext *ctx) {
  switch (s_time->tm_hour) {
    case 1 :
    case 13 :
      break;

    case 2 :
    case 14 :
      draw_hour_inner_segment(ctx, 2, 4);
      draw_hour_inner_segment(ctx, 14, 16);
      break;

    case 3 :
    case 15 :
      draw_hour_inner_segment(ctx, 5, 7);
      break;

    case 4 :
    case 16 :
      draw_hour_inner_segment(ctx, 5, 7);
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
      draw_outer_reveal(ctx);
      draw_inner(ctx);
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

  APP_LOG(APP_LOG_LEVEL_INFO, "s_animation_target_phase: %d", s_animation_target_phase);
  APP_LOG(APP_LOG_LEVEL_INFO, "s_animation_phase: %d", s_animation_phase);

  if(s_animation_phase < s_animation_target_phase) {
    s_animation_progress = ANIMATION_NORMALIZED_MIN;
    ++s_animation_phase;
    next_animation(false);
  } else if(s_animation_phase > s_animation_target_phase) {
    s_animation_progress = ANIMATION_NORMALIZED_MAX;
    --s_animation_phase;
    next_animation(true);
  } else {
    APP_LOG(APP_LOG_LEVEL_INFO, "s_animation_progress: %d", s_animation_progress);
    s_animation_running = false;
    if(s_animation_callback) {
      s_animation_callback();
    }
  }
}

static void next_animation(bool reverse) {
  if(s_animation) {
    animation_destroy(s_animation);
  }
  s_animation = animation_create();
  s_animation_implementation.update = update_animation;

  animation_set_duration(s_animation, s_animation_phase_timing[s_animation_phase]);
  animation_set_reverse(s_animation, reverse);
  animation_set_curve(s_animation, AnimationCurveLinear);
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

  s_hour_inner_radius = s_base_size * HOUR_OUTER_SEGMENT_RATIO;

  layer_set_update_proc(window_get_root_layer(window), update_layer);
}

static void main_window_unload(Window *window) {

}

static void on_time_change_hide_done(void) {
  ++s_demo_time;
  // Get a tm structure
  time_t temp = time(NULL);
  s_time = localtime(&temp);
  s_time->tm_hour = s_demo_time;
  animate_to_phase(2, NULL);
}

static void update_time() {

  // @TODO fake time here
  APP_LOG(APP_LOG_LEVEL_INFO, "UPDATE_TIME - s_animation_phase: %d", s_animation_phase);

  if(s_animation_phase == 2) {
     animate_to_phase(1, on_time_change_hide_done);
  } else {
    time_t temp = time(NULL);
    s_time = localtime(&temp);
    s_time->tm_hour = s_demo_time;
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
}

static void deinit() {
  // Destroy Window
  animation_destroy(s_animation);
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

//  APP_LOG(APP_LOG_LEVEL_INFO, "draw_hour_animation_phase_0: %d", size);




//static void animation_3_stopped(Animation *animation, bool stopped, void *ctx) {
//  s_animation_progress = ANIMATION_NORMALIZED_MAX;
//  APP_LOG(APP_LOG_LEVEL_INFO, "STOPPED");
//}
//
//
//static void animation_phase_3() {
//  s_animation_phase = 3;
//
//  s_animation = animation_create();
//  animation_set_duration(s_animation, s_animation_timing.phase_3_duration);
//  s_animation_implementation.update = update_animation;
//
//  animation_set_implementation(s_animation, &s_animation_implementation);
//  animation_set_handlers(
//    s_animation, (AnimationHandlers) {
//    .stopped = animation_3_stopped
//  }, NULL);
//
//  animation_schedule(s_animation);
//}
//
//
//static void animation_2_stopped(Animation *animation, bool stopped, void *ctx) {
//  s_animation_progress = ANIMATION_NORMALIZED_MIN;
//  animation_phase_3();
//}
//
//static void animation_2_stopped_reverse(Animation *animation, bool stopped, void *ctx) {
//  s_animation_progress = ANIMATION_NORMALIZED_MIN;
//  animation_phase_2(false);
//}
//
//static void animation_phase_2(bool reverse) {
//  s_animation_phase = 2;
//  s_animation = animation_create();
//  animation_set_duration(s_animation, s_animation_timing.phase_2_duration);
//  animation_set_reverse(s_animation, reverse);
//  s_animation_implementation.update = update_animation;
//
//  animation_set_implementation(s_animation, &s_animation_implementation);
//  if(reverse) {
//    animation_set_handlers(
//      s_animation, (AnimationHandlers) {
//      .stopped = animation_2_stopped_reverse
//    }, NULL);
//  }else {
//    animation_set_handlers(
//      s_animation, (AnimationHandlers) {
//      .stopped = animation_2_stopped
//    }, NULL);
//  }
//
//  animation_schedule(s_animation);
//}
//
//
//static void animation_1_stopped(Animation *animation, bool stopped, void *ctx) {
//  s_animation_progress = ANIMATION_NORMALIZED_MIN;
//  animation_phase_2(false);
//}
//
//static void animation_phase_1() {
//  s_animation_phase = 1;
//  s_animation = animation_create();
//  animation_set_duration(s_animation, s_animation_timing.phase_1_duration);
//  s_animation_implementation.update = update_animation;
//  animation_set_curve(s_animation, AnimationCurveLinear);
//  animation_set_implementation(s_animation, &s_animation_implementation);
//  animation_set_handlers(
//    s_animation, (AnimationHandlers) {
//    .stopped = animation_1_stopped
//  }, NULL);
//
//  animation_schedule(s_animation);
//}
