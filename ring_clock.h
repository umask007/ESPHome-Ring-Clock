void update_clock_hands(AddressableLight &it, ESPTime time,
                        Color clock_ring_colors[], int offset[], Color colors[]) {
  static boolean initialized;
  int num_leds = it.size();
//FIXME: allow specifying ring size ring[], eg: {60,48}

/*
esphome globals used:
   id(clock_brightness) bool
   id(clock_fade_enabled) bool
   id(clock_mode) integer
    0: classic analog clock
    1: paint dim arcs of 5 min
    2: paint dim arcs from top
   id(clock_seconds_enabled) bool

*/

  if (initialized == false) {
    std::fill_n(clock_ring_colors, it.size(), Color::BLACK);
    initialized = true;
  }

  if (!time.is_valid()) {
    it[0]=Color(0xfffffff); // all hands at 12
    return;
  }
  // calculate HAND indices
  // if size is <=60, assuming single ring of that size
  int second_idx = (int)(time.second * num_leds / 60.0);
  int minute_idx = (int)(time.minute * num_leds / 60.0);
  int hour_idx   = (int)(0.5f+(time.hour%12 * 60 + time.minute) * num_leds / 720.0f);
  int hour_1_idx = (hour_idx + num_leds -1 ) % num_leds;
  int hour_2_idx = (hour_idx             +1 ) % num_leds;
  int r1_size=num_leds;
  int r2_size=0;
  // if size is >60, assuming outer ring size 60 and inner ring of remaining leds
  // re-calculate HAND indices if there is an inner ring
  if (r1_size > 60) {
    r1_size=60;
    r2_size=num_leds-r1_size;
    second_idx = (int)time.second;
    minute_idx = (int)(time.minute);
    // hours on second ring
    hour_idx   = (int)(0.5f+(time.hour%12 * 60 + time.minute) * r2_size / 720.0f) + 60;
    hour_1_idx = (hour_idx -61 + r2_size ) % r2_size + 60;
    hour_2_idx = (hour_idx -59           ) % r2_size + 60;
  }

  for (int i = 0; i < num_leds; i++) {
    if ( id(clock_fade_enabled) ) {
      int percentage = 98;
      clock_ring_colors[i] *= (uint8)(255*percentage/100);
    } else {
      clock_ring_colors[i] = Color::BLACK;
    }
  }

  // set base colors
  uint8 full = id(clock_brightness); 
  uint8 dim = id(clock_brightness)/3; 
  Color hour_color       = colors[0]*full; //Color(0xe11584)* full; //magenta
  Color minute_color     = colors[1]*full; //Color(0xffd700)* full; //gold
  Color second_color     = colors[2]*full; //Color(0xdc582a)* full; //dark orange
  Color tick_color       = colors[3]*dim; //Color(0x555555)* full; //gray
  Color hour_arc_color   = hour_color * (uint8)(255/3);
  Color minute_arc_color = minute_color * (uint8)(255/3);
  Color second_arc_color = second_color * (uint8)(255/3);

  if (id(clock_mode)==2) { // arc from 0
    for (int i = 0; i < minute_idx; i++) {
      clock_ring_colors[i] = minute_arc_color;
    }
    if (r2_size > 0 ) { 
      for (int i = r1_size; i < hour_idx; i++) {
        clock_ring_colors[i] = hour_arc_color;
      }
    } else {
      for (int i = 0; i < hour_idx; i++) {
        clock_ring_colors[i] = hour_arc_color;
      }
      if (minute_idx < hour_idx ) { //minutes overwritten, repaint them :(
        for (int i = 0; i < minute_idx; i++) {
          clock_ring_colors[i] = minute_arc_color;
        }
      }
    }
  }
  if (id(clock_mode)==1) { // 5 min arc
    int tick=r1_size/12;
    int from= tick*(int)(second_idx/tick);
    int to  = second_idx;
    to  = tick*(int)((second_idx+tick-1)/tick);
    if (from == to) { from--; to++; }
    if (id(clock_seconds_enabled)) {
      for (int i = from; i <= to; i++) {
        clock_ring_colors[(i+r1_size)%r1_size] = second_arc_color;
      }
    }
    from= tick*(int)(minute_idx/tick);
    to  = tick*(int)((minute_idx+tick-1)/tick);
    if (from == to) { from--; to++; }
    for (int i = from; i <= to; i++) {
      clock_ring_colors[(i+r1_size)%r1_size] = minute_arc_color;
    }
    if (r2_size > 0 ) { 
      tick=r2_size/12;
      from= tick*(int)((hour_idx-r1_size)/tick);
      to  = tick*(int)((hour_idx-r1_size+tick-1)/tick);
      if (from == to) { from--; to++; }
      for (int i = from; i <= to; i++) {
        clock_ring_colors[(i+r2_size)%r2_size+r1_size] = hour_arc_color;
      }
    } else {
      // will overwrite hour arc... no mixing
      from= tick*(int)((hour_idx)/tick);
      to  = tick*(int)((hour_idx+tick-1)/tick);
      if (from == to) { from--; to++; }
      for (int i = from; i <= to; i++) {
        clock_ring_colors[(i+r1_size)%r1_size] = hour_arc_color;
      }
    }
  }
  
  // ticks and hands
  if (id(clock_mode)==0) { // classic has ticks
    for (int i = 0; i < r1_size; i+=(int)(r1_size/12)) {
      clock_ring_colors[i] = tick_color;
    }
    /*// 3h ticks on inner ring helps alignment
    for (int i = r1_size; i < num_leds; i+=(int)(r2_size/4)) {
      clock_ring_colors[i] = tick_color;
    }
    */
  }
  clock_ring_colors[hour_idx] = hour_color;
  if (id(clock_mode)==0) { // classic has wide hour hand
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
  for (int i = 0; i < r1_size; i++) {
    x = (i + r1_size + offset[0]) % r1_size;
    it[x] = clock_ring_colors[i];
  }
  if (r2_size > 0 ) {
    for (int i = r1_size; i < num_leds; i++) {
      x = (i - r1_size + offset[1]) % r2_size + r1_size;
      it[x] = clock_ring_colors[i];
    }
  }
}
