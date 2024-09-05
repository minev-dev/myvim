#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

#define LINE_NUM_LEN 2
#define LINE_NUM_SEP_LEN 2
#define LINE_NUM_PADDING (LINE_NUM_LEN + LINE_NUM_SEP_LEN)

#define MIN_X LINE_NUM_PADDING + 1
#define MIN_Y 1

enum InputMode
{
    NORMAL = 0,
    INSERT = 1,
};

int pos[2] = {MIN_X, MIN_Y};

// TODO: Store content in dynamic array
char content[100];

enum InputMode g_input_mode = NORMAL;

void read_file(char *file_path)
{
    FILE *fp = fopen(file_path, "r");

    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF)
    {
        content[i++] = c;
    }

    fclose(fp);
}

void save_file(char *file_path)
{
    FILE *fp = fopen(file_path, "w");

    for (int i = 0; i < 100; i++)
    {
        if (content[i] == 0)
        {
            break;
        }

        fputc(content[i], fp);
    }

    fclose(fp);
}

void render()
{
    clear();

    unsigned int line_n = 1;

    char prev_c;
    char c;
    for (int i = 0; i < 100; i++)
    {
        if (i == 0 || prev_c == '\n')
        {
            printf("%-*d| ", LINE_NUM_LEN, line_n++);
        }

        c = content[i];
        if (c == 0)
        {
            break;
        }

        printf("%c", c);

        prev_c = c;
    }

    gotoxy(pos[0], pos[1]);
}

int main(int argc, char *argv[])
{
    char *file_path = argv[1];

    read_file(file_path);

    render();

    //
    // No-echo input
    //
    struct termios oldt, newt;

    // Get current terminal settings
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Disable echoing
    newt.c_lflag &= ~(ICANON | ECHO);

    // Set new terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    // Read input without echo
    char input;
    while (input != ':')
    {
        input = getchar();

        if (input == '\033')
        {
            g_input_mode = NORMAL;
            continue;
        }

        if (g_input_mode == INSERT)
        {
            // TODO: fix
            // content[pos[1] - 1][pos[0] - LINE_NUM_PADDING - 1] = input;
            pos[0] += 1;
        }
        else
        {
            switch (input)
            {
            case 'k':
                if (pos[1] == MIN_Y)
                    break;

                pos[1] -= 1;
                break;
            case 'j':
                pos[1] += 1;
                break;
            case 'h':
                if (pos[0] == MIN_X)
                    break;

                pos[0] -= 1;
                break;
            case 'l':
                pos[0] += 1;
                break;
            case 'i':
                g_input_mode = INSERT;
                break;
            }
        }

        render();
    }

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    save_file(file_path);

    clear();

    return 0;
}
