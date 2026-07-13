/*
* Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
* Copyright (c) Epic Games Tools.
* Licensed under the MIT license.
*/
internal U32 string8_match(String8 a, String8 b)
{
    U32 result = 0;
    if (a.size == b.size)
    {
        if (MemoryMatch(a.str, b.str, b.size))
        {
            result = 1;
        }
    }
    return result;
}

internal U64 length_of_cstring(char *cstring)
{
    char *ptr = cstring;
    while(*ptr != '\0')
    {
        ptr += 1;
    }

    U64 length = (U64)(ptr - cstring);
    return(length);
}

internal String8 string8_from_cstring(char *cstring)
{
    U64 size = length_of_cstring(cstring);
    return (String8){.str = (U8 *)cstring, .size = size};
}

internal U64 u64_from_string8(String8 string, U32 radix)
{
    U64 x = 0;
    if(1 < radix && radix <= 16)
    {
        for(U64 index = 0; index < string.size; index += 1)
        {
            x *= radix;
            x += integer_symbol_reverse[string.str[index] & 0x7F];
        }
    }
    return(x);
}

internal U32 string8_starts_with(String8 string, String8 prefix)
{
    U32 result = 0;

    if(string.size >= prefix.size)
    {
        if(MemoryMatch(string.str, prefix.str, prefix.size))
        {
            result = 1;
        }
    }

    return(result);
}

internal U32 char_is_digit(U8 c, U32 base)
{
    U32 result = 0;
    if(0 < base && base <= 16)
    {
        U8 val = integer_symbol_reverse[c];
        if(val < base)
        {
            result = 1;
        }
    }
    return(result);
}

internal U32 string8_has_base10_digits_only(String8 string)
{
    U32 result = 1;

    for(U32 index = 0; index < string.size; index += 1)
    {
        if(char_is_digit(string.str[index], 10) == 0)
        {
            result = 0;
            break;
        }
    }

    return(result);
}

internal U32 char_is_space(U8 c)
{
    return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

internal U32 char_is_upper(U8 c)
{
  return ('A' <= c && c <= 'Z');
}

internal U32 char_is_lower(U8 c)
{
    return('a' <= c && c <= 'z');
}

internal U32 char_is_alpha(U8 c)
{
    return(char_is_upper(c) || char_is_lower(c));
}

internal U32 char_is_slash(U8 c)
{
    return(c == '/' || c == '\\');
}

internal U8 lower_from_char(U8 c)
{
    if(char_is_upper(c))
    {
        c += ('a' - 'A');
    }
    return(c);
}

internal U8 upper_from_char(U8 c)
{
    if(char_is_lower(c))
    {
        c += ('A' - 'a');
    }
    return(c);
}

////////////////////////////////
// String8 Format Builder

internal void write_string8_format(int fd, String8 format, ...)
{
    TemporaryArena scratch = ScratchArenaBegin(0);
    va_list arguments;
    va_start(arguments, format);
    String8 result = string8_format_builder(scratch.arena, format, arguments);
    write(fd, result.str, result.size);
    va_end(arguments);
    ScratchArenaEnd(scratch);
}

internal String8 push_string8_format(Arena *arena, String8 format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    String8 result = string8_format_builder(arena, format, arguments);
    va_end(arguments);
    return result;
}

global String8FormatFunctionHandler function_handler_table[256] =
{
    ['d'] = handle_int,
    ['i'] = handle_int,
    ['u'] = handle_u64,
    ['s'] = handle_cstring,
    ['S'] = handle_string8,
    ['c'] = handle_char,
    ['x'] = handle_hex,
    ['X'] = handle_upper_hex,
    ['p'] = handle_ptr,
    ['f'] = handle_float,
    ['%'] = handle_percent_literal,
};

internal FormatOptions format_options_from_format(String8 format, U64 *index)
{
    FormatOptions format_options =
    {
        .padding_char = ' ',
    };
    U8 *f = format.str;
    U64 p = *index;

    while (p < format.size && (f[p] == '-' || f [p]== '0'))
    {
        if (f[p] == '-')
        {
            format_options.left_align = 1;
        }
        else if (f[p] == '0')
        {
            format_options.padding_char = '0';
        }
        p += 1;
    }

    while (p < format.size && f[p] >= '0' && f[p] <= '9')
    {
        format_options.width = (format_options.width * 10) + f[p] - '0';
        p += 1;
    }

    if (p < format.size && f[p] == '.')
    {
        p += 1;
        while (p < format.size && f[p] >= '0' && f[p] <= '9')
        {
            format_options.precision = (format_options.precision * 10) + f[p] - '0';
            p += 1;
        }
    }

    *index = p;

    return format_options;
}

internal String8 string8_format_builder(Arena *arena, String8 format, va_list args)
{
    // NOTE:  Everything allocated onto the arena from this point up to the string size
    //        is going to be included in the returned string!
    //        Also, the new U8's pushed onto the arena are not aligned. Which allows for chaining strings together.
    U8 *start = push_array_aligned(arena, U8, 0, 1);

    for(U64 index = 0; index < format.size; index += 1)
    {
        if(format.str[index] == '%')
        {
            index += 1;
            FormatOptions format_options = format_options_from_format(format, &index);

            String8FormatFunctionHandler handler = function_handler_table[format.str[index]];
            if(handler)
            {
                handler(arena, args, &format_options);
            }
            else if(index < format.size)
            {
                U8 *c = push_array_aligned(arena, U8, 1, 1);
                *c    = format.str[index];
            }
        }
        else
        {
            U8 *c = push_array_aligned(arena, U8, 1, 1);
            *c    = format.str[index];
        }
    }

    U8 *end  = push_array_aligned(arena, U8, 0, 1);
    U64 size = (U64)(end - start);
    String8 result = { .str = start, .size = size};

    return(result);
}

internal void handle_char(Arena *arena, va_list args, FormatOptions *opt)
{
    U8 c = (U8)va_arg(args, S32);

    String8 s;
    s.str = &c;
    s.size = 1;

    apply_padding(arena, s, opt);
}

internal void handle_cstring(Arena *arena, va_list args, FormatOptions *opt)
{
    char* cstring = va_arg(args, char*);
    apply_padding(arena, (String8){.str = (U8 *)cstring, .size = length_of_cstring(cstring)}, opt);
}

internal U8 my_ftoa_buf(U8 *buffer, F64 val, FormatOptions *opt)
{
    U8 i = 0;
    if (val < 0) { buffer[i++] = '-'; val = -val; }

    S32 precision = (opt->precision >= 0) ? opt->precision : 6;

    F64 rounding = 0.5;
    for (S32 p = 0; p < precision; ++p) rounding /= 10.0;
    val += rounding;

    U64 whole = (U64)val;
    F64 fraction = val - (F64)whole;
    i += my_u64toa_buf(buffer + i, whole);

    if (precision > 0) {
        buffer[i++] = '.';
        while (precision--) {
            fraction *= 10.0;
            U32 digit = (U32)fraction;
            buffer[i++] = (U8)digit + '0';
            fraction -= (F64)digit;
        }
    }
    return i;
}

internal void handle_float(Arena *arena, va_list args, FormatOptions *opt)
{
	F64 val = va_arg(args, F64);
	U8 buf[32] = {0};
	U8 len = my_ftoa_buf(buf, val, opt);
	String8 s = (String8){.str = buf, .size = len};
	apply_padding(arena, s, opt);
}

internal void handle_hex_common(Arena *arena, va_list args, FormatOptions *opt, U8 table[16])
{
    U64 val = va_arg(args, U64);

    U8 temp[32];
    U8 result[32];
    U32 len = 0;

    if (val == 0)
    {
        temp[len++] = '0';
    }
    else
    {
        while (val)
        {
            temp[len++] = table[val & 0xF];
            val >>= 4;
        }
    }

    U32 i = 0;
    while (i < len)
    {
        result[i] = temp[len - 1 - i];
        i += 1;
    }

    String8 s;
    s.str = result;
    s.size = len;

    apply_padding(arena, s, opt);
}

internal void handle_upper_hex(Arena *arena, va_list args, FormatOptions *opt)
{
    handle_hex_common(arena, args, opt, (U8*)"0123456789ABCDEF");
}

internal void handle_hex(Arena *arena, va_list args, FormatOptions *opt)
{
    handle_hex_common(arena, args, opt, (U8*)"0123456789abcdef");
}

internal S32 my_stoa_buf(U8 *buf, S32 x)
{
    S32 i = 0;
    S64 nb = x;

    if (nb == 0)
    {
        buf[0] = '0';
        return 1;
    }

    if (nb < 0) nb = -nb;

    U8 temp[32];
    S32 temp_len = 0;
    while (nb)
    {
        temp[temp_len++] = (nb % 10) + '0';
        nb /= 10;
    }

    if (x < 0) buf[i++] = '-';

    while (temp_len > 0)
    {
        buf[i++] = temp[--temp_len];
    }

    return i;
}

internal void handle_int(Arena *arena, va_list args, FormatOptions *opt)
{
    S32 val = va_arg(args, S32);
    U8 buffer[32] = {0};

    S32 len = my_stoa_buf(buffer, val);

    String8 s;
    s.str = buffer;
    s.size = len;

    apply_padding(arena, s, opt);
}

internal void handle_percent_literal(Arena *arena, va_list args, FormatOptions *opt)
{
    (void)args;

    U8 c = '%';
    String8 s;
    s.str = &c;
    s.size = 1;

    apply_padding(arena, s, opt);
}

internal void handle_hex_ptr(Arena *arena, va_list args, FormatOptions *opt, U8 table[16])
{
    void *ptr = va_arg(args, void *);
    U64 val = (U64)ptr;

    U8 buffer[32];
    U64 len = 0;

    if (ptr == NULL)
    {
        U8 *nil_str = (U8 *)"(nil)";
        while (*nil_str)
        {
            buffer[len++] = *nil_str++;
        }
    }
    else
    {
        buffer[len++] = '0';
        buffer[len++] = 'x';

        U8 temp[20];
        S32 temp_idx = 0;

        while (val)
        {
            temp[temp_idx++] = table[val & 0xF];
            val >>= 4;
        }

        while (temp_idx > 0)
        {
            buffer[len++] = temp[--temp_idx];
        }
    }

    String8 s;
    s.str = buffer;
    s.size = len;

    apply_padding(arena, s, opt);
}

internal void handle_ptr(Arena *arena, va_list args, FormatOptions *opt)
{
    handle_hex_ptr(arena, args, opt, (U8 *)"0123456789abcdef");
}

internal void handle_string8(Arena *arena, va_list args, FormatOptions *opt)
{
    String8 s = va_arg(args, String8);
    apply_padding(arena, s, opt);
}

internal U32 my_u64toa_buf(U8 *buf, U64 x)
{
    U32 i = 0;

    if (x == 0)
    {
        buf[0] = '0';
        return 1;
    }

    U8 temp[24];
    U32 temp_len = 0;

    while (x > 0)
    {
        temp[temp_len++] = (x % 10) + '0';
        x /= 10;
    }

    while (temp_len > 0)
    {
        buf[i++] = temp[--temp_len];
    }

    return i;
}

internal void handle_u64(Arena *arena, va_list args, FormatOptions *opt)
{
    U64 val = va_arg(args, U64);
    U8 buffer[32];

    U32 len = my_u64toa_buf(buffer, val);

    String8 s = { .str = buffer, .size = len };
    apply_padding(arena, s, opt);
}

internal void apply_padding(Arena *arena, String8 s, FormatOptions *opt)
{
    if (s.size >= (U64)opt->width)
    {
        U8 *result = push_array_aligned(arena, U8, s.size, 1);
        MemoryCopyString8(result, s);
    }
    else
    {
        S32 total_padding = opt->width - s.size;

        U8 *padded = push_array_aligned(arena, U8, opt->width, 1);
        if (opt->left_align)
        {
            /* <TEXT;SPACES> */
            MemoryCopyString8(padded, s);
            MemorySet(padded + s.size, ' ', total_padding);
        }
        else // right align
        {
            /* <SPACES/ZEROS;TEXT> */
            MemorySet(padded, opt->padding_char, total_padding);
            MemoryCopyString8(padded + total_padding, s);
        }
    }
}
