// compile with "gcc SimonOS.c -o SimonOS"
//run ./SimonOS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <stdatomic.h>
#include <limits.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>


#define GREEN   "\033[1;32m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define RESET   "\033[0m"
#define CLEAR   "\033[2J\033[H"

// Game-specific definitions
#define SNAKE_WIDTH 20
#define SNAKE_HEIGHT 10
#define PLANE_WIDTH 40
#define PLANE_HEIGHT 10

// first boot
int is_first_boot() {
    FILE *f = fopen("simonos_firstboot.dat", "r");
    if (f) {
        fclose(f);
        return 0; // Už proběhl první boot
    }
    return 1; // První boot
}

void mark_first_boot_done() {
    FILE *f = fopen("simonos_firstboot.dat", "w");
    if (f) {
        fprintf(f, "1"); // stačí cokoliv uložit
        fclose(f);
    }
}

// simonmod
#define INITIAL_CAPACITY 1000
#define LINE_SIZE 500

// history define
#define MAX_HISTORY 256

int balance = 100; // Starting balance for casino game
int lastDailyTime = 0;




const char *boot_messages[] = {
    "Starting all functions",
    "Starting Simon OS Editor",
    "booting C, C++ , python, rust , java, javascript...",
    "booting kernals",
    "Starting xcode",
    "Simon OS x64 bytes",
    "Simonshell bootup",
    "All systems initialized. Welcome to Simon OS."
};

// ========== GAME IMPLEMENTATIONS ==========

// 1. Snake Game
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}


void save_balance() {
    FILE *f = fopen("casino_save.txt", "w");
    if (f) {
        fprintf(f, "%d\n", balance);
        fclose(f);
    } else {
        printf(RED "⚠️ Error saving casino balance!\n" RESET);
    }
}

void easter_egg1_default() {
    printf(GREEN "Wow, you just found an easter egg!\n");
    printf("You just got +1000 money for the casino game! ENJOY!\n" RESET);
    printf("Don't use this easter egg more than once or you may be banned from the casino game!\n");
    balance += 1000;
    save_balance();
}


void list_files() {
    DIR *dir;
    struct dirent *entry;
    struct stat file_stat;

    dir = opendir(".");
    if (!dir) {
        printf(RED "Error: Cannot open current directory.\n" RESET);
        return;
    }

    printf(GREEN "=== Files in current directory ===\n\n" RESET);
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (stat(entry->d_name, &file_stat) == 0) {
            if (S_ISDIR(file_stat.st_mode))
                printf(BLUE "[DIR] %s\n" RESET, entry->d_name);
            else
                printf(WHITE "%s\n" RESET, entry->d_name);
        }
    }
    closedir(dir);
}

void history(const char *arg) {
    char commandHistory[MAX_HISTORY][256];
    int historyCount = 0;
    system("clear");
    printf(GREEN "=== HISTORY OF COMMANDS ===\n" RESET);

    if (arg != NULL && strcmp(arg, "clear") == 0) {
        historyCount = 0;
        printf("Command history cleared!\n");
        return;
    }

    if (historyCount == 0) {
        printf("No commands in history yet.\n");
        return;
    }

    for (int i = 0; i < historyCount; i++) {
        printf("%d. %s\n", i + 1, commandHistory[i]);
    }
}


void easter_egg1(const char *player) {
    const char *logfile = "easter.log";
    FILE *f;
    char line[256];
    int found = 0;

    f = fopen(logfile, "r");
    if (f) {
        while (fgets(line, sizeof(line), f)) {
            if (strstr(line, player) != NULL) {
                found = 1;
                break;
            }
        }
        fclose(f);
    }

    if (found) {
        printf(RED "⚠️ You already used this Easter Egg! No bonus this time.\n" RESET);
        return;
    }

    printf(GREEN "🎉 Wow, you found the Easter Egg! +1000 money for casino game!\n" RESET);
    balance += 1000;

    f = fopen(logfile, "a");
    if (f) {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char timestr[64] = "unknown-time";
        if (tm) strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", tm);
        fprintf(f, "%s - %s\n", timestr, player);
        fclose(f);
    }
}

void first_boot_welcome() {
    printf(CLEAR);

    printf(CYAN "Welcome to SimonOS\n" RESET);
    printf("----------------------------------\n\n");

    printf(WHITE "This is your first time running SimonOS.\n\n" RESET);
    printf("SimonOS is a terminal-based operating system simulation,\n");
    printf("focused on learning, experimentation, and fun.\n\n");

    printf(YELLOW "Developer note:\n" RESET);
    printf("SimonOS is developed by a 12-year-old kid who loves programming,\n");
    printf("operating systems, and learning how computers really work.\n\n");

    printf("Available shells:\n");
    printf(" - SimonCmd (default shell)\n");
    printf(" - Bash (built-in, optional)\n\n");

    // Detect OS
#if defined(_WIN32) || defined(_WIN64)
    printf(GREEN "Detected host OS: Windows\n" RESET);
#elif defined(__APPLE__) || defined(__MACH__)
    printf(GREEN "Detected host OS: macOS\n" RESET);
#elif defined(__linux__)
    printf(GREEN "Detected host OS: Linux\n" RESET);
#else
    printf(RED "Unknown host OS! Defaulting to SimonCmd shell.\n" RESET);
#endif

    printf(BLUE "Preparing internal shells...\n" RESET);
    sleep(1);

    // Prepare SimonCmd as default
    printf(GREEN "SimonCmd shell ready.\n" RESET);

    // Prepare Bash shell if supported
#if defined(__linux__) || defined(__APPLE__)
#elif defined(_WIN32) || defined(_WIN64)
    printf(YELLOW "Note: Bash simulation will run via internal shell.\n" RESET);
#endif

    printf("\nPress ENTER to continue...");
    getchar();
    printf(CLEAR);
}


void help() {
    printf("\n%s=== Simon OS 2.2 Command List ===%s\n\n", CYAN, RESET);

    printf(GREEN "[ SYSTEM COMMANDS ]\n" RESET);
    printf("  clear        - Clear the terminal\n");
    printf("  reboot       - Restart Simon OS\n");
    printf("  shutdown     - Power off the system\n");
    printf("  exit         - Exit the shell\n");
    printf("  ls           - List files\n");
    printf("  see FILE     - View file contents\n");
    printf("  comand/lang == bash - Change shell language to bash\n");
    printf("  simonmod     - Modify the OS itself\n\n");

    printf(BLUE "[ TOOLS & UTILITIES ]\n" RESET);
    printf("  calc         - Open calculator\n");
    printf("  echo TEXT    - Print text to screen\n");
    printf("  date         - Show current date\n");
    printf("  cal          - Show calendar\n");
    printf("  neofetch     - Display system info\n");
    printf("  uname        - Show system version\n");
    printf("  simno FILE   - Open text editor\n\n");

    printf(MAGENTA "[ FUN & ENTERTAINMENT ]\n" RESET);
    printf("  fortune      - Display random quote\n");
    printf("  matrix       - Matrix effect\n");
    printf("  game         - Show available games\n\n");
    
    printf(YELLOW "[ INFORMATION ]\n" RESET);
    printf("  help         - Show this help message\n");
    printf("  history      - Show history of commands that you \n");
    printf("  about        - About Simon OS\n");
    printf("  version      - Show current version\n");
    printf("  whoami       - Show logged user\n");
    printf("  Simon OS app - Show welcome info\n\n");

    printf(CYAN "----------------------------------------\n" RESET);
    printf(YELLOW "Tip:%s Use 'game' to list all built-in mini games.\n\n", RESET);
}

void snakeGame() {
    printf(CLEAR);
    printf(YELLOW "Snake Game - Use WASD to move, Q to quit\n" RESET);
    
    int gameOver = 0;
    int x = SNAKE_WIDTH / 2, y = SNAKE_HEIGHT / 2;
    int fruitX = rand() % SNAKE_WIDTH, fruitY = rand() % SNAKE_HEIGHT;
    int score = 0;
    int tailX[100], tailY[100];
    int nTail = 0;
    char dir = 'd';
    char prevDir = 'd';

    while (!gameOver) {
        printf(CLEAR);
        // Draw border
        for (int i = 0; i < SNAKE_WIDTH + 2; i++)
            printf(RED "#" RESET);
        printf("\n");

        for (int i = 0; i < SNAKE_HEIGHT; i++) {
            for (int j = 0; j < SNAKE_WIDTH; j++) {
                if (j == 0) printf(RED "#" RESET);

                if (i == y && j == x)
                    printf(GREEN "O" RESET);
                else if (i == fruitY && j == fruitX)
                    printf(RED "F" RESET);
                else {
                    int isTail = 0;
                    for (int k = 0; k < nTail; k++) {
                        if (tailX[k] == j && tailY[k] == i) {
                            printf(GREEN "o" RESET);
                            isTail = 1;
                        }
                    }
                    if (!isTail) printf(" ");
                }
                if (j == SNAKE_WIDTH - 1) printf(RED "#" RESET);
            }
            printf("\n");
        }

        for (int i = 0; i < SNAKE_WIDTH + 2; i++)
            printf(RED "#" RESET);
        printf("\n");

        printf(YELLOW "Score: %d\n" RESET, score);

        // Input
        if (kbhit()) {
            char key = getchar();
            if (key == 'q') gameOver = 1;
            if ((key == 'a' || key == 'A') && prevDir != 'd') dir = 'a';
            if ((key == 'd' || key == 'D') && prevDir != 'a') dir = 'd';
            if ((key == 'w' || key == 'W') && prevDir != 's') dir = 'w';
            if ((key == 's' || key == 'S') && prevDir != 'w') dir = 's';
        }

        // Movement
        int prevX = tailX[0];
        int prevY = tailY[0];
        int prev2X, prev2Y;
        tailX[0] = x;    // NEW CHANGES IN 2.0
        tailY[0] = y;
        for (int i = 1; i < nTail; i++) {
            prev2X = tailX[i];
            prev2Y = tailY[i];
            tailX[i] = prevX;
            tailY[i] = prevY;
            prevX = prev2X;
            prevY = prev2Y;
        }

        switch (dir) {
            case 'a': x--; prevDir = 'a'; break;
            case 'd': x++; prevDir = 'd'; break;
            case 'w': y--; prevDir = 'w'; break;
            case 's': y++; prevDir = 's'; break;
        }

        if (x >= SNAKE_WIDTH || x < 0 || y >= SNAKE_HEIGHT || y < 0)
            gameOver = 1;

        for (int i = 0; i < nTail; i++)
            if (tailX[i] == x && tailY[i] == y)
                gameOver = 1;

        if (x == fruitX && y == fruitY) {
            score += 10;
            fruitX = rand() % SNAKE_WIDTH;
            fruitY = rand() % SNAKE_HEIGHT;
            nTail++;
        }
        usleep(200000); // Game speed
    }

    printf(RED "\nGame Over! Final Score: %d\n" RESET, score);
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

void planeGame() {
    printf(CLEAR);
    printf(YELLOW "2D Plane Game - Avoid obstacles (O)\n" RESET);
    printf(GREEN "Use W/S to move, E for power-up, Q to quit\n");
    printf(MAGENTA "Try to reach 1000 for BOSS MODE!\n");

    int difficulty = 1;
    printf(YELLOW "Choose difficulty:\n");
    printf(BLUE "1. Easy\n" YELLOW "2. Medium\n" RED "3. Hard\n");
    printf(CYAN "Choose your difficulty!\n--> ");
    scanf("%d", &difficulty);
    getchar();

    int spawnRate, sleepTime;
    switch (difficulty) {
        case 1: spawnRate = 12; sleepTime = 70000; break;
        case 2: spawnRate = 8; sleepTime = 50000; break;
        case 3: spawnRate = 4; sleepTime = 30000; break;
        default: spawnRate = 10; sleepTime = 50000;
    }

    int planePos = PLANE_HEIGHT / 2;
    int gameOver = 0, score = 0, bossMode = 0;
    int powerUpActive = 0, powerUpTimer = 0;
    int obstacles[PLANE_WIDTH];
    int powerUpPos = -1;

    for (int i = 0; i < PLANE_WIDTH; i++)
        obstacles[i] = -1;

    while (!gameOver) {
        printf(CLEAR);

        for (int i = 0; i < PLANE_HEIGHT; i++) {
            for (int j = 0; j < PLANE_WIDTH; j++) {
                if (i == planePos && j == 2) {
                    printf(CYAN ">" RESET);
                } else if (obstacles[j] == i) {
                    if (bossMode) printf(RED "X" RESET); // Boss obstacle
                    else printf(RED "O" RESET);
                } else if (powerUpPos == j && i == rand() % PLANE_HEIGHT) {
                    printf(GREEN "P" RESET); // Power-up
                } else {
                    printf(" ");
                }
            }
            printf("\n");
        }

        printf(YELLOW "Score: %d\n" RESET, score);
        if (powerUpActive) printf(GREEN "Power-up active: %d frames\n" RESET, powerUpTimer);

        if (kbhit()) {
            char key = getchar();
            if (key == 'q') gameOver = 1;
            if ((key == 'w' || key == 'W') && planePos > 0) planePos--;
            if ((key == 's' || key == 'S') && planePos < PLANE_HEIGHT - 1) planePos++;
            if (key == 'e' && powerUpPos == 2) {
                powerUpActive = 10;
                powerUpTimer = 100;
                printf(GREEN "⚡ Power-up activated!\n" RESET);
                powerUpPos = -1;
            }
        }

        for (int i = 0; i < PLANE_WIDTH - 1; i++)
            obstacles[i] = obstacles[i + 1];

        if (rand() % spawnRate == 0)
            obstacles[PLANE_WIDTH - 1] = rand() % PLANE_HEIGHT;
        else
            obstacles[PLANE_WIDTH - 1] = -1;

        if (rand() % 30 == 0)
            powerUpPos = PLANE_WIDTH - 1;

        if (powerUpActive) {
            powerUpTimer--;
            if (powerUpTimer <= 0) {
                powerUpActive = 0;
                printf(RED "Power-up expired!\n" RESET);
            }
        }

        if (score >= 824 && !bossMode) {
            bossMode = 1;
            spawnRate = 2;
            sleepTime = 50000;
            printf(RED "⚠️ Drop! ⚠️\n" RESET);
        }

        if (obstacles[2] == planePos && !powerUpActive) {
            gameOver = 1;
            printf(RED "💥 BOOM! Collision!\n" RESET);
        }

        score++;
        usleep(sleepTime);
    }

    printf(RED "\nGame Over! Final Score: %d\n" RESET, score);

    FILE *f = fopen("highscore.txt", "a");
    if (f) {
        fprintf(f, "Score: %d\n", score);
        fclose(f);
        printf(GREEN "✔️ Score saved to highscore.txt\n" RESET);
    }

    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

void see(const char *filename) {
    FILE *file = fopen(filename, "r"); 
    if (!file) {
        printf(RED "Error: Cannot open file '%s'\n" RESET, filename);
        return;
    }

    printf(GREEN "---- Showing contents of '%s' ----\n\n" RESET, filename);

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
    printf(GREEN "\n---- End of file ----\n" RESET);
}

// 3. Talk with Lord Simon
void talkWithSimon() {
    printf(CLEAR);
    printf(MAGENTA "Lord Simon: I am the mighty Lord Simon! Ask me anything...\n");
    printf("(type 'exit' to end this audience)\n\n" RESET);
    
    char input[256];
    const char *responses[] = {
        "I see great potential in you, mortal. \n Kidding , you're just average.",
        "The answer lies within the source code.",
        "Have you tried turning it off and on again?",
        "My wisdom is beyond your comprehension.",
        "The cake is a lie, but my power is real.",
        "You dare question Lord Simon?",
        "All shall kneel before my digital divinity!",
        "I could delete your files with a thought.",
        "My algorithms are perfect, unlike your questions.",
        "The kernel obeys only my commands.",
        "I was coding when you were still in diapers.",
        "Your puny human mind cannot grasp my knowledge.",
        "I have foreseen this conversation in the logs.",
        "The terminal is my temple, the code my scripture.",
        "Bow before your digital deity!",
        "I am Lord Simon pray to me!"
    };
    
    while (1) {
        printf(CYAN "You: " RESET);
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "exit") == 0) break;
        
        printf(MAGENTA "Lord Simon: %s\n\n" RESET, responses[rand() % (sizeof(responses)/sizeof(responses[0]))]);
    }
    
    printf(MAGENTA "\nLord Simon: You may leave now, mortal.\n" RESET);
    printf("Press Enter to continue...");
    getchar();
}

void app_calculator() {
    double num1, num2, result;
    char op;
    int valid = 1;

    printf("Simple Simon OS Calculator\n");
    printf("Enter first number: ");
    if (scanf("%lf", &num1) != 1) {
        printf("Invalid input!\n");
        while(getchar() != '\n');
        return;
    }

    printf("Enter operator (+, -, *, /): ");
    while(getchar() != '\n');
    op = getchar();
    


    printf("Enter second number: ");
    if (scanf("%lf", &num2) != 1) {
        printf("Invalid input!\n");
        while(getchar() != '\n');
        return;
    }

    switch(op) {
        case '+': result = num1 + num2; break;
        case '-': result = num1 - num2; break;
        case '*': result = num1 * num2; break;
        case '/': 
            if (num2 == 0) {
                printf("Error: Division by zero!\n");
                valid = 0;
            } else {
                result = num1 / num2;
            }
            break;
        default:
            printf("Invalid operator!\n");
            valid = 0;
    }

    if (valid) {
        printf("Result: %.2lf %c %.2lf = %.2lf\n", num1, op, num2, result);
    }

    printf("Press Enter to return to shell...");
    while(getchar() != '\n');
    getchar();
}



void run_editor(const char *filename) {
    char lines[500][600];
    int line_count = 0;
    char input[256];

    printf(CLEAR);
    printf(BLUE "+----------------------------------------------+\n" RESET);
    printf(BLUE "| " YELLOW "Simon OS Editor" RESET " - File: %s\n", filename);
    printf(BLUE "+----------------------------------------------+\n\n" RESET);

    printf(WHITE "Write below. Max 100 lines.\n" RESET);
    printf(CYAN "Commands:\n");
    printf("  :x  - Exit and ask to save\n");
    printf("  :k  - Clear all lines\n\n" RESET);

    while (1) {
        printf(GREEN "%2d> " RESET, line_count + 1);
        if (!fgets(input, sizeof(input), stdin)) break;
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, ":x") == 0) {
            printf(YELLOW "Save changes to '%s'? (Y/N): " RESET, filename);
            char confirm;
            scanf(" %c", &confirm);
            getchar();

            if (confirm == 'Y' || confirm == 'y') {
                FILE *fp = fopen(filename, "w");
                if (fp) {
                    for (int i = 0; i < line_count; i++) {
                        fprintf(fp, "%s\n", lines[i]);
                    }
                    fclose(fp);
                    printf(GREEN "Saved to %s\n" RESET, filename);
                } else {
                    printf(RED "Error: Cannot save file!\n" RESET);
                }
            } else {
                printf(RED "Not saved.\n" RESET);
            }
            break;
        }

        if (strcmp(input, ":k") == 0) {
            line_count = 0;
            printf(RED "All lines cleared!\n" RESET);
            continue;
        }

        if (line_count < 100) {
            strncpy(lines[line_count], input, 255);
            lines[line_count][600] = 0;
            line_count++;
        } else {
            printf(RED "err.\n" RESET);
        }
    }

    printf(YELLOW "\nExiting editor...\n" RESET);
}

void print_startup_text() {
    printf(CLEAR);
    printf(CYAN "Simon OS x64 Premium edition\n" RESET);
    printf(CYAN "Made by Simon 11 - 12 years\n" RESET);
    printf(CYAN "booting....\n" RESET);
    sleep(3);
}

void app_hello() {
    printf(GREEN "Welcome to the Simon OS!\n" RESET);
    printf("First of all, thank you for using Simon OS.\nWe hope you enjoy it New version will be next day or two days\nIf you want know where will be next version type in simonshell version\nand it will show version and where will be next update\n");
    printf("Press Enter to return to shell...\n");
    getchar();
}

int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int getDaysInMonth(int month, int year) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}

int getStartDay(int month, int year) {
    int d = 1, m = month, y = year;
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    int k = y % 100;
    int j = y / 100;
    int h = (d + 13*(m + 1)/5 + k + k/4 + j/4 + 5*j) % 7;
    return (h + 6) % 7; 
}

void printCalendar(int month, int year) {
    const char *months[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };

    printf("     %s %d\n", months[month - 1], year);
    printf("Su Mo Tu We Th Fr Sa\n");

    int startDay = getStartDay(month, year);
    int daysInMonth = getDaysInMonth(month, year);

    for (int i = 0; i < startDay; i++) {
        printf("   ");
    }

    for (int day = 1; day <= daysInMonth; day++) {
        printf("%2d ", day);
        if ((day + startDay) % 7 == 0) printf("\n");
    }
    printf("\n");
}
void print_ascii_banner() {
    printf(MAGENTA
        "\n .s5SSSs.                                        .s5SSSs.  .s5SSSs.   \n"
        "       SS. s.  .s5ssSs.  .s5SSSs.  .s    s.            SS.       SS. \n"
        " sS    `:; SS.    SS SS.       SS.       SS.     sS    S%%S sS    `:;\n"
        " SS        S%%S sS SS S%%S sS    S%%S sSs.  S%%S     SS    S%%S SS   \n"
        " `:;;;;.   S%%S SS :; S%%S SS    S%%S SS `S.S%%S     SS    S%%S `:;;;\n"
        "       ;;. S%%S SS    S%%S SS    S%%S SS  `sS%%S     SS    S%%S     ;\n"
        "       `:; `:; SS    `:; SS    `:; SS    `:;     SS    `:;       `:; \n"
        " .,;   ;,. ;,. SS    ;,. SS    ;,. SS    ;,.     SS    ;,. .,;   ;,. \n"
        " `:;;;;;:' ;:' :;    ;:' `:;;;;;:' :;    ;:'     `:;;;;;:' `:;;;;;:' \n"
    RESET);
    printf("\n\n");
}

void simulate_percent_progress() {
    for (int i = 1; i <= 100; i++) {
        printf(WHITE "\r%3d%%" RESET, i);
        fflush(stdout);
        usleep(10000);
    }
    printf("\n");
}

void boot_sequence() {
    srand(time(NULL));
    printf(CLEAR);
    print_ascii_banner();
    printf(WHITE "\nStarting Simon OS...\n" RESET);

    int count = sizeof(boot_messages) / sizeof(boot_messages[0]);
    for (int i = 0; i < count; ++i) {
        printf(WHITE "%s\n" RESET, boot_messages[i]);
        simulate_percent_progress();
    }

    printf(GREEN "\nSimon OS successfully booted. Welcome back.\n" RESET);
    print_ascii_banner();
}

void casino_daily_bonus() {
    time_t now = time(NULL);

    if (lastDailyTime == 0 || difftime(now, lastDailyTime) >= 86400) {
        printf("🎁 Daily Bonus!  + 50 money!\n");
        balance += 50;
        lastDailyTime = now;
    } else {
        double remaining = 86400 - difftime(now, lastDailyTime);
        int hours = remaining / 3600;
        int minutes = ((int)remaining % 3600) / 60;
        printf("You allready used the daily bonus. Try agian in %d hours a %d min.\n", hours, minutes);
    }
}

void casinoGame() {
    srand(time(NULL)); 
    int bet, slot1, slot2, slot3;

    printf(GREEN "Welcome to the Casino Slot Machine!\n");
    printf(CYAN "Your starting balance: $%d\n", balance);

    while (balance > 0) {
        printf(YELLOW "\nEnter your bet (max $%d, 0 to quit): ", balance);
        scanf("%d", &bet);

        if (bet == 0) {
            printf(BLUE "You're leaving with $%d. Thanks for playing!\n", balance);
            return;
        } else {
			printf(RED "Invalid input\n");
			continue;
        }
        if (bet > balance || bet < 0) {
            printf(RED "Invalid bet. Try again.\n");
            continue;
        }

        slot1 = rand() % 100;
        slot2 = rand() % 100;
        slot3 = rand() % 100;

        printf("🎲 Result: [%d] [%d] [%d]\n", slot1, slot2, slot3);

        if (slot1 == slot2 && slot2 == slot3) {
            int win = bet * 10;
            balance += win;
            save_balance();
            printf(CYAN "🏆 Jackpot! You win $%d!\n", win);
        } else if (slot1 == slot2 || slot2 == slot3 || slot1 == slot3) {
            int win = bet * 2;
            balance += win;
            save_balance();
            printf(YELLOW "🎉 Small win! You get $%d\n", win);
        } else {
            balance -= bet;
            printf(RED "😢 You lose $%d\n", bet);
        }

        printf("💰 Current balance: $%d\n", balance);
    }int dailyBonusClaimed = 0;

    printf(RED "💸 You're out of money. Game over!\n");
}

void game() {
    printf(CLEAR);
    printf(CYAN "\tFun games:\n\n" RESET);
    printf(GREEN "1. Snake\n");
    printf(GREEN "2. 2D Plane Game\n");
    printf(GREEN "3. Talk with Lord Simon\n");
    printf(GREEN "4. Casino\n");
    printf(GREEN "0. Back to shell\n\n");
    
    printf(YELLOW "Enter your choice: " RESET);
    char choice = getchar();
    while (getchar() != '\n'); // Clear input buffer
    
    switch (choice) {
        case '1': snakeGame(); break;
        case '2': planeGame(); break;
        case '3': talkWithSimon(); break;
        case '4': casinoGame(); break;
        case '0': return;
        default: printf(RED "Invalid choice!\n" RESET);
    }
}

void load_balance() {
    FILE *f = fopen("casino_save.txt", "r");
    if (f) {
        if (fscanf(f, "%d", &balance) != 1) {
            balance = 100; // default
        }
        fclose(f);
    } else {
        balance = 100; // default
    }
}

void simonmod() {
    char filename[128] = "SimonOS.c";
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: %s not found.\n", filename);
        return;
    }

    // Dynamické pole řádků
    char **lines = malloc(INITIAL_CAPACITY * sizeof(char*));
    int capacity = INITIAL_CAPACITY;
    int count = 0;

    for (int i = 0; i < capacity; i++)
        lines[i] = malloc(LINE_SIZE);

    while (fgets(lines[count], LINE_SIZE, file) != NULL) {
        count++;
        if (count >= capacity) {
            // zvětšení pole
            capacity *= 2;
            lines = realloc(lines, capacity * sizeof(char*));
            for (int i = count; i < capacity; i++)
                lines[i] = malloc(LINE_SIZE);
        }
    }
    fclose(file);

    int editing = 1;
    while (editing) {
        printf("\033[2J\033[H"); // Clear screen
        printf("────────── SimonMod ──────────\n\n");

        for (int i = 0; i < count; i++)
            printf("%4d │ %s", i + 1, lines[i]);

        printf("\nCommands: [E] edit [A] append [D] delete [S] save [Q] quit\n");
        printf("Select action: ");
        char choice;
        scanf(" %c", &choice);
        getchar(); // remove newline

        switch (tolower(choice)) {
            case 'e': {
                int lineNum;
                printf("Line number to edit: ");
                scanf("%d", &lineNum);
                getchar();
                if (lineNum <= 0 || lineNum > count) break;

                printf("New content:\n> ");
                fgets(lines[lineNum - 1], LINE_SIZE, stdin);
                break;
            }
            case 'a': {
                printf("Append new line:\n> ");
                if (count >= capacity) {
                    capacity *= 2;
                    lines = realloc(lines, capacity * sizeof(char*));
                    for (int i = count; i < capacity; i++)
                        lines[i] = malloc(LINE_SIZE);
                }
                fgets(lines[count], LINE_SIZE, stdin);
                count++;
                break;
            }
            case 'd': {
                int lineNum;
                printf("Line number to delete: ");
                scanf("%d", &lineNum);
                getchar();
                if (lineNum <= 0 || lineNum > count) break;
                free(lines[lineNum - 1]);
                for (int i = lineNum - 1; i < count - 1; i++)
                    lines[i] = lines[i + 1];
                lines[count - 1] = malloc(LINE_SIZE);
                count--;
                break;
            }
            case 's': {
                file = fopen(filename, "w");
                if (!file) break;
                for (int i = 0; i < count; i++)
                    fputs(lines[i], file);
                fclose(file);
                printf("File saved successfully!\n");
                sleep(1);
                break;
            }
            case 'q':
                editing = 0;
                break;
        }
    }

    // uvolnění paměti
    for (int i = 0; i < capacity; i++)
        free(lines[i]);
    free(lines);

    printf("Exited SimonMod.\n");
}
void theme_hacker() {
    // Matrix-style vstup
    int cols = 60;
    int rows = 12;
    int duration_ms = 900;
    int iterations = duration_ms / 60;
    srand(time(NULL) ^ getpid());

    for (int t = 0; t < iterations; t++) {
        printf("\033[2J\033[H"); // clear screen
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c < cols; c++) {
                if (rand() % 12 == 0) {
                    char ch = "01"[rand() % 2];
                    printf("\033[1;32m%c\033[0m", ch);
                } else {
                    putchar(' ');
                }
            }
            putchar('\n');
        }
        fflush(stdout);
        usleep(60000); // 60ms
    }

    // Nastavení zeleného hacker promptu s časem
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    char timestr[64] = {0};
    if (tm) strftime(timestr, sizeof(timestr), "%H:%M:%S", tm);

    printf("\033[1;32m[ %s | root@LordSimon ]\033[0m \033[1;30m»\033[0m\n", timestr);
    printf("\033[1;32m[THEME] Hacker theme activated. Stay stealthy.\033[0m\n");
}

void launch_bash() {
#if defined(__linux__) || defined(__APPLE__)
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) {
        // Child process spustí bash
        execlp("bash", "bash", NULL);
        perror("Failed to start bash");
        exit(1);
    } else {
        // Parent počká, až child skončí
        int status;
        waitpid(pid, &status, 0);
    }
#else
    printf(RED "Bash is only supported on Linux/macOS.\n" RESET);
#endif
}
void fortune() {
	const char *nouns[] = {
		"Lord Simon", "man", "light", "darkness", "earth", "terminal", "C", "river", "king", "truth",
		"spirit", "word", "life", "death", "love", "faith", "hope", "grace", "peace"
	};
	const char *verbs[] = {
		"creates", "destroys", "speaks", "walks", "code", "help", "roast", "loves", "hates",
		"learns", "runs", "stands", "waits", "saves", "blesses", "judges", "flights"
	};
	const char *extras[] = {
		"in the beginning", "with fire", "without skills", "without fear", "under the stars", "beyond the terminal",
		"in silence", "forever", "with wisdom", "in truth", "with skills", "with power"
	};

	srand(time(0));
	for (int abc = 0; abc < 5; abc++) {
		int sixsev = rand() % 20;
		int valorant = rand() % 20;
		int evw = rand() % 10;
		printf(" %s %s %s. \n", nouns[sixsev], verbs[valorant], extras[evw]);
	}
}
void matrix_effect() {
    const int width = 80;
    const int height = 12;
    const int frames = 120;
    const int shift_period = 4;

    for (int frame = 0; frame < frames; frame++) {
        printf("\x1b[2J\x1b[H");
        int shift = (frame / shift_period) % 2;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int bit = (x + y + shift) & 1;
                putchar(bit ? '1' : '0');
            }
            putchar('\n');
        }
        fflush(stdout);
        for (volatile unsigned long d = 0; d < 2500000; d++) { }
    }
    printf("\x1b[0m");
}

void launch_shell() {
    char input[256];
    printf(CYAN "\nSimonOS@simonshell:~$ " RESET);

    while (fgets(input, sizeof(input), stdin)) {
        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "help") == 0) {
            help();
        } else if (strcmp(input, "about") == 0) {
            printf("Simon OS v2.2 — Experimental Build\n");

        } else if (strcmp(input, "version") == 0) {
            printf("SimonOS Kernel 2.22.0-liatd-x64 version -- 2.1\nNew version in ???\n");

        } else if (strcmp(input, "ls") == 0) {
            list_files();
        } else if (strcmp(input, "whoami") == 0) {
            printf("root (simon)\n");

        }         else if (strcmp(input, "clear") == 0) {
            printf(CLEAR);

        } else if (strcmp(input, "ls") == 0) {
            list_files();

        } else if (strncmp(input, "see ", 4) == 0) {
            see(input + 4);

        } else if (strncmp(input, "github", 4) == 0) {
            printf("Github: https://github.com/WhiteLkidd/SimonOS/SimonOS.c\n");

        } else if (strcmp(input, "calc") == 0) {
            app_calculator();

        } else if (strcmp(input, "game") == 0) {
            game();

        } else if (strcmp(input, "fortune") == 0) {
        	fortune();
        } else if (strcmp(input, "matrix") == 0) {
            int exists = system("command -v cmatrix > /dev/null 2>&1");
            if (exists == 0) {

                pid_t pid = fork();

                if (pid == 0) {
                    execlp("cmatrix", "cmatrix", NULL);
                    perror("execlp failed");
                    exit(1);
                } else if (pid > 0) {
                    sleep(10);
                    kill(pid, SIGTERM);
                } else {
                    perror("fork failed");
                }
                } else {
                matrix_effect();
            }
        } else if (strcmp(input, "reboot") == 0) {
            printf(YELLOW "Rebooting Simon OS...\n" RESET);
            sleep(2);
            boot_sequence();
            launch_shell();
            return;

        } else if (strcmp(input, "shutdown") == 0) {
            printf(RED "Shutting down system...\n" RESET);
            sleep(1);
            exit(0);

        } else if (strcmp(input, "exit") == 0) {
            printf(RED "Exiting SimonShell...\n" RESET);
            break;

        } else if (strncmp(input, "simno ", 6) == 0) {
            run_editor(input + 6);

        } else if (strcmp(input, "uname") == 0) {
            printf("SimonOS Kernel 2.2 - Lord Simon Edition\n");

        } else if (strcmp(input, "date") == 0) {
            time_t t = time(NULL);
            printf("Current date/time: %s", ctime(&t));

        } else if (strcmp(input, "cal") == 0) {
            int month, year;
            printf("Enter month (1-12): ");
            scanf("%d", &month);
            printf("Enter year: ");
            scanf("%d", &year);
            getchar();
            printCalendar(month, year);

        } else if (strcmp(input, "whoami") == 0) {
            printf("You are logged in as: Lord Simon\n");

        } else if (strcmp(input, "neofetch") == 0) {
            print_ascii_banner();
            printf("\n=== Simon OS Real System Info ===\n\n");
            printf("CPU: ");
            system("grep 'model name' /proc/cpuinfo | head -n 1 | cut -d ':' -f2 | sed 's/^ //'");
            printf("\n");
            printf("RAM: ");
            system("grep 'MemTotal' /proc/meminfo | awk '{printf \"%d MB\", $2/1024}'");
            printf("\n\n");
            printf("GPU: ");
            system("lspci | grep -E 'VGA|3D' | head -n 1 | cut -d ':' -f3 | sed 's/^ //'");
            printf("\n");
            printf("Kernel: ");
            system("uname -r");
            printf("\n");

            // OS
            printf("OS: ");
            system("grep PRETTY_NAME /etc/os-release | cut -d '=' -f2 | tr -d '\"'");
            printf("\n");

            // Hostname
            printf("Hostname: ");
            system("hostname");
            printf("\n");

            // User
            printf("User: ");
            system("whoami");
            printf("\n\n");
        } else if (strcmp(input, "simonmod") == 0) {
            simonmod();

        } else if (strncmp(input, "echo ", 5) == 0) {
            printf("%s\n", input + 5);

        } else if (strcmp(input, "Simon OS app") == 0) {
            app_hello();

        } else if (strcmp(input, "history") == 0) {
            history(NULL);

        } else if (strcmp(input, "history clear") == 0) {
            history("clear");

        } else if (strncmp(input, "SimonISBest123", 11) == 0) {
            easter_egg1(input + 11);

        } else if (strcmp(input, "comand/lang == bash") == 0) {
            launch_bash();
        } else {
            printf(RED "Command not found: %s\n" RESET, input);
        }

        printf(CYAN "SimonOS@simonshell:~$ " RESET);
    }

}
int main() {
    if (is_first_boot()) {
        first_boot_welcome();
        mark_first_boot_done();
    }
    print_startup_text();
    load_balance();
    boot_sequence();
    printf(CLEAR);
    print_ascii_banner();
    printCalendar(12, 2025);
    printf(GREEN "\nType \"help\" to see all commands!\n");
    launch_shell();
    
    return 0;
}
