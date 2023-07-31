#include <stdio.h>
#include <string.h>

#include "application.h"
#include "microui.h"

#include "icons.h"

static  char logbuf[64000];
static   int logbuf_updated = 0;
static float bg[3] = { 90, 95, 100 };


static void write_log(const char *text) {
  if (logbuf[0]) { strcat(logbuf, "\n"); }
  strcat(logbuf, text);
  logbuf_updated = 1;
}

static mu_Rect last_win = {0};
static void test_window(mu_Context *ctx) {
  /* do window */
  if (mu_begin_window(ctx, "Demo Window", mu_rect(40, 40, 300, 450))) {
    mu_Container *win = mu_get_current_container(ctx);
    win->rect.w = mu_max(win->rect.w, 240);
    win->rect.h = mu_max(win->rect.h, 300);

    /* window info */
    if (mu_header(ctx, "Window Info")) {
      mu_Container *win = mu_get_current_container(ctx);
      char buf[64];
      mu_layout_row(ctx, 2, (int[]) { 54, -1 }, 0);
      mu_label(ctx,"Position:");
      sprintf(buf, "%d, %d", win->rect.x, win->rect.y); mu_label(ctx, buf);
      mu_label(ctx, "Size:");
      sprintf(buf, "%d, %d", win->rect.w, win->rect.h); mu_label(ctx, buf);
    }

    /* labels + buttons */
    if (mu_header_ex(ctx, "Test Buttons", MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 3, (int[]) { 86, -110, -1 }, 0);
      mu_label(ctx, "Test buttons 1:");
      if (mu_button(ctx, "Button 1")) { write_log("Pressed button 1"); }
      if (mu_button(ctx, "Button 2")) { write_log("Pressed button 2"); }
      mu_label(ctx, "Test buttons 2:");
      if (mu_button(ctx, "Button 3")) { write_log("Pressed button 3"); }
      if (mu_button(ctx, "Popup")) { mu_open_popup(ctx, "Test Popup"); }
      if (mu_begin_popup(ctx, "Test Popup")) {
        mu_button(ctx, "Hello");
        mu_button(ctx, "World");
        mu_end_popup(ctx);
      }
    }

    /* tree */
    if (mu_header_ex(ctx, "Tree and Text", MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 2, (int[]) { 140, -1 }, 0);
      mu_layout_begin_column(ctx);
      if (mu_begin_treenode(ctx, "Test 1")) {
        if (mu_begin_treenode(ctx, "Test 1a")) {
          mu_label(ctx, "Hello");
          mu_label(ctx, "world");
          mu_end_treenode(ctx);
        }
        if (mu_begin_treenode(ctx, "Test 1b")) {
          if (mu_button(ctx, "Button 1")) { write_log("Pressed button 1"); }
          if (mu_button(ctx, "Button 2")) { write_log("Pressed button 2"); }
          mu_end_treenode(ctx);
        }
        mu_end_treenode(ctx);
      }
      if (mu_begin_treenode(ctx, "Test 2")) {
        mu_layout_row(ctx, 2, (int[]) { 54, 54 }, 0);
        if (mu_button(ctx, "Button 3")) { write_log("Pressed button 3"); }
        if (mu_button(ctx, "Button 4")) { write_log("Pressed button 4"); }
        if (mu_button(ctx, "Button 5")) { write_log("Pressed button 5"); }
        if (mu_button(ctx, "Button 6")) { write_log("Pressed button 6"); }
        mu_end_treenode(ctx);
      }
      if (mu_begin_treenode(ctx, "Test 3")) {
        static int checks[3] = { 1, 0, 1 };
        mu_checkbox(ctx, "Checkbox 1", &checks[0]);
        mu_checkbox(ctx, "Checkbox 2", &checks[1]);
        mu_checkbox(ctx, "Checkbox 3", &checks[2]);
        mu_end_treenode(ctx);
      }
      mu_layout_end_column(ctx);

      mu_layout_begin_column(ctx);
      mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
      mu_text(ctx, "Lorem ipsum dolor sit amet, consectetur adipiscing "
        "elit. Maecenas lacinia, sem eu lacinia molestie, mi risus faucibus "
        "ipsum, eu varius magna felis a nulla.");
      mu_layout_end_column(ctx);
    }

    /* background color sliders */
    if (mu_header_ex(ctx, "Background Color", MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 2, (int[]) { -78, -1 }, 74);
      /* sliders */
      mu_layout_begin_column(ctx);
      mu_layout_row(ctx, 2, (int[]) { 46, -1 }, 0);
      mu_label(ctx, "Red:");   mu_slider(ctx, &bg[0], 0, 255);
      mu_label(ctx, "Green:"); mu_slider(ctx, &bg[1], 0, 255);
      mu_label(ctx, "Blue:");  mu_slider(ctx, &bg[2], 0, 255);
      mu_layout_end_column(ctx);
      /* color preview */
      mu_Rect r = mu_layout_next(ctx);
      mu_draw_rect(ctx, r, mu_color(bg[0], bg[1], bg[2], 255));
      char buf[32];
      sprintf(buf, "#%02X%02X%02X", (int) bg[0], (int) bg[1], (int) bg[2]);
      mu_draw_control_text(ctx, buf, r, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
    }

    mu_end_window(ctx);
  }
}


static void log_window(mu_Context *ctx) {
  if (mu_begin_window(ctx, "Log Window", mu_rect(350, 40, 300, 200))) {
    /* output text panel */
    mu_layout_row(ctx, 1, (int[]) { -1 }, -25);
    mu_begin_panel(ctx, "Log Output");
    mu_Container *panel = mu_get_current_container(ctx);
    mu_layout_row(ctx, 1, (int[]) { -1 }, -1);
    mu_text(ctx, logbuf);
    mu_end_panel(ctx);
    if (logbuf_updated) {
      panel->scroll.y = panel->content_size.y;
      logbuf_updated = 0;
    }

    /* input textbox + submit button */
    static char buf[128];
    int submitted = 0;
    mu_layout_row(ctx, 2, (int[]) { -70, -1 }, 0);
    if (mu_textbox(ctx, buf, sizeof(buf)) & MU_RES_SUBMIT) {
      mu_set_focus(ctx, ctx->last_id);
      submitted = 1;
    }
    if (mu_button(ctx, "Submit")) { submitted = 1; }
    if (submitted) {
      write_log(buf);
      buf[0] = '\0';
    }

    mu_end_window(ctx);
  }
}


static int uint8_slider(mu_Context *ctx, unsigned char *value, int low, int high) {
  static float tmp;
  mu_push_id(ctx, &value, sizeof(value));
  tmp = *value;
  int res = mu_slider_ex(ctx, &tmp, low, high, 0, "%.0f", MU_OPT_ALIGNCENTER);
  *value = tmp;
  mu_pop_id(ctx);
  return res;
}


static void style_window(mu_Context *ctx) {
  static struct { const char *label; int idx; } colors[] = {
    { "text:",         MU_COLOR_TEXT        },
    { "border:",       MU_COLOR_BORDER      },
    { "windowbg:",     MU_COLOR_WINDOWBG    },
    { "titlebg:",      MU_COLOR_TITLEBG     },
    { "titletext:",    MU_COLOR_TITLETEXT   },
    { "panelbg:",      MU_COLOR_PANELBG     },
    { "button:",       MU_COLOR_BUTTON      },
    { "buttonhover:",  MU_COLOR_BUTTONHOVER },
    { "buttonfocus:",  MU_COLOR_BUTTONFOCUS },
    { "base:",         MU_COLOR_BASE        },
    { "basehover:",    MU_COLOR_BASEHOVER   },
    { "basefocus:",    MU_COLOR_BASEFOCUS   },
    { "scrollbase:",   MU_COLOR_SCROLLBASE  },
    { "scrollthumb:",  MU_COLOR_SCROLLTHUMB },
    { NULL }
  };

  if (mu_begin_window(ctx, "Style Editor", mu_rect(350, 250, 300, 240))) {
    int sw = mu_get_current_container(ctx)->body.w * 0.14;
    mu_layout_row(ctx, 6, (int[]) { 80, sw, sw, sw, sw, -1 }, 0);
    for (int i = 0; colors[i].label; i++) {
      mu_label(ctx, colors[i].label);
      uint8_slider(ctx, &ctx->style->colors[i].r, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].g, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].b, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].a, 0, 255);
      mu_draw_rect(ctx, mu_layout_next(ctx), ctx->style->colors[i]);
    }
    mu_end_window(ctx);
  }
}

// int mu_multiline_textbox_raw(mu_Context *ctx, char *buf, int bufsz, mu_Rect r,int opt)
// {
//   mu_Id id = mu_get_id(ctx, &buf, sizeof(buf));
//   int res = 0;
//   mu_update_control(ctx, id, r, opt | MU_OPT_HOLDFOCUS);

//   if (ctx->focus == id) {
//     /* handle text input */
//     int len = strlen(buf);
//     int n = mu_min(bufsz - len - 1, (int) strlen(ctx->input_text));
//     if (n > 0) {
//       memcpy(buf + len, ctx->input_text, n);
//       len += n;
//       buf[len] = '\0';
//       res |= MU_RES_CHANGE;
//     }
//     /* handle backspace */
//     if (ctx->key_pressed & MU_KEY_BACKSPACE && len > 0) {
//       /* skip utf-8 continuation bytes */
//       while ((buf[--len] & 0xc0) == 0x80 && len > 0);
//       buf[len] = '\0';
//       res |= MU_RES_CHANGE;
//     }
//     /* handle return */
//     if (ctx->key_pressed & MU_KEY_RETURN) {
//       mu_set_focus(ctx, 0);
//       res |= MU_RES_SUBMIT;
//     }
//   }

//   /* draw */
//   mu_draw_control_frame(ctx, id, r, MU_COLOR_BASE, opt);
//   if (ctx->focus == id) {
//     mu_Color color = ctx->style->colors[MU_COLOR_TEXT];
//     mu_Font font = ctx->style->font;
//     int textw = ctx->text_width(font, buf, -1);
//     int texth = ctx->text_height(font);
//     int ofx = r.w - ctx->style->padding - textw - 1;
//     int textx = r.x + mu_min(ofx, ctx->style->padding);
//     int texty = r.y + (r.h - texth) / 2;
//     mu_push_clip_rect(ctx, r);
//     mu_draw_text(ctx, font, buf, -1, mu_vec2(textx, texty), color);
//     mu_draw_rect(ctx, mu_rect(textx + textw, texty, 1, texth), color);
//     mu_pop_clip_rect(ctx);
//   } else {
//     // strstr()
//     mu_draw_control_text(ctx, buf, r, MU_COLOR_TEXT, opt);
//   }

//   return res;
// }

    // char* sub = start;
    // memset(line,0,2048);
    // memcpy(line,sub,size);
    // mu_Id id = mu_get_id(ctx, &start, sizeof(size));
    // int adder = 1;
    // if(mu_textbox_raw(ctx, line, 2048,id,r, opt) & MU_RES_CHANGE){
    //   size_t end_len = strlen(line);
    //   if(end_len < size){
    //     while(sub[end_len] != '\0'){
    //       sub[end_len++] = sub[end_len+1];
    //     }
    //     adder = 0;
    //   }
    //   else {
    //     size_t write = end_len;
    //     char keep = sub[end_len-1];
    //     while(keep !='\0'){
    //       char nk = sub[end_len]; 
    //       sub[end_len++] = keep;
    //       keep = nk;
    //     }
    //     memcpy(sub,line,write);
    //     adder +=1;
    //   }
    //   return 1;
    // }
    // mu_draw_text(ctx, font, " ", 1, mu_vec2(r.x, r.y), color);

typedef struct {
  char* text;
  size_t len;
  mu_Rect r;
}
data_text_t;

int mu_multiline_textbox_ex(mu_Context *ctx, char *buf, int bufsz, int opt) {
  mu_Container* cnt = mu_get_current_container(ctx);
  char *start, *end, *p = buf;
  int width = -1;
  mu_Font font = ctx->style->font;
  mu_Color color = ctx->style->colors[MU_COLOR_TEXT];
  
  mu_layout_begin_column(ctx);
  // mu_layout_row(ctx, 1, &width, ctx->text_height(font));
  mu_layout_row(ctx, 1, (int[]) { -1}, ctx->text_height(font));
  mu_Rect init_rect = mu_layout_next(ctx);
  init_rect.w = cnt->body.w - ctx->style->padding * 2;
  mu_Rect last = init_rect;
  mu_Id id = mu_get_id(ctx, &buf, sizeof(buf));
  data_text_t data[1024] = {0};
  size_t d_len = 0;
  do {
    data_text_t* node = &data[d_len++];
    int w = 0;
    start = end = p;
    char line[2048] = {0};
    mu_Rect r = last;
    init_rect.h = r.y - init_rect.y + r.h;
    r.w = r.w < init_rect.w ? init_rect.w : r.w;
    node->r = r;
    do {
      const char* word = p;
      while (*p && *p != '\n') { p++; }
      w += ctx->text_width(font, word, p - word);
      if (w > r.w && end != start) { break; }
      w += ctx->text_width(font, p, 1);
      end = p++;
    } while (*end && *end != '\n');

    size_t size = p != start ? end - start: 1;
    node->len = size;
    node->text = start;
    p = end + 1;
    last = mu_layout_next(ctx);
  } while (*end);

  int res =0;
  mu_update_control(ctx, id, init_rect, opt | MU_OPT_HOLDFOCUS);
  mu_draw_control_frame(ctx, id, init_rect, MU_COLOR_BASE, opt);
  for(int i =0; i < d_len;++i){
    data_text_t node = data[i];
    mu_Rect r = node.r; 
    if (ctx->focus == id) {
      static mu_Vec2 m_pos = {0};
      /* handle text input */
      int len = strlen(buf);
      int n = mu_min(bufsz - len - 1, (int) strlen(ctx->input_text));
      if (n > 0) {
        memcpy(buf + len, ctx->input_text, n);
        len += n;
        buf[len] = '\0';
        res |= MU_RES_CHANGE;
      }
      /* handle backspace */
      if (ctx->key_pressed & MU_KEY_BACKSPACE && len > 0) {
        /* skip utf-8 continuation bytes */
        while ((buf[--len] & 0xc0) == 0x80 && len > 0);
        buf[len] = '\0';
        res |= MU_RES_CHANGE;
      }
      /* handle return */
      if (ctx->key_pressed & MU_KEY_RETURN) {
        mu_set_focus(ctx, 0);
        res |= MU_RES_SUBMIT;
      }

      if(ctx->mouse_down){
        m_pos = ctx->mouse_pos;
      }
      mu_Color color = ctx->style->colors[MU_COLOR_TEXT];
      mu_Font font = ctx->style->font;
      int textw = ctx->text_width(font, node.text,node.len);
      int offsetx = m_pos.x - r.x;
      int width = 0;
      for(int i =0; i < node.len;++i){
        char temp[2] = {0};
        temp[0] = node.text[i];
        int nwidth = ctx->text_width(font, temp,1); 
        if(offsetx < width+ nwidth){
          offsetx = width;
          break;
        }
        width+= nwidth;
      }
      offsetx = textw < offsetx ? textw : offsetx;
      int texth = ctx->text_height(font);
      int ofx = r.w - textw - 1;
      int textx = r.x + mu_min(ofx, ctx->style->padding);
      int texty = r.y + (r.h - texth) / 2;
      mu_push_clip_rect(ctx, r);
      mu_draw_text(ctx, font, node.text, node.len, mu_vec2(textx, texty), color);
      if(r.x + r.w > m_pos.x && r.x <= m_pos.x && r.y <= m_pos.y && r.y + r.h > m_pos.y){
        mu_draw_rect(ctx, mu_rect(textx + offsetx, texty, 1, texth), color);
      } 
      mu_pop_clip_rect(ctx);
    }
    else {
      mu_Vec2 pos;
      int tw = ctx->text_width(font, node.text, node.len);
      mu_push_clip_rect(ctx, r);
      pos.y = r.y + (r.h - ctx->text_height(font)) / 2;
      if (opt & MU_OPT_ALIGNCENTER) {
        pos.x = r.x + (r.w - tw) / 2;
      } else if (opt & MU_OPT_ALIGNRIGHT) {
        pos.x = r.x + r.w - tw - ctx->style->padding;
      } else {
        pos.x = r.x + ctx->style->padding;
      }
      mu_draw_text(ctx, font, node.text,node.len, mu_vec2(r.x, r.y), color);
      mu_pop_clip_rect(ctx);
    }
  }
  mu_layout_end_column(ctx);

  return res;
}
// int mu_multiline_textbox_ex(mu_Context *ctx, char *buf, int bufsz, int opt) {
//   size_t first = 0;
//   size_t last = 0;
//   char line[2048] = {0};
//   size_t buflen = strlen(buf);
//   while(last != buflen+1){
//     if(buf[last] == '\n' || last == buflen){
//       size_t size = 1;
//       if(last != first){
//         size = last-first;
//       }
//       char* sub = &buf[first];
//       first = last+1;
//       memset(line,0,2048);
//       memcpy(line,sub,size);
//       mu_layout_row(ctx, 1, (int[]) { -1,-1 }, 0);
//       mu_Id id = mu_get_id(ctx, &sub, sizeof(size));
//       mu_Rect r = mu_layout_next(ctx);
//       if(mu_textbox_raw(ctx, line, 2048,id,r, opt) & MU_RES_CHANGE){
//         size_t end_len = strlen(line);
//         if(end_len < size){
//           while(sub[end_len] != '\0'){
//             sub[end_len++] = sub[end_len+1];
//           }
//           buflen--;
//         }
//         else {
//           size_t write = end_len;
//           char keep = sub[end_len-1];
//           while(keep !='\0'){
//             char nk = sub[end_len]; 
//             sub[end_len++] = keep;
//             keep = nk;
//           }
//           memcpy(sub,line,write);
//           buflen++;
//         }
//         return 1;
//       }
//     }
//     last++;
//   }
//   return 1;
//   // mu_Id id = mu_get_id(ctx, &buf, sizeof(buf));
//   // mu_Rect r = mu_layout_next(ctx);
//   // return mu_textbox_raw(ctx, buf, bufsz,id,r, opt);
// }

static char* lorem_text = NULL;
static void texteditor_window(mu_Context *ctx){
  if (mu_begin_window(ctx, "Text Editor", mu_rect(650, 250, 300, 240))){
    mu_multiline_textbox_ex(ctx,lorem_text,sizeof(char) * 2048 *2,0);
    mu_end_window(ctx);
  }
}


static void process_frame(mu_Context *ctx) {
  mu_begin(ctx);
  style_window(ctx);
  log_window(ctx);
  test_window(ctx);
  texteditor_window(ctx);
  mu_end(ctx);
}

mu_Context* ctx;
int64_t now = 0;
int t_last_x = 0;
int text_width(mu_Font font, const char *str, int len){
  return ren_get_font_width((RenFont*)font,str,len);
}
int text_height(mu_Font font){
  return ren_get_font_height((RenFont*)font);
}
int key_conversion[128] = {0};
void application_init(void){
  lorem_text = malloc(sizeof(char) * 2048 *2);
  /*
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Quis eleifend quam adipiscing vitae proin sagittis nisl rhoncus mattis. Scelerisque felis imperdiet proin fermentum leo vel orci porta. In hac habitasse platea dictumst vestibulum rhoncus est. Ullamcorper sit amet risus nullam eget felis. Condimentum id venenatis a condimentum vitae sapien pellentesque habitant. Feugiat sed lectus vestibulum mattis ullamcorper velit sed. Aliquam id diam maecenas ultricies mi. Justo eget magna fermentum iaculis eu non diam. Ac tortor vitae purus faucibus ornare suspendisse. Dui vivamus arcu felis bibendum ut tristique et egestas quis. Orci nulla pellentesque dignissim enim sit amet venenatis. Sit amet consectetur adipiscing elit ut aliquam purus. Quis vel eros donec ac odio tempor orci dapibus. Maecenas pharetra convallis posuere morbi leo urna molestie. Auctor augue mauris augue neque gravida in fermentum et. Hac habitasse platea dictumst vestibulum rhoncus est. Et netus et malesuada fames ac turpis egestas.\n\n
  Lectus vestibulum mattis ullamcorper velit sed ullamcorper morbi tincidunt. Diam quis enim lobortis scelerisque fermentum dui faucibus. Nullam non nisi est sit amet facilisis magna etiam. Amet aliquam id diam maecenas ultricies mi. Risus commodo viverra maecenas accumsan lacus vel facilisis volutpat est. Arcu cursus euismod quis viverra nibh cras pulvinar. Sapien eget mi proin sed libero enim sed faucibus turpis. Ipsum suspendisse ultrices gravida dictum fusce ut. Velit aliquet sagittis id consectetur purus. Condimentum vitae sapien pellentesque habitant morbi tristique senectus. Felis eget nunc lobortis mattis aliquam faucibus purus in massa. Scelerisque purus semper eget duis at tellus. Quam adipiscing vitae proin sagittis nisl. Nibh ipsum consequat nisl vel. Quis viverra nibh cras pulvinar mattis nunc.\n\n
  Sed augue lacus viverra vitae congue eu consequat ac. Amet consectetur adipiscing elit pellentesque habitant morbi tristique senectus. Dolor purus non enim praesent elementum facilisis leo. Libero justo laoreet sit amet cursus. Proin fermentum leo vel orci porta non pulvinar neque. Imperdiet massa tincidunt nunc pulvinar sapien et. Placerat vestibulum lectus mauris ultrices eros in cursus. Curabitur vitae nunc sed velit. Augue lacus viverra vitae congue eu consequat ac felis. Lorem ipsum dolor sit amet."
  */
  strcpy(lorem_text,
  "Hello World !\n\n"
  "This is a test yo.\n\n"
  "Me love you long time.");
  for(int i =0; i < 128;++i){
    if(i == 10){
      key_conversion[i] = MU_KEY_RETURN;
    }
    else {
      key_conversion[i] =i;
    }
  }
  now = fenster_time();
  ctx = malloc(sizeof(mu_Context));
  mu_init(ctx);
  ctx->text_width = text_width ;
  ctx->text_height = text_height;
  ctx->style->font = ren_load_font("assets/fonts/font.ttf",14);
}
RenColor color = {.r=255,.b=0,.g=0,.a=255};
RenRect window_rect = {0};
RenColor bg_color = {.r=0,.b=0,.g=0,.a=255};
int y =100;
int last_x,last_y = 0;
int keys_state[128] = {0};
int mouse_state[3] = {0};
void application_update(fenster_window* f){
    window_rect.width = f->width;
    window_rect.height = f->height;
    int max = 32;
    for (int i = 0; i < max; i++) {
      if (keys_state[i] != f->keys[i]) {
        keys_state[i] = f->keys[i];
        if(keys_state[i]){
          mu_input_keydown(ctx,key_conversion[i]);
        }
        else{
          mu_input_keyup(ctx,key_conversion[i]);
        }
      }
      // if(i == 31){
      //   i = 90;
      //   max = 97;
      // }
      // if( i == 96){
      //   i = 122;
      //   max = 128;
      // }
    }
    size_t len = strlen((char*)&f->keys[128]);
    if(len > 0){
      char tex[32] = {0};
      for(int i = 0; i < len;++i){
        int key = f->keys[128+i];
        tex[i] = key;
      }
      printf("%s\n",tex);
      mu_input_text(ctx,tex);
    }
    int left_down = (f->mouse >> MOUSE_LEFT_BUTTON) & 1;
    int middle_down = (f->mouse >> MOUSE_MIDDLE_BUTTON) & 1;
    int right_down = (f->mouse >> MOUSE_RIGHT_BUTTON) & 1;

    if(mouse_state[0] != left_down){
        mouse_state[0] = left_down;
        if(left_down)
            mu_input_mousedown(ctx,f->x,f->y,MU_MOUSE_LEFT);
        else{
            mu_input_mouseup(ctx,f->x,f->y,MU_MOUSE_LEFT);
        }
    }
    if(mouse_state[1] != middle_down){
        mouse_state[1] = middle_down;
        if(middle_down)
            mu_input_mousedown(ctx,f->x,f->y,MU_MOUSE_MIDDLE);
        else{
            mu_input_mouseup(ctx,f->x,f->y,MU_MOUSE_MIDDLE);
        }
    }
    if(mouse_state[2] != right_down){
        mouse_state[2] = right_down;
        if(left_down)
            mu_input_mousedown(ctx,f->x,f->y,MU_MOUSE_RIGHT);
        else{
            mu_input_mouseup(ctx,f->x,f->y,MU_MOUSE_RIGHT);
        }
    }
    if(last_x != f->x || last_y != f->y){
        last_x = f->x;
        last_y = f->y;

        mu_input_mousemove(ctx,f->x,f->y);
    }

    process_frame(ctx);

    // rencache_show_debug(true);
    rencache_begin_frame();
    rencache_draw_rect(window_rect,*(RenColor*)&ctx->style->colors[MU_COLOR_WINDOWBG]);
    mu_Command *cmd = NULL;
    while (mu_next_command(ctx, &cmd)) {
      switch (cmd->type) {
        case MU_COMMAND_TEXT: 
            rencache_draw_text(ctx->style->font,cmd->text.str, cmd->text.pos.x,cmd->text.pos.y, *(RenColor*)&cmd->text.color);
            break;
        case MU_COMMAND_RECT:
            rencache_draw_rect(*(RenRect*)&cmd->rect.rect, *(RenColor*)&cmd->rect.color);
            break;
        case MU_COMMAND_ICON:
            if(cmd->icon.id < 5){
              static RenImage imgs[] = {
                {0},
                {.pixels=(RenColor*)icon_close_pixels,.width=icon_close_width,.height=icon_close_height},
                {.pixels=(RenColor*)icon_close_pixels,.width=icon_close_width,.height=icon_close_height},
                {.pixels=(RenColor*)icon_collapsed_pixels,.width=icon_collapsed_width,.height=icon_collapsed_height},
                {.pixels=(RenColor*)icon_expanded_pixels,.width=icon_expanded_width,.height=icon_expanded_height},
              };
              RenImage img = imgs[cmd->icon.id];
              RenRect sub = {0,0,img.width,img.height};
              rencache_draw_img(&imgs[cmd->icon.id],sub,cmd->rect.rect.x,cmd->rect.rect.y,*(RenColor*)&cmd->icon.color);
            }
            // r_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color);
            break;
        case MU_COMMAND_CLIP: rencache_set_clip_rect(*(RenRect*)&cmd->clip.rect); break;
      }
    }
    // RenRect temp = {0};
    // temp.x = temp.y = temp.height = temp.width = 100;
    // // rencache_draw_circle(100,100,20,color);
    // // rencache_draw_line(0,0,f->x,f->y,color);

    // rencache_draw_triangle(0,0,0,100,f->x,f->y,color);

    // last_x = f->x;
    // last_y = f->y;
    
    rencache_end_frame();

    int64_t time = fenster_time();
    if (time - now < 1000 / 60) {
      fenster_sleep(time - now);
    }
    now = time;
}