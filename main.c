#define ARGPARSE_IMPLEMENTATION
#include "argparse.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct argparse_parser *parser =
        argparse_new("test", "test description", "0.0.1");
    argparse_add_argument(parser, 'x', "number", "number",
                          ARGUMENT_TYPE_VALUE);
    argparse_add_argument(parser, 'f', "file", "file name",
                          ARGUMENT_TYPE_POSITIONAL);

    argparse_parse(parser, argc, argv);

    char *number = argparse_get_value(parser, "number");
    char *file = argparse_get_value(parser, "file");

    if (number) {
        printf("number: %s\n", number);
    }

    printf("file: %s\n", file);

    argparse_free(parser);
    return 0;
}
