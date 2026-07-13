/*
* Portions of this file are derived from RADDebugger (https://github.com/EpicGamesExt/raddebugger),
* Copyright (c) Epic Games Tools.
* Licensed under the MIT license.
*/
#ifndef BASE_STRING_H
#define BASE_STRING_H

typedef struct String8 String8;
struct String8
{
    U64 size;
    U8 *str;
};

#define String8Literal(cstring) ((String8){sizeof(cstring) - 1, (U8*)(cstring)})

internal U32        string8_match(String8 a, String8 b);
internal String8    string8_from_cstring(char *cstring);
internal U64        length_of_cstring(char *cstring);

internal U32        char_is_space(U8 c);
internal U32        char_is_digit(U8 c, U32 base);
internal U32        char_is_upper(U8 c);
internal U32        char_is_lower(U8 c);
internal U32        char_is_alpha(U8 c);
internal U32        char_is_slash(U8 c);
internal U8         lower_from_char(U8 c);
internal U8         upper_from_char(U8 c);

internal U32        string8_has_base10_digits_only(String8 string);
internal U32        string8_starts_with(String8 string, String8 prefix);

// String to Integer Conversion
internal U64        u64_from_string8(String8 string, U32 radix);

////////////////////////////////
// String8 Format Builder

typedef struct FormatOptions FormatOptions;
struct FormatOptions
{
    U8  padding_char;
    S32 width;
    U8  left_align;
    S32 precision;
};

typedef void (*String8FormatFunctionHandler)(Arena *arena, va_list args, FormatOptions *options);

internal FormatOptions      format_options_from_format(String8 format, U64 *index);
internal String8            string8_format_builder(Arena *arena, String8 format, va_list args);
internal String8            push_string8_format(Arena *arena, String8 format, ...);
internal void               write_string8_format(int fd, String8 format, ...);
#define Print(format, ...)  write_string8_format(STDOUT_FILENO, String8Literal(format), ##__VA_ARGS__)

// conversion
internal void             apply_padding(Arena *arena, String8 s, FormatOptions *opt);
internal void             handle_int(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_u64(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_hex_common(Arena *arena, va_list args, FormatOptions *opt, U8 table[16]);
internal void             handle_hex(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_upper_hex(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_ptr(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_float(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_string8(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_cstring(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_char(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_percent_literal(Arena *arena, va_list args, FormatOptions *opt);
internal void             handle_hex_ptr(Arena *arena, va_list args, FormatOptions *opt, U8 table[16]);
internal S32              my_stoa_buf(U8 *buf, S32 x);
internal U32              my_u64toa_buf(U8 *buf, U64 x);
internal U8               my_ftoa_buf(U8 *buf, F64 val, FormatOptions *opt);

#endif // BASE_STRING_H
