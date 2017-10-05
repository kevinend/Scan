#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdarg.h>  // varargs 

#include <string.h>
#include <ctype.h>

//
// NOTES: 
//  Just realized for signed integers i didn't read the minus sign (-), need to add this to the parser!
//

// required components are the preprocessor macros:
//  va_list
//  va_start
//  va_arg
//  va_end 


#define PARSE_NEXT_INTEGER(T)                                  \
T parse_next_##T( char *start, char *end, bool negative )       \
{                                                \
	uint64_t sum    = 0;                         \
	uint64_t addend = 0;                         \
	uint64_t scale  = 1;                         \
	while ( end >= start )                       \
	{                                            \
		addend = (*end - '0') * scale;           \
		sum += addend;                           \
                                                 \
		if ( (T)sum < sum )                      \
		{                                        \
			fprintf( stderr, "Integer overflow detected\n" ); \
			break;                               \
		}                                        \
                                                 \
		if ( (T)sum > sum )                      \
		{                                        \
			fprintf( stderr, "Integer overflow detected\n" );  \
		}                                        \
                                                 \
		--end;                                   \
		scale *= 10;                             \
    }                                            \
                                                 \
    if ( negative == true )                      \
    {                                            \
    	return sum * -1;                         \
    }                                            \
    else                                         \
    {                                            \
    	return sum;                              \
    }                                            \
}

PARSE_NEXT_INTEGER(int8_t)
PARSE_NEXT_INTEGER(int16_t)
PARSE_NEXT_INTEGER(int32_t)

//
// still not sure how to handle underflow/overflow for int64_t
//

int64_t
parse_next_int64_t(char *start, char *end)
{
	//
	// Incomplete: Need to figure out how to handle overflow/underflow
	//

	return -1;
}

PARSE_NEXT_INTEGER(uint8_t)
PARSE_NEXT_INTEGER(uint16_t)
PARSE_NEXT_INTEGER(uint32_t)

//
// still not sure how to handle underflow/overflow for uint64_t
// 

uint64_t
parse_next_uint64_t(char *start, char *end)
{
	//
	// INCOMPLETE: not sure how to handle the unsigned overflow at the moment
	//

	return 0;
}

//
// source and format string have to match character for character
// Ex. "1 2 3" and "%u32 %u32 %u32" will match since each value and each format specifier are each one space apart from another
// Ex. "1   2 3" and "%u32 %u32 %u32" will NOT match because of the additional spaces between the values 1 and 2
//

// i want to separate them into their own sections of code

// find next designator (%) for a format specifier
// advance source to match the number of characters read in the format string to find the designator

// read format specifier


int
scan(char *source, char *format_string, ...)
{
    #define MAX_FORMAT_SPECIFIER_SIZE 5

    va_list args;
    va_start(args, format_string);
    int successful_conversions = 0;

    while (*format_string != '\0')
    {
    	//
    	// find the format designator (%) in `format string`
    	// advance through both the `source` and `format_string` at the same time
    	//
        while (*format_string != '%')
        {
            ++format_string;
            ++source;
        }
        ++format_string;

		//
		// read the format specifier from `format_string` 
		//
        char format_specifier[MAX_FORMAT_SPECIFIER_SIZE + 1];

        int i = 0;
        while (*format_string != ' ' && i < MAX_FORMAT_SPECIFIER_SIZE && *format_string != '\0')
        {
            format_specifier[i] = *format_string;
            ++format_string;
            ++i;
        }
        format_specifier[i] = '\0';

		//
        // parse the `source` string into a substring starting at the same index as the start of the format specifier
        //
        char *substring_start;
        char *substring_end;

        substring_start = source;
        substring_end   = source;

        while (*source != *format_string)
        {
            ++source;
        }
        substring_end = source - 1; 

        //
        // `source` ends when it matches the same character as the format string
        // this is one past the substring containing the digits for a value therefore `substring_end` is `source` decremented by 1
        // also, note that the substring defined from `substring_start` to `substring_end` is NOT null terminated.
        // the parse functions doesn't expect it to be null terminated 
        //

		//
		// determine if the substring is signed/unsigned/float/string
		//	

		typedef enum 
		{
			UNKNOWN,
			UNSIGNED,
			SIGNED,
			FLOAT,
			STRING,
			INVALID

		} Format_Type;
	
		Format_Type format_specifier_type;
		format_specifier_type = UNKNOWN;

		//
		// handle the case where the format specifier is for an unsigned integer
		//

		char *valid_unsigned_specifiers[] = { "u8", "u16", "u32", "u64" };
		int   valid_unsigned_specifiers_count = (sizeof valid_unsigned_specifiers) / (sizeof valid_unsigned_specifiers[0]);

		for ( int i = 0; i < valid_unsigned_specifiers_count; ++i)
		{
			if ( strcmp( valid_unsigned_specifiers[i], format_specifier ) == 0 )
			{
				format_specifier_type = UNSIGNED;
				break;
			}
		}

		if ( format_specifier_type == UNSIGNED )
		{
			//
			// Make sure substring is a valid unsigned integer, no other characters besides digits and calculate value
			//

			char *current = substring_start;

			while ( current <= substring_end )
			{
				if ( !isdigit( *current ) )
				{
					fprintf( stderr, "Format specifier %s is for an unsigned integer, associated input is invalid", format_specifier );
					return -1;
				}
				++current;
			}

			if (strcmp(format_specifier, "u8") == 0)
        	{
            	*(va_arg(args, uint8_t *)) = parse_next_uint8_t( substring_start, substring_end, false );
        	}
        	else if (strcmp(format_specifier, "u16") == 0)
        	{
            	*(va_arg(args, uint16_t *)) = parse_next_uint16_t( substring_start, substring_end, false );
        	}
        	else if (strcmp(format_specifier, "u32") == 0)
        	{
            	*(va_arg(args, uint32_t *)) = parse_next_uint32_t( substring_start, substring_end, false );
        	}
        	else if (strcmp(format_specifier, "u64") == 0)
        	{
            	*(va_arg(args, uint64_t *)) = parse_next_uint64_t( substring_start, substring_end );
        	}
		}

		//
		// handle the case where the format specifier is for a signed integer
		//

		char *valid_signed_specifiers[] = { "i8", "i16", "i32", "i64" };
		int   valid_signed_specifiers_count = (sizeof valid_signed_specifiers) / (sizeof valid_signed_specifiers[0]);

		for ( int i = 0; i < valid_signed_specifiers_count; ++i )
		{
			if ( strcmp( valid_signed_specifiers[i], format_specifier ) == 0 )
			{
				format_specifier_type = SIGNED;
			}
		}

		if ( format_specifier_type == SIGNED )
		{
			//
			// Make sure substring is a valid signed integer, starts with minus sign, 
			// no other characters besides digits then calculate value
			//

			char *current = substring_start;
			bool negative = false;
			if ( *current == '-' )
			{
				negative = true;
				++substring_start;
				++current;
			}

			while ( current <= substring_end )
			{
				if ( !isdigit( *current ) )
				{
					fprintf( stderr, "Format specifier %s is for an signed integer, associated input is invalid", format_specifier );
					return -2;
				}
				++current;
			}

			if (strcmp(format_specifier, "i8") == 0 )
        	{
            	*(va_arg(args, int8_t *)) = parse_next_int8_t(substring_start, substring_end, negative );
        	}
        	else if (strcmp(format_specifier, "i16") == 0 )
        	{
            	*(va_arg(args, int16_t *)) = parse_next_int16_t( substring_start, substring_end, negative );
        	}
        	else if (strcmp(format_specifier, "i32") == 0  )
        	{
            	*(va_arg(args, int32_t *)) = parse_next_int32_t( substring_start, substring_end, negative );
        	}
        	else if (strcmp(format_specifier, "i64") == 0  )
        	{
            	*(va_arg(args, int64_t *)) = parse_next_int64_t( substring_start, substring_end );
        	}	
		}

#if 0
	// Do something similar in here!
		char *valid_float_specifiers[] = { };
		int   valid_float_specifiers_count = (sizeof valid_float_specifiers) / (sizeof valid_float_specifiers[0]);
	
		char *valid_string_specifiers[] = { };
		int   valid_string_specifiers_count = (sizeof valid_string_specifiers) / (sizeof valid_string_specifiers[0]);
#endif

        ++successful_conversions;
    }

    va_end(args);

    return successful_conversions;
}


int
main(int argc, char **argv)
{

// uint8_t test
#if 0

	uint8_t v1;
	uint8_t v2;
	uint8_t v3;

	scan( "1 128 45", "%u8 %u8 %u8", &v1, &v2, &v3 );
	fprintf( stdout, "%u %u %u\n", v1, v2, v3 );

#endif

// uint16_t test
#if 0
	uint16_t v1;
	uint16_t v2;
	uint16_t v3;

	scan( "1 2 3", "%u16 %u16 %u16", &v1, &v2, &v3 );
	fprintf( stdout, "%u %u %u\n", v1, v2, v3 );

#endif

// uint32_t test
#if 0

	uint32_t v1;
	uint32_t v2;
	uint32_t v3;

	scan( "1 2 3", "%u32 %u32 %u32", &v1, &v2, &v3 );
	fprintf( stdout, "%u %u %u\n", v1, v2, v3 );

#endif

// uint64_t test
#if 0

	uint64_t v1;
	uint64_t v2;
	uint64_t v3;

	scan( "1 2 3", "%u64 %u64 %u64", &v1, &v2, &v3 );
	fprintf( stdout, "%llu %llu %llu\n", v1, v2, v3 );

#endif

// int8_t test
#if 0

	int8_t v1;
	int8_t v2;
	int8_t v3;

	scan( "-1 -2 -3", "%i8 %i8 %i8", &v1, &v2, &v3 );
	fprintf( stdout, "%d %d %d\n", v1, v2, v3 );

#endif

// int16_t test
#if 1

	int16_t v1;
	int16_t v2;
	int16_t v3;

	scan( "-1545 200 -361", "%i16 %i16 %i16", &v1, &v2, &v3 );
	fprintf( stdout, "%d %d %d\n", v1, v2, v3 );

#endif

// int32_t test
#if 0

	int32_t v1;
	int32_t v2;
	int32_t v3;

	scan( "1 2 3", "%i32 %i32 %i32", &v1, &v2, &v3 );
	fprintf( stdout, "%d %d %d\n", v1, v2, v3 );

#endif

// int64_t test
#if 0

	int64_t v1;
	int64_t v2;
	int64_t v3;

	scan( "1 2 3", "%i64 %i64 %i64", &v1, &v2, &v3 );
	fprintf( stdout, "%lld %lld %lld\n", v1, v2, v3 );

#endif


    return 0;
}

