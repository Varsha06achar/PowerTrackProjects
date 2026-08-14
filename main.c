#include "rbtree.h"
#include <stdio.h>

static void print_menu(void)
{
    printf("\n");
    printf("====================================\n");
    printf("        RED BLACK TREE MENU\n");
    printf("====================================\n");
    printf("1.  Insert\n");
    printf("2.  Delete\n");
    printf("3.  Search\n");
    printf("4.  Find Minimum\n");
    printf("5.  Delete Minimum\n");
    printf("6.  Find Maximum\n");
    printf("7.  Delete Maximum\n");
    printf("8.  Display Tree\n");
    printf("9.  Validate Tree\n");
    printf("0.  Exit\n");
    printf("====================================\n");
    printf("Enter choice: ");
}

static int read_integer(const char *prompt, int *value)
{
    int ch;

    printf("%s", prompt);

    if (scanf("%d", value) != 1) {
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
        return 0;
    }

    return 1;
}

int main(void)
{
    tree_t *root = NULL;
    int choice, value, result;

    printf("\nRed Black Tree - Data Structures Project\n");

    while (1) {
        print_menu();

        if (!read_integer("", &choice)) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {

        case 1:
            if (!read_integer("Enter value to insert: ", &value)) {
                printf("Invalid value.\n");
                break;
            }

            result = insert(&root, value);

            printf(result
                   ? "Inserted %d successfully.\n"
                   : "Insertion failed. Duplicate value or memory allocation failure.\n",
                   value);
            break;

        case 2:
            if (!read_integer("Enter value to delete: ", &value)) {
                printf("Invalid value.\n");
                break;
            }

            result = delete(&root, value);

            printf(result
                   ? "Deleted %d successfully.\n"
                   : "Deletion failed. Value not found or tree is empty.\n",
                   value);
            break;

        case 3:
            if (!read_integer("Enter value to search: ", &value)) {
                printf("Invalid value.\n");
                break;
            }

            printf(search(&root, value)
                   ? "%d is present in the tree.\n"
                   : "%d is not present in the tree.\n",
                   value);
            break;

        case 4:
            if (find_minimum(&root, &value))
                printf("Minimum value: %d\n", value);
            else
                printf("Tree is empty.\n");
            break;

        case 5:
            printf(delete_minimum(&root)
                   ? "Minimum node deleted successfully.\n"
                   : "Tree is empty.\n");
            break;

        case 6:
            if (find_maximum(&root, &value))
                printf("Maximum value: %d\n", value);
            else
                printf("Tree is empty.\n");
            break;

        case 7:
            printf(delete_maximum(&root)
                   ? "Maximum node deleted successfully.\n"
                   : "Tree is empty.\n");
            break;

        case 8:
            display_tree(&root);
            break;

        case 9:
            printf(validate_tree(&root)
                   ? "Tree is a valid Red Black Tree.\n"
                   : "Tree validation failed.\n");
            break;

        case 0:
            rbtree_destroy(&root);
            printf("Exiting...\n");
            return 0;

        default:
            printf("Invalid choice. Please try again.\n");
        }
    }
}