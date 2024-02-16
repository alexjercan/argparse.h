#define ARGPARSE_IMPLEMENTATION
#include "argparse.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct argparse_parser *parser =
        argparse_new("test", "test description", "0.0.1");
    argparse_add_argument(parser, (struct argparse_options) {
        .short_name = 'x',
        .long_name = "number",
        .description = "some number input",
        .type = ARGUMENT_TYPE_VALUE,
        .required = 1,
    });
    argparse_add_argument(parser, (struct argparse_options) {
        .short_name = 'f',
        .long_name = "file",
        .description = "just a file",
        .type = ARGUMENT_TYPE_POSITIONAL,
        .required = 1,
    });
    argparse_add_argument(parser, (struct argparse_options) {
        .short_name = 'o',
        .long_name = "output",
        .description = "optional output file",
        .type = ARGUMENT_TYPE_VALUE,
        .required = 0,
    });

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
