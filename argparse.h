#ifndef ARGPARSE_H
#define ARGPARSE_H

#define ARG_TERMINAL_RED "\033[1;31m"
#define ARG_TERMINAL_YLW "\033[1;33m"
#define ARG_TERMINAL_RST "\033[0m"

#define ARG_LOG_WARN(format, ...)                                              \
    fprintf(stdout,                                                            \
            ARG_TERMINAL_YLW "WARN" ARG_TERMINAL_RST ": %s:%d: " format "\n",  \
            __FILE__, __LINE__, ##__VA_ARGS__)

#define ARG_LOG_ERROR(format, ...)                                             \
    fprintf(stderr,                                                            \
            ARG_TERMINAL_RED "ERROR" ARG_TERMINAL_RST ": %s:%d: " format "\n", \
            __FILE__, __LINE__, ##__VA_ARGS__)

#define ARG_PANIC(format, ...)                                                 \
    do {                                                                       \
        ARG_LOG_ERROR(format, ##__VA_ARGS__);                                  \
        exit(1);                                                               \
    } while (0)

#define ARG_INIT_CAPACITY 8192
#define ARG_DA_REALLOC(ptr, old_sz, new_sz) realloc(ptr, new_sz)
#define arg_da_append(da, item)                                                \
    do {                                                                       \
        if ((da)->count >= (da)->capacity) {                                   \
            unsigned int new_capacity = (da)->capacity * 2;                    \
            if (new_capacity == 0) {                                           \
                new_capacity = ARG_INIT_CAPACITY;                              \
            }                                                                  \
                                                                               \
            (da)->items = ARG_DA_REALLOC(                                      \
                (da)->items, (da)->capacity * sizeof(*(da)->items),            \
                new_capacity * sizeof(*(da)->items));                          \
            if ((da)->items == NULL) {                                         \
                ARG_PANIC("Failed to reallocate dynamic array");               \
            }                                                                  \
                                                                               \
            (da)->capacity = new_capacity;                                     \
        }                                                                      \
                                                                               \
        (da)->items[(da)->count++] = (item);                                   \
    } while (0)

enum argument_type {
    ARGUMENT_TYPE_VALUE,
    ARGUMENT_TYPE_FLAG,
    ARGUMENT_TYPE_POSITIONAL,
};

struct argparse_parser;

struct argparse_parser *argparse_new(char *name, char *description,
                                     char *version);
void argparse_add_argument(struct argparse_parser *parser, char short_name,
                           char *long_name, char *description,
                           enum argument_type type);
void argparse_parse(struct argparse_parser *parser, int argc, char *argv[]);
char *argparse_get_value(struct argparse_parser *parser, char *long_name);
unsigned int argparse_get_flag(struct argparse_parser *parser, char *long_name);
char *argparse_get_positional(struct argparse_parser *parser, char *long_name);

void argparse_print_help(struct argparse_parser *parser);
void argparse_print_version(struct argparse_parser *parser);

void argparse_free(struct argparse_parser *parser);

#endif // ARGPARSE_H

#ifdef ARGPARSE_IMPLEMENTATION

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct argument {
        char short_name;
        char *long_name;
        char *description;
        enum argument_type type;
        char *value;
        unsigned int flag;
};

struct argparse_parser {
        char *name;
        char *description;
        char *version;
        struct argument *items;
        size_t count;
        size_t capacity;
};

struct argparse_parser *argparse_new(char *name, char *description,
                                     char *version) {
    struct argparse_parser *parser = malloc(sizeof(struct argparse_parser));

    parser->name = name;
    parser->description = description;
    parser->version = version;
    parser->items = NULL;
    parser->count = 0;
    parser->capacity = 0;

    argparse_add_argument(parser, 'v', "version", "print the program version",
                          ARGUMENT_TYPE_FLAG);
    argparse_add_argument(parser, 'h', "help", "print help menu",
                          ARGUMENT_TYPE_FLAG);

    return parser;
}

void argparse_add_argument(struct argparse_parser *parser, char short_name,
                           char *long_name, char *description,
                           enum argument_type type) {
    struct argument arg = {
        .short_name = short_name,
        .long_name = long_name,
        .description = description,
        .type = type,
        .value = NULL,
        .flag = 0,
    };

    arg_da_append(parser, arg);
}

void argparse_parse(struct argparse_parser *parser, int argc, char *argv[]) {
    for (int i = 1; i < argc; i++) {
        char *name = argv[i];

        if (strcmp(name, "-h") == 0 || strcmp(name, "--help") == 0) {
            argparse_print_help(parser);
            exit(0);
        }

        if (strcmp(name, "-v") == 0 || strcmp(name, "--version") == 0) {
            argparse_print_version(parser);
            exit(0);
        }

        if (name[0] == '-') {
            struct argument *arg = NULL;

            for (size_t j = 0; j < parser->count; j++) {
                struct argument *item = &parser->items[j];

                if ((name[1] == '-' && item->long_name != NULL &&
                     strcmp(name + 2, item->long_name) == 0) ||
                    (name[1] != '-' && item->short_name != '\0' &&
                     name[1] == item->short_name)) {
                    arg = item;
                    break;
                }
            }

            if (arg == NULL) {
                ARG_LOG_ERROR("invalid argument: %s", name);
                argparse_print_help(parser);
                exit(1);
            }

            switch (arg->type) {
            case ARGUMENT_TYPE_FLAG: {
                arg->flag = 1;
                break;
            }
            case ARGUMENT_TYPE_VALUE: {
                if (i + 1 >= argc) {
                    ARG_LOG_ERROR("missing value for argument: %s", name);
                    argparse_print_help(parser);
                    exit(1);
                }

                arg->value = argv[++i];
                break;
            }
            default: {
                ARG_LOG_ERROR("type not supported for argument: %s", name);
                argparse_print_help(parser);
                exit(1);
            }
            }
        } else {
            struct argument *arg = NULL;

            for (size_t j = 0; j < parser->count; j++) {
                struct argument *item = &parser->items[j];

                if (item->type == ARGUMENT_TYPE_POSITIONAL &&
                    item->value == NULL) {
                    arg = item;
                    break;
                }
            }

            if (arg == NULL) {
                ARG_LOG_ERROR("unexpected positional argument: %s", name);
                argparse_print_help(parser);
                exit(1);
            }

            arg->value = name;
        }
    }

    for (size_t i = 0; i < parser->count; i++) {
        struct argument *item = &parser->items[i];

        if (item->type == ARGUMENT_TYPE_POSITIONAL && item->value == NULL) {
            ARG_LOG_ERROR("missing value for positional argument: %s",
                          item->long_name);
            argparse_print_help(parser);
            exit(1);
        }
    }
}

char *argparse_get_value(struct argparse_parser *parser, char *long_name) {
    for (size_t i = 0; i < parser->count; i++) {
        struct argument *item = &parser->items[i];

        if (item->long_name != NULL &&
            strcmp(long_name, item->long_name) == 0) {
            if (item->type != ARGUMENT_TYPE_VALUE &&
                item->type != ARGUMENT_TYPE_POSITIONAL) {
                ARG_LOG_WARN("argument is not a value: %s", long_name);
            }
            return item->value;
        }
    }

    return NULL;
}

unsigned int argparse_get_flag(struct argparse_parser *parser,
                               char *long_name) {
    for (size_t i = 0; i < parser->count; i++) {
        struct argument *item = &parser->items[i];

        if (item->long_name != NULL &&
            strcmp(long_name, item->long_name) == 0) {
            if (item->type != ARGUMENT_TYPE_FLAG) {
                ARG_LOG_WARN("argument is not a flag: %s", long_name);
            }
            return item->flag;
        }
    }

    return 0;
}

void argparse_print_help(struct argparse_parser *parser) {
    printf("usage: %s [options]", parser->name);
    for (size_t i = 0; i < parser->count; i++) {
        struct argument *item = &parser->items[i];

        if (item->type == ARGUMENT_TYPE_POSITIONAL) {
            printf(" <%s>", item->long_name);
        }
    }
    printf("\n");
    printf("%s\n", parser->description);
    printf("\n");
    printf("options:\n");

    for (size_t i = 0; i < parser->count; i++) {
        struct argument *item = &parser->items[i];

        switch (item->type) {
        case ARGUMENT_TYPE_POSITIONAL: {
            printf("  %c, %s\n", item->short_name, item->long_name);
            printf("      %s\n", item->description);
            printf("\n");
            break;
        }
        case ARGUMENT_TYPE_FLAG: {
            printf("  -%c, --%s\n", item->short_name, item->long_name);
            printf("      %s\n", item->description);
            printf("\n");
            break;
        }
        case ARGUMENT_TYPE_VALUE: {
            printf("  -%c, --%s <value>\n", item->short_name, item->long_name);
            printf("      %s\n", item->description);
            printf("\n");
            break;
        }
        default: {
            ARG_PANIC("invalid argument type");
        }
        }
    }
}

void argparse_print_version(struct argparse_parser *parser) {
    printf("%s %s\n", parser->name, parser->version);
}

void argparse_free(struct argparse_parser *parser) {
    free(parser->items);
    free(parser);
}

#endif // ARGPARSE_IMPLEMENTATION
