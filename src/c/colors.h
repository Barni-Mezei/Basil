#define color_red "\033[31m"
#define color_green "\033[32m"
#define color_gray "\033[30m"
#define color_white "\033[m"

void print_color(char *str, char *color) {
    printf("%s%s%s\n", color, str, color_white);
}

void write_color(char *str, char *color) {
    printf("%s%s%s", color, str, color_white);
}