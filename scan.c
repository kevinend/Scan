#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <stdarg.h>  // varargs 

#include <string.h>
#include <ctype.h>


// so the only problems left are for
// handling overflow/underflow for the following:
//		parse_next_u64 and parse_next_s64

// required components are the preprocessor macros:
//  va_list
//  va_start
//  va_arg
//  va_end 

// could do something clever here with the value
// you could figure the smallest value that it could take without underflow then only test in the larger portion
// might be better for huge numbers 

// checking on each iteration of the loop, maybe we should change this to the minimum value 
// NOTES: OPTIMIZATION!!!!

int8_t
parse_next_int8_t(char *start, char *end)
{
	int16_t sum = 0;
	int16_t addend = 0;
	int16_t scale = 1;
	int8_t  value = 0;

	while (end >= start)
	{
		addend = (*end - '0') * scale;
		sum += addend;

		value = (int8_t)sum;
		if (value > sum)
		{
			fprintf(stderr, "Integer underflow detected\n");
			break;
		}
		if (value < sum)
		{
			fprintf(stderr, "Integer overflow detected\n");
			break;
		}

		--end;
		scale *= 10;
	}

	return value;

}

int16_t
parse_next_int16_t(char *start, char *end)
{
	int32_t sum = 0;
	int32_t addend = 0;
	int32_t scale = 1;
	int32_t value = 0;

	while (end >= start)
	{
		addend = (*end - '0') * scale;
		sum += addend;

		value = (int16_t)sum;
		if (value > sum)
		{
			fprintf(stderr, "Integer underflow detected\n");
			break;
		}
		if (value < sum)
		{
			fprintf(stderr, "Integer overflow detected\n");
			break;
		}

		--end;
		scale *= 10;
	}

	return value;
}

int32_t
parse_next_int32_t(char *start, char *end)
{
	int64_t sum = 0;
	int64_t addend = 0;
	int64_t scale = 1;
	int64_t value = 0;

	while (end >= start)
	{
		addend = (*end - '0') * scale;
		sum += addend;
		
		if (value > sum)
		{
			fprintf(stderr, "Integer overflow detected\n");
			break;
		}
		if (value < sum)
		{
			fprintf(stderr, "Integer underflow detected\n");
			break;
		}
		
		--end;
		scale *= 10;
	}

	return value;
}

//
// NOTE: This strategy for underflow or overflow detection won't work 
// BULLETPROOF!!!!
//

int64_t
parse_next_int64_t(char *start, char *end)
{
	// DON'T USE!!!
}

uint8_t
parse_next_uint8_t(char *start, char *end)
{
	uint16_t sum = 0;
	uint16_t addend = 0;
	uint16_t scale = 1;
	uint8_t value = 0;

	while (end >= start)
	{
		addend = (*end - '0') * scale;
		sum += addend;

		if (value < sum)
		{
			fprintf(stderr, "Integer underflow detected\n");
			break;
		}

		--end;
		scale *= 10;
	}

	return value;

}

uint16_t
parse_next_uint16_t(char *start, char *end)
{
	uint32_t sum = 0;
	uint32_t addend = 0;
	uint32_t scale = 1;
	uint16_t value = 0;

	while (end >= start)
	{
		addend = (*end - '0') * scale;
		sum += addend;

		if (value < sum)
		{
			fprintf(stderr, "Integer underflow detected\n");
			break;
		}

		--end;
		scale *= 10;
	}

	return value;
}

uint32_t
parse_next_uint32_t(char *start, char *end)
{
	uint64_t sum = 0;
	uint64_t addend = 0;
	uint64_t scale = 1;
	uint32_t value = 0;

	while (end >= start)
	{
		addend = (*end - '0') * scale;
		sum += addend;

		if (value < sum)
		{
			fprintf(stderr, "Integer underflow detected\n");
			break;
		}

		--end;
		scale *= 10;
	}

	return value;
}

//
// Need to handle this case, can't use the downcasting method to check for overflow/underflow
// BULLETPROOF this
//

uint64_t
parse_next_uint64_t(char *start, char *end)
{
	// DON'T use!
	return 1; 
}


int
scan(char *source, char *format_string, ...)
{
#define MAX_FORMAT_SPECIFIER_SIZE 5
	// don't need the static trick here, because we parse the string in a single pass
	// walk the format string looking for the first format specifier
	// once we find it we dispatch to the function call we are looking for
	// do this while the format string isn't empty

	va_list args;
	va_start(args, format_string);
	int successful_conversions = 0;

	while (*format_string != '\0')
	{
		while (*format_string != '%')
		{
			++format_string;
			++source;
		}
		++format_string;

		char format_specifier[MAX_FORMAT_SPECIFIER_SIZE + 1];

		int i = 0;
		while (*format_string != ' ' && i < MAX_FORMAT_SPECIFIER_SIZE && *format_string != '\0')
		{
			format_specifier[i] = *format_string;
			++format_string;
			++i;
		}
		format_specifier[i] = '\0';

		fprintf(stdout, "Format specifier requested == %s\n", &format_specifier[0]);

		char *accepted_format_specifiers[] =
		{
			"i8", "i16", "i32", "i64",
			"u8", "u16", "u32", "u64",
		};

		int accepted_format_specifiers_count = (sizeof accepted_format_specifiers) / (sizeof accepted_format_specifiers[0]);

		bool format_specifier_found = false;
		for (int i = 0; i < accepted_format_specifiers_count; ++i)
		{
			if (strcmp(accepted_format_specifiers[i], format_specifier) == 0)
			{
				format_specifier_found = true;
				break;
			}
		}

		if (format_specifier_found == false)
		{
			fprintf(stderr, "Unrecognized format specifier '%s', unable to complete formatted scan\n", format_specifier);
			return -1;
		}

		// parse the actual string we were passed in
		char *substring_start = source;
		char *substring_end = substring_start;

		while (*substring_end != *format_string)
		{
			++substring_end;
		}
		--substring_end;

		//
		// `substring_end` is one past the end of the substring so we decrement by 1
		// also, note that the substring defined from `substring_start` to `substring_end` is NOT null terminated.
		// the parse functions doesn't expect it to be null terminated 
		//


		//  parse the actual string we were passed in
		if (strcmp(format_specifier, "i8") == 0)
		{
			*(va_arg(args, int8_t *)) = parse_next_int8_t(substring_start, substring_end);
		}
		else if (strcmp(format_specifier, "i16") == 0)
		{
			*(va_arg(args, int16_t *)) = parse_next_int16_t(substring_start, substring_end);
		}
		else if (strcmp(format_specifier, "i32") == 0)
		{
			*(va_arg(args, int32_t *)) = parse_next_int32_t(substring_start, substring_end);
		}
		else if (strcmp(format_specifier, "i64") == 0)
		{
			*(va_arg(args, int64_t *)) = parse_next_int64_t(substring_start, substring_end);
		}
		else if (strcmp(format_specifier, "u8") == 0)
		{
			*(va_arg(args, uint8_t *)) = parse_next_uint8_t(substring_start, substring_end);
		}
		else if (strcmp(format_specifier, "u16") == 0)
		{
			*(va_arg(args, uint16_t *)) = parse_next_uint16_t(substring_start, substring_end);
		}
		else if (strcmp(format_specifier, "u32") == 0)
		{
			*(va_arg(args, uint32_t *)) = parse_next_uint32_t(substring_start, substring_end);
		}
		else if (strcmp(format_specifier, "u64") == 0)
		{
			*(va_arg(args, uint64_t *)) = parse_next_uint64_t(substring_start, substring_end);
		}

		++successful_conversions;
	}

	va_end(args);

	return successful_conversions;
}

// 
// These were test functions for varargs...
// 
#if 0

int
sum(int count, ...)
{
	va_list args;
	int sum = 0;

	va_start(args, count); // the list starts at the offset of arg1
	for (int i = 0; i < count; ++i)
	{
		sum += va_arg(args, int32_t); // requires the type to cast to
	}
	va_end(args);

	return sum;
}

void
function1(int count, ...)
{
	va_list args;
	va_start(args, count);

	for (int i = 0; i < count; ++i)
	{
		*(va_arg(args, int32_t *)) = 10;
	}
	va_end(args);
	return;
}

void
function2(int count, ...)
{
	va_list args;
	va_start(args, count);

	*(va_arg(args, int32_t *)) = 100;
	*(va_arg(args, int32_t *)) = 200;
	*(va_arg(args, int32_t *)) = 300;
	*(va_arg(args, int32_t *)) = 400;
	*(va_arg(args, int32_t *)) = 500;

	return;
}

#endif

int
main(int argc, char **argv)
{
	int value1;
	int value2;
	int value3;

	scan("1 2 3", "%i32 %i32 %i32", &value1, &value2, &value3);

	fprintf(stdout, "%d %d %d\n", value1, value2, value3);

	return 0;
}

