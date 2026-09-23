/*
* Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
* Copyright (c) Epic Games Tools.
* Licensed under the MIT license.
*/
internal_function
U32 string8_match(String8 a, String8 b)
{
    U32 result = 0;
    if(a.size == b.size)
    {
        if(MemoryMatch(a.str, b.str, b.size))
        {
            result = 1;
        }
    }
    return(result);
}

internal_function
U64 length_of_cstring(char *cstring)
{
    char *ptr = cstring;
    while(*ptr != '\0')
    {
        ptr += 1;
    }
    U64 length = (U64)(ptr - cstring);
    return(length);
}

internal_function
String8 string8_from_cstring(char *cstring)
{
    U64 size = length_of_cstring(cstring);
    return( (String8){ .str = (U8 *)cstring, .size = size } );
}

internal_function
U64 u64_from_string8(String8 string, U32 radix)
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

internal_function
U32 string8_starts_with(String8 string, String8 prefix)
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

internal_function
U32 char_is_digit(U8 c, U32 base)
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

internal_function
U32 string8_has_base10_digits_only(String8 string)
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

internal_function
U32 char_is_space(U8 c)
{
    return(c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

internal_function
U32 char_is_upper(U8 c)
{
    return('A' <= c && c <= 'Z');
}

internal_function
U32 char_is_lower(U8 c)
{
    return('a' <= c && c <= 'z');
}

internal_function
U32 char_is_alpha(U8 c)
{
    return(char_is_upper(c) || char_is_lower(c));
}

internal_function
U32 char_is_slash(U8 c)
{
    return(c == '/' || c == '\\');
}

internal_function
U8 lower_from_char(U8 c)
{
    if(char_is_upper(c))
    {
        c += ('a' - 'A');
    }
    return(c);
}

internal_function
U8 upper_from_char(U8 c)
{
    if(char_is_lower(c))
    {
        c += ('A' - 'a');
    }
    return(c);
}

internal_function
String8 lower_from_string8(Arena *arena, String8 source)
{
    String8 result;

    result.size = source.size;
    result.str  = push_array_no_zero(arena, U8, result.size);
    for(U32 char_index = 0; char_index < result.size; char_index += 1)
    {
        result.str[char_index] = lower_from_char(source.str[char_index]);
    }
    return(result);
}

internal_function
String8 upper_from_string8(Arena *arena, String8 source)
{
    String8 result;

    result.size = source.size;
    result.str  = push_array_no_zero(arena, U8, result.size);
    for(U32 char_index = 0; char_index < result.size; char_index += 1)
    {
        result.str[char_index] = upper_from_char(source.str[char_index]);
    }
    return(result);
}

////////////////////////////////
// String8 Format Builder

internal_function
void write_string8_format(int fd, String8 format, ...)
{
    TemporaryArena scratch = ScratchArenaBegin(0);
    va_list arguments;
    va_start(arguments, format);
    String8 result = string8_format_builder(scratch.arena, format, arguments);
    write(fd, result.str, result.size);
    va_end(arguments);
    ScratchArenaEnd(scratch);
}

internal_function
String8 push_string8_format(Arena *arena, String8 format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    String8 result = string8_format_builder(arena, format, arguments);
    va_end(arguments);
    return(result);
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

internal_function
FormatOptions format_options_from_format(String8 format, U64 *index)
{
    FormatOptions format_options =
    {
        .padding_char = ' ',
    };
    U8 *f = format.str;
    U64 p = *index;

    while(p < format.size && (f[p] == '-' || f[p]== '0'))
    {
        if(f[p] == '-')
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
    return(format_options);
}

internal_function
String8 string8_format_builder(Arena *arena, String8 format, va_list args)
{
    // NOTE: Everything pushed on the arena from this moment is going to be included in the returned string!
    // Also, the new characters pushed on the arena are not aligned. Which allows for chaining strings together.
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

internal_function
void handle_char(Arena *arena, va_list args, FormatOptions *opt)
{
    U8 c = (U8)va_arg(args, S32);
    String8 s =
    {
        .str = &c,
        .size = 1,
    };
    apply_padding(arena, s, opt);
}

internal_function
void handle_cstring(Arena *arena, va_list args, FormatOptions *opt)
{
    char* cstring = va_arg(args, char*);
    String8 s =
    {
        .str = (U8 *)cstring,
        .size = length_of_cstring(cstring),
    };
    apply_padding(arena, s, opt);
}

internal_function
void handle_float(Arena *arena, va_list args, FormatOptions *opt)
{
    F64 value = va_arg(args, F64);
    U32 precision = (opt->precision >= 0) ? (U32)opt->precision : 6;
    char buffer[512];
    int length = d2fixed_buffered_n(value, precision, buffer);
    String8 s =
    {
        .str = (U8*)buffer,
        .size = (U64)length,
    };
    apply_padding(arena, s, opt);
}

internal_function
void handle_hex_common(Arena *arena, va_list args, FormatOptions *opt, U8 table[16])
{
    U64 value = va_arg(args, U64);

    U8 temp[32];
    U8 result[32];
    U32 length = 0;

    if(value == 0)
    {
        temp[length] = '0';
        length += 1;
    }
    else
    {
        while(value)
        {
            temp[length] = table[value & 0xF];
            length += 1;
            value >>= 4;
        }
    }

    for(U32 i = 0; i < length; i += 1)
    {
        result[i] = temp[length - 1 - i];
    }

    String8 s =
    {
        .str = result,
        .size = length,
    };
    apply_padding(arena, s, opt);
}

internal_function
void handle_upper_hex(Arena *arena, va_list args, FormatOptions *opt)
{
    handle_hex_common(arena, args, opt, (U8*)"0123456789ABCDEF");
}

internal_function
void handle_hex(Arena *arena, va_list args, FormatOptions *opt)
{
    handle_hex_common(arena, args, opt, (U8*)"0123456789abcdef");
}

internal_function
S32 signed_to_ascii(U8 *buffer, S32 x)
{
    S64 value = x;
    S32 length = 0;
    if(value == 0)
    {
        buffer[0] = '0';
        length = 1;
        return(length);
    }
    if(value < 0)
    {
        value = -value;
    }

    U8 temp[32];
    S32 temp_length = 0;
    while(value)
    {
        temp[temp_length] = (value % 10) + '0';
        temp_length += 1;
        value /= 10;
    }

    if(x < 0)
    {
        buffer[length] = '-';
        length += 1;
    }

    while(temp_length > 0)
    {
        temp_length -= 1;
        buffer[length] = temp[temp_length];
        length =+ 1;
    }
    return(length);
}

internal_function
void handle_int(Arena *arena, va_list args, FormatOptions *opt)
{
    S32 value = va_arg(args, S32);
    U8 buffer[32] = {0};

    S32 length = signed_to_ascii(buffer, value);

    String8 s =
    {
        .str = buffer,
        .size = length,
    };
    apply_padding(arena, s, opt);
}

internal_function
void handle_percent_literal(Arena *arena, va_list args, FormatOptions *opt)
{
    (void)args;

    U8 c = '%';
    String8 s =
    {
        .str = &c,
        .size = 1,
    };
    apply_padding(arena, s, opt);
}

internal_function
void handle_hex_ptr(Arena *arena, va_list args, FormatOptions *opt, U8 table[16])
{
    void *ptr = va_arg(args, void *);
    U64 value = (U64)ptr;

    U8 buffer[32];
    U64 length = 0;
    String8 s;

    if(ptr == 0)
    {
        s = String8Literal("(nil)");
    }
    else
    {
        buffer[0] = '0';
        buffer[1] = 'x';
        length += 2;

        U8 temp[20];
        S32 temp_index = 0;

        while(value)
        {
            temp[temp_index] = table[value & 0xF];
            temp_index += 1;
            value >>= 4;
        }

        while(temp_index > 0)
        {
            temp_index -= 1;
            buffer[length] = temp[temp_index];
            length += 1;
        }

        s.str = buffer;
        s.size = length;
    }
    apply_padding(arena, s, opt);
}

internal_function
void handle_ptr(Arena *arena, va_list args, FormatOptions *opt)
{
    handle_hex_ptr(arena, args, opt, (U8 *)"0123456789abcdef");
}

internal_function
void handle_string8(Arena *arena, va_list args, FormatOptions *opt)
{
    String8 s = va_arg(args, String8);
    apply_padding(arena, s, opt);
}

internal_function
U32 unsigned_to_ascii(U8 *buffer, U64 x)
{
    U32 length = 0;

    if(x == 0)
    {
        buffer[0] = '0';
        length = 1;
        return(length);
    }

    U8 temp[24];
    U32 temp_length = 0;

    while(x > 0)
    {
        temp[temp_length] = (x % 10) + '0';
        temp_length += 1;
        x /= 10;
    }

    while(temp_length > 0)
    {
        temp_length -= 1;
        buffer[length] = temp[temp_length];
        length += 1;
    }
    return(length);
}

internal_function
void handle_u64(Arena *arena, va_list args, FormatOptions *opt)
{
    U64 value = va_arg(args, U64);
    U8 buffer[32];

    U32 length = unsigned_to_ascii(buffer, value);
    String8 s =
    {
        .str = buffer,
        .size = length,
    };
    apply_padding(arena, s, opt);
}

internal_function
void apply_padding(Arena *arena, String8 s, FormatOptions *opt)
{
    if(s.size >= (U64)opt->width)
    {
        U8 *result = push_array_aligned(arena, U8, s.size, 1);
        MemoryCopyString8(result, s);
    }
    else
    {
        S32 total_padding = opt->width - s.size;
        U8 *padded = push_array_aligned(arena, U8, opt->width, 1);
        if(opt->left_align)
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
