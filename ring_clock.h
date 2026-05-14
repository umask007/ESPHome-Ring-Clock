void update_clock_hands(AddressableLight &it, ESPTime time,
                        Color clock_ring_colors[], int offset[], Color colors[]) {
  static boolean initialized;
  int num_leds = it.size();

/*
esphome globals used:
   id(clock_brightness) bool
   id(clock_fade_enabled) bool
   id(clock_mode) integer
    0: classic analog clock
    1: paint dim arcs of 5 min
    2: paint dim arcs from top
    3: classic inverse minutes
   id(clock_seconds_enabled) bool
*/
#define MODE_ANALOG 0
#define MODE_ARC5 1
#define MODE_ARC12 2
#define MODE_AINV 3

  if (initialized == false) {
    std::fill_n(clock_ring_colors, it.size(), Color::BLACK);
    initialized = true;
  }

  if (!time.is_valid()) {
    it[0]=Color(0xfffffff); // all hands at 12
    return;
  }

  // calculate hand indices
  // if num_leds is <=60, assuming single ring of that size
  int r1_size=num_leds;
  int r2_size=num_leds;
  int r2_base=0;
  // if num_leds is >60, assuming outer ring size 60 and inner ring of remaining leds
  if (r1_size > 60) {
    r1_size=60;
    r2_size=num_leds-r1_size;
    r2_base=r1_size;
  }

  int second_idx = (int)(time.second * r1_size / 60.0);
  int minute_idx = (int)(time.minute * r1_size / 60.0);
  int minute_1_idx = (minute_idx -1 + r1_size ) % r1_size;
  int minute_2_idx = (minute_idx +1           ) % r1_size;
  int hour_idx   = (int)(0.5f+(time.hour%12 * 60 + time.minute) * r2_size / 720.0f)+r2_base;
  int hour_1_idx = (hour_idx-r2_base -1 + r2_size ) % r2_size + r2_base;
  int hour_2_idx = (hour_idx-r2_base +1           ) % r2_size + r2_base;

  // set base colors
  uint8 full = id(clock_brightness); 
  uint8 dim = id(clock_brightness)/3; 
  Color hour_color       = colors[0]*full;
  Color minute_color     = colors[1]*full;
  Color second_color     = colors[2]*full;
  Color tick_color       = colors[3]*dim;
  Color hour_arc_color   = hour_color * (uint8)(255/3);
  Color minute_arc_color = minute_color * (uint8)(255/3);
  Color second_arc_color = second_color * (uint8)(255/3);

  for (int i = 0; i < num_leds; i++) {
    if ( id(clock_fade_enabled) ) {
      int percentage = 98;
      clock_ring_colors[i] *= (uint8)(255*percentage/100);
    } else {
      if (id(clock_mode)==MODE_AINV) { // inverted
        if ( i < r1_size ) {
          clock_ring_colors[i] = minute_color;
        } else {
          clock_ring_colors[i] = hour_color;
        }
      } else {
        clock_ring_colors[i] = Color::BLACK;
      }
    }
  }
  if (id(clock_mode)==MODE_AINV) { // inverted
    minute_color= Color::BLACK;
    hour_color= Color::BLACK;
    tick_color= minute_color * (uint8)(2*255/3);
  }

  if (id(clock_mode)==MODE_ARC12) { // arc from 0
    for (int i = 0; i < minute_idx; i++) {
      clock_ring_colors[i] = minute_arc_color;
    }
    for (int i = r2_base; i < hour_idx; i++) {
      clock_ring_colors[i] = hour_arc_color;
    }
    if (r2_base == 0 ) { 
      if (minute_idx < hour_idx ) { //minutes got overwritten, repaint them :(
        for (int i = 0; i < minute_idx; i++) {
            clock_ring_colors[i] = minute_arc_color;
        }
      }
    }
  }
  if (id(clock_mode)==MODE_ARC5) { // 5 min arc
    int tick=r1_size/12;
    int from= tick*(int)(minute_idx/tick);
    int to  = tick*(int)((minute_idx+tick-1)/tick);
    if (from == to) { from-=tick; to+=tick; }
    for (int i = from; i <= to; i++) {
      clock_ring_colors[(i+r1_size)%r1_size] = minute_arc_color;
    }
    if (r2_base > 0 ) { 
      tick=r2_size/12;
      from= tick*(int)((hour_idx-r2_base)/tick);
      to  = tick*(int)((hour_idx-r2_base+tick-1)/tick);
      if (from == to) { from-=tick; to+=tick; }
      for (int i = from; i <= to; i++) {
        clock_ring_colors[(i+r2_size)%r2_size+r2_base] = hour_arc_color;
      }
    }
  }
  
  // ticks and hands
  if (id(clock_mode)==MODE_ANALOG) { // classic has ticks
    for (int i = 0; i < r1_size; i+=(int)(r1_size/12)) {
      clock_ring_colors[i] = tick_color;
    }
    // 3h ticks on inner ring helps alignment
    for (int i = r1_size; i < num_leds; i+=(int)(r2_size/4)) {
      clock_ring_colors[i] = tick_color;
    }
    
  }
  clock_ring_colors[hour_idx] = hour_color;
  if ((id(clock_mode)==MODE_AINV)) { // wide minute hand
    clock_ring_colors[minute_1_idx] = minute_color;
    clock_ring_colors[minute_2_idx] = minute_color;
  }
  if ((id(clock_mode)==MODE_ANALOG)||
      (id(clock_mode)==MODE_ARC5)||
      (id(clock_mode)==MODE_AINV)) { // wide hour hand
    clock_ring_colors[hour_1_idx] = hour_color;
    clock_ring_colors[hour_2_idx] = hour_color;
  }
  // overwriting sides of wide hour hand (if any)
  if ( minute_idx == hour_idx ) { // mix
    clock_ring_colors[minute_idx] += minute_color;
  } else {
    clock_ring_colors[minute_idx] = minute_color;
  }
  if (id(clock_seconds_enabled)) {
    if ( second_idx == hour_idx || second_idx == minute_idx ) {
      clock_ring_colors[second_idx] += second_color;
    } else {
      clock_ring_colors[second_idx] = second_color;
    }
  }

  // apply clock_ring_colors to light
  int x=0;
  //each ring has its own offset
  for (int i = 0; i < r1_size; i++) {
    x = (i + r1_size + offset[0]) % r1_size;
    it[x] = clock_ring_colors[i];
  }
  if (r2_base > 0 ) {
    for (int i = r2_base; i < num_leds; i++) {
      x = (i - r2_base + offset[1]) % r2_size + r2_base;
      it[x] = clock_ring_colors[i];
    }
  }
}
