#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char shortcut[50];
    char action[100];
} Shortcut;

Shortcut shortcuts[10];
int shortcut_count = 0;

void add_shortcut() {
    if (shortcut_count >= 10) {
        printf("Maximum shortcuts reached.\n");
        return;
    }
    printf("Enter shortcut (e.g., ctrl+alt+y): ");
    scanf("%s", shortcuts[shortcut_count].shortcut);
    printf("Enter action (e.g., open_youtube): ");
    scanf("%s", shortcuts[shortcut_count].action);
    shortcut_count++;
    printf("Shortcut added.\n");
}

void list_shortcuts() {
    for (int i = 0; i < shortcut_count; i++) {
        printf("%d. %s -> %s\n", i + 1, shortcuts[i].shortcut, shortcuts[i].action);
    }
}

int main() {
    int choice;
    while (1) {
        printf("\nShortcut Manager:\n1. Add Shortcut\n2. List Shortcuts\n3. Exit\n");
        scanf("%d", &choice);
        if (choice == 1) {
            add_shortcut();
        } else if (choice == 2) {
            list_shortcuts();
        } else if (choice == 3) {
            exit(0);
        } else {
            printf("Invalid choice.\n");
        }
    }
    return 0;
}
