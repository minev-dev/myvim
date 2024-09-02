#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

#define LINE_NUMBER_PADDING 4

#define MIN_X LINE_NUMBER_PADDING + 1
#define MIN_Y 1

int pos[2] = {MIN_X, MIN_Y};
char content[5][100];

// TODO: Introduce enum
bool input_mode = false;

void read_file(char *file_path)
{
    FILE *fp = fopen(file_path, "r");

    int i = 0;
    while (fgets(content[i++], 100, fp))
    {
    }

    fclose(fp);
}

void save_file(char *file_path)
{
    FILE *fp = fopen(file_path, "w");

    for (int i = 0; i < 5; i++)
    {
        if (content[i][0] == 0)
        {
            break;
        }

        for (int j = 0; j < 100; j++)
        {
            if (content[i][j] == 0)
            {
                break;
            }

            fputc(content[i][j], fp);
        }
    }

    fclose(fp);
}

void render()
{
    clear();

    for (int i = 0; i < 5; i++)
    {
        if (content[i][0] == 0)
        {
            break;
        }

        // TODO: Fix for empty end line
        printf("%-2d| ", i + 1);

        for (int j = 0; j < 100; j++)
        {
            if (content[i][j] == 0)
            {
                break;
            }

            printf("%c", content[i][j]);
        }
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
            input_mode = false;
            continue;
        }

        if (input_mode)
        {
            content[pos[1] - 1][pos[0] - LINE_NUMBER_PADDING - 1] = input;
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
                input_mode = true;
                break;
            }
        }

        render();
    }

    // Restore original terminal settings
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    save_file(file_path);

    return 0;
}
