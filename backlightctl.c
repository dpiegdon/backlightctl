#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

unsigned read_file(const char * file)
{
	char buf[32];
	int c = 0;
	int fd = open(file, O_RDONLY);
	if(fd < 0)
	{
		printf("could not open '%s'\n", file);
		exit(1);
	}
	c = read(fd, buf, sizeof(buf-1));
	buf[c] = 0;
	return strtoul(buf, NULL, 10);
}

void write_file(const char * file, unsigned value)
{
	int fd = open(file, O_WRONLY);
	if(fd < 0) {
		printf("could not open '%s'\n", file);
		exit(1);
	}
	dprintf(fd, "%u", value);
	close(fd);
}

const char * generate_filename(const char * bl_name, const char * file)
{
	static char f[128];
	if(sizeof(f) <= snprintf(f, sizeof(f), "/sys/class/backlight/%s/%s", bl_name, file))
	{
		printf("filename too large: '/sys/class/backlight/%s/%s'\n", bl_name, file);
		exit(1);
	}
	return f;
}

unsigned current_brightness(const char * bl_name)
{
	return read_file(generate_filename(bl_name, "brightness"));
}

unsigned max_brightness(const char * bl_name)
{
	return read_file(generate_filename(bl_name, "max_brightness"));
}

unsigned set_brightness(const char * bl_name, unsigned value)
{
	write_file(generate_filename(bl_name, "brightness"), value);
}

void bl_name_is_safe(const char * bl_name)
{
	const char * p = bl_name;
	while(*p)
	{
		if(!(	   (*p >= 'a' && *p <= 'z')
			|| (*p >= 'A' && *p <= 'Z')
			|| (*p >= '0' && *p <= '9')
			|| (*p == '-')
			|| (*p == '_') ))
		{
			printf("invalid device name '%s'\n", bl_name);
			exit(1);
		}
		++p;
	}
}

int main(int argc, char**argv)
{
	char * bl_name;
	char unit;
	unsigned value;
	unsigned current;
	unsigned max;
	unsigned set;

	if(argc != 4)
	{
		printf("invalid argument count\n"
			"expecting arguments:\n"
			"<bl_name> <=|+|-> <value>\n");
		exit(1);
	}
	bl_name = argv[1];
	bl_name_is_safe(bl_name);
	if( (strlen(argv[2]) != 1))
	{
		printf("invalid 2nd arg\n");
		exit(1);
	}
	unit = argv[2][0];
	value = strtoul(argv[3], NULL, 10);

	current = current_brightness(bl_name);
	max = max_brightness(bl_name);

	switch(unit)
	{
		case '=':
			set = (value/100.) * max;
			if(set > max)
				set = max;
			break;
		case '+':
			set = (value/100.) * max;
			set += current;
			if(set > max)
				set = max;
			break;
		case '-':
			set = (value/100.) * max;
			if(set > current)
				set = 0;
			else
				set = current - set;
			break;
		default:
			printf("invalid 2nd arg\n");
			exit(1);
	}

	set_brightness(bl_name, set);
	printf("%.0f\n", (100.*(set / (float)max)));
}

