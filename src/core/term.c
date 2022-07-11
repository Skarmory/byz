#include "core/term.h"

#include "core/bit_flags.h"
#include "core/colour.h"
#include "core/log.h"

#include <sys/ioctl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define PRINT_BUFFER_SIZE (1024 * 1024)

#define c_escape "\033"
#define c_escape_kind "["
#define c_graphics_mode "m"
#define c_sep ";"
#define c_high "h"
#define c_low "l"

#define c_alt_buffer     c_escape c_escape_kind "?1049"
#define c_alt_buffer_on  c_alt_buffer c_high
#define c_alt_buffer_off c_alt_buffer c_low
#define c_clear          c_escape c_escape_kind "2J"
#define c_cursor         c_escape c_escape_kind "?25"
#define c_cursor_on      c_cursor c_high
#define c_cursor_off     c_cursor c_low
#define c_move_format    c_escape c_escape_kind "%d" c_sep "%df"

#define c_foreground "38"
#define c_background "48"
#define c_default_foreground "39"
#define c_default_background "49"
#define c_true_colour "2"
#define c_no_true_colour "5"
#define c_rgb_format "%d" c_sep "%d" c_sep "%d"
#define c_colour(ground) ground c_sep c_true_colour c_sep c_rgb_format
#define c_colour_fg_format c_colour(c_foreground)
#define c_colour_bg_format c_colour(c_background)
#define c_colour_default_fg c_default_foreground
#define c_colour_default_bg c_default_background

#define c_attribute_format "%d"

#define c_default c_escape c_escape_kind "0" c_graphics_mode

enum TextAttribute
{
    A_NONE       = 0,
    A_BOLD       = 1,
    A_UNDERSCORE = 4,
    A_BLINK      = 5,
    A_REVERSE    = 7
};

struct
{
    char buffer[PRINT_BUFFER_SIZE];
    int buffer_len;
} write_buffer;

struct VTermSymbol
{
    struct Colour fg;
    struct Colour bg;
    TextAttributeFlags ta_flags;
    char symbol;
    bool redraw;
};

struct VTerm
{
    int width;
    int height;
    struct VTermSymbol* symbols;
    struct termios initial_state;
    bool cursor;
};

struct VTerm* vterm = NULL;
struct Colour c_default_colour = { -1, -1, -1 };

static void _writef(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    vsnprintf(write_buffer.buffer + write_buffer.buffer_len, PRINT_BUFFER_SIZE - write_buffer.buffer_len, format, args);
    write_buffer.buffer_len += strlen(write_buffer.buffer + write_buffer.buffer_len);

    va_end(args);
}

static void _flush(void)
{
    write(1, write_buffer.buffer, write_buffer.buffer_len);
    write_buffer.buffer_len = 0;
}

static inline struct VTermSymbol* _term_get_symbol(int x, int y)
{
    return &vterm->symbols[y * vterm->width + x];
}

static inline bool _should_redraw(struct VTermSymbol* symbol, char new_symbol, struct Colour* new_fg, struct Colour* new_bg, TextAttributeFlags new_ta_flags)
{
    if(symbol->symbol != new_symbol)
    {
        return true;
    }

    if(symbol->fg.r != new_fg->r || symbol->fg.g != new_fg->g || symbol->fg.b != new_fg->b)
    {
        return true;
    }

    if(symbol->bg.r != new_bg->r || symbol->bg.g != new_bg->g || symbol->bg.b != new_bg->b)
    {
        return true;
    }

    if(symbol->ta_flags != new_ta_flags)
    {
        return true;
    }

    return false;
}

void term_get_wh(int* w, int* h)
{
    if(w) *w = vterm->width;
    if(h) *h = vterm->height;
}

void term_clear(void)
{
    memset(vterm->symbols, 0, vterm->width * vterm->height * sizeof(struct VTermSymbol));
    _writef("%s", c_clear);
}

void term_clear_area(int x, int y, int w, int h)
{
    struct VTermSymbol* symbol;
    for(int _y = y; _y < (y + h); ++_y)
    for(int _x = x; _x < (x + w); ++_x)
    {
        symbol = _term_get_symbol(_x, _y);
        symbol->symbol = ' ';
        symbol->fg = c_default_colour;
        symbol->bg = c_default_colour;
        symbol->ta_flags = A_NONE;
        symbol->redraw = true;
    }
}

void term_set_cursor(bool on)
{
    vterm->cursor = on;

    if(on)
    {
        write(1, c_cursor_on, sizeof(c_cursor_on));
    }
    else
    {
        write(1, c_cursor_off, sizeof(c_cursor_off));
    }
}

void term_set_echo(bool state)
{
    struct termios t;
    t.c_lflag = state ? ECHO : ~ECHO;
    tcsetattr(1, TCSANOW, &t);
}

void term_set_canon(bool state)
{
    struct termios t;
    t.c_lflag = state ? ICANON : ~ICANON;
    tcsetattr(1, TCSANOW, &t);
}

void term_set_sigint_callback(void(*handler)(int))
{
    signal(SIGINT, handler);
}

void term_resize(void)
{
    struct winsize ws = { 0 };
    ioctl(1, TIOCGWINSZ, &ws);

    vterm->width  = ws.ws_col;
    vterm->height = ws.ws_row;

    struct VTermSymbol* new_sym = realloc(vterm->symbols, vterm->width * vterm->height * sizeof(struct VTermSymbol));
    if(!new_sym)
    {
        // Couldn't realloc the memory, which means something bad has happened.
        // Cannot really continue from this situation so just die.
        abort();
    }

    memset(new_sym, 0, vterm->width * vterm->height * sizeof(struct VTermSymbol));
    vterm->symbols = new_sym;
}

static void _write_attribute(enum TextAttribute attribute, bool semicolon)
{
    if(semicolon)
    {
        _writef(c_sep);
    }

    _writef(c_attribute_format, attribute);
}

static void _write_fg_colour(struct Colour* fg, bool semicolon)
{
    if(semicolon)
    {
        _writef(c_sep);
    }

    if(fg->r == -1)
    {
        _writef(c_default_foreground);
    }
    else
    {
        _writef(c_colour_fg_format, fg->r, fg->g, fg->b);
    }
}

static void _write_bg_colour(struct Colour* bg, bool semicolon)
{
    if(semicolon)
    {
        _writef(c_sep);
    }

    if(bg->r == -1)
    {
        _writef(c_default_background);
    }
    else
    {
        _writef(c_colour_bg_format, bg->r, bg->g, bg->b);
    }
}

void term_refresh(void)
{
    struct VTermSymbol* sym = NULL;
    int lx = -1;
    int ly = -1;

    for(int y = 0; y < vterm->height; ++y)
    for(int x = 0; x < vterm->width; ++x)
    {
        sym = _term_get_symbol(x, y);

        if(!sym->redraw)
        {
            continue;
        }

        // Move to
        if (x != lx + 1 || y != ly)
        {
            term_move_cursor(x, y);
        }

        lx = x;
        ly = y;

        // Start graphics mode
        bool semicolon = false;
        _writef(c_escape);
        _writef(c_escape_kind);

        // Set attributes
        //_writef(c_attribute_format, A_NONE);
        if(sym->ta_flags != A_NONE_BIT)
        {
            if(sym->ta_flags & A_BOLD_BIT)
            {
                _write_attribute(A_BOLD, semicolon);
                semicolon = true;
            }

            if(sym->ta_flags & A_UNDERSCORE_BIT)
            {
                _write_attribute(A_UNDERSCORE, semicolon);
                semicolon = true;
            }

            if(sym->ta_flags & A_BLINK_BIT)
            {
                _write_attribute(A_BLINK, semicolon);
                semicolon = true;
            }

            if(sym->ta_flags & A_REVERSE_BIT)
            {
                _write_attribute(A_REVERSE, semicolon);
                semicolon = true;
            }
        }
        else
        {
            _write_attribute(A_NONE, semicolon);
            semicolon = true;
        }

        // Set colours
        _write_fg_colour(&sym->fg, semicolon);
        _write_bg_colour(&sym->bg, semicolon);

        _writef(c_graphics_mode);

        // Write symbol
        _writef("%c", sym->symbol);
        _writef(c_default);

        sym->redraw = false;
    }

    // Reset term attributes
    _writef(c_default);
    _flush();
}

void term_getch(char* buf, int size)
{
    memset(buf, '\0', size);
    read(1, buf, size);
}

void term_wait_on_input(void)
{
    char c;
    read(1, &c, 1);
}

void term_move_cursor(int x, int y)
{
    _writef(c_move_format, y+1, x+1);
}

void term_set_attr(int x, int y, TextAttributeFlags ta_flags)
{
    struct VTermSymbol* sym = _term_get_symbol(x, y);

    if(!bit_flags_has_flags(sym->ta_flags, ta_flags))
    {
        bit_flags_set_flags(sym->ta_flags, ta_flags);
        sym->redraw = true;
    }
}

void term_unset_attr(int x, int y, TextAttributeFlags ta_flags)
{
    struct VTermSymbol* sym = _term_get_symbol(x, y);

    if(bit_flags_has_flags(sym->ta_flags, ta_flags))
    {
        bit_flags_unset_flags(sym->ta_flags, ta_flags);
        sym->redraw = true;
    }
}

void term_uninit(void)
{
    if(!vterm)
    {
        return;
    }

    term_clear();
    term_set_cursor(true);

    write(1, c_alt_buffer_off, sizeof(c_alt_buffer_off));
    tcsetattr(1, TCSANOW, &vterm->initial_state);

    free(vterm->symbols);
    free(vterm);

    vterm = NULL;
}

void term_init(void)
{
    if(vterm)
    {
        return;
    }

    // Take control of the terminal
    setvbuf(stdout, NULL, _IONBF, 0);
    write(1, c_alt_buffer_on, sizeof(c_alt_buffer_on));

    vterm = malloc(sizeof(struct VTerm));
    memset(vterm, 0, sizeof(struct VTerm));

    struct termios t;
    tcgetattr(1, &t);
    vterm->initial_state = t;

    t.c_lflag &= (~ECHO & ~ICANON);
    tcsetattr(1, TCSANOW, &t);

    term_resize();
    term_clear();
    term_set_cursor(false);
    write_buffer.buffer_len = 0;

    atexit(&term_uninit);
}

void term_draw_symbol(int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, char symbol)
{
    if(!fg) fg = &c_default_colour;
    if(!bg) bg = &c_default_colour;

    struct VTermSymbol* sym = _term_get_symbol(x, y);
    if(_should_redraw(sym, symbol, fg, bg, ta_flags))
    {
        sym->symbol = symbol;
        sym->fg = *fg;
        sym->bg = *bg;
        sym->ta_flags = ta_flags;
        sym->redraw = true;
    }
}

void term_draw_text(int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, const char* text)
{
    int length = strlen(text);
    for(int _x = x; (_x - x) < length; ++_x)
    {
        term_draw_symbol(_x, y, fg, bg, ta_flags, text[_x - x]);
    }
}

void term_draw_ftext(int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, const char* format, ...)
{
    va_list args;

    va_start(args, format);

    char tmp_buf[256];
    vsprintf(tmp_buf, format, args);
    term_draw_text(x, y, fg, bg, ta_flags, tmp_buf);

    va_end(args);
}

void term_draw_fntext(int count, int x, int y, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, const char* format, ...)
{
    va_list args;

    va_start(args, format);

    char tmp_buf[256];
    vsnprintf(tmp_buf, count, format, args);
    term_draw_text(x, y, fg, bg, ta_flags, tmp_buf);

    va_end(args);
}

void term_draw_area(int x, int y, int w, int h, struct Colour* fg, struct Colour* bg, TextAttributeFlags ta_flags, char symbol)
{
    for(int _y = y; _y < (y+h); ++_y)
    for(int _x = x; _x < (x+w); ++_x)
    {
        term_draw_symbol(_x, _y, fg, bg, ta_flags, symbol);
    }
}
