#define ARGPARSE_IMPLEMENTATION
#include "argparse.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    int result = 0;
    struct argparse_parser *parser =
        argparse_new("test", "test description", "0.0.1");
    argparse_add_argument(parser, (struct argparse_options){
                                      .short_name = 'x',
                                      .long_name = "number",
                                      .description = "some number input",
                                      .type = ARGUMENT_TYPE_VALUE,
                                      .required = 1,
                                  });
    argparse_add_argument(parser, (struct argparse_options){
                                      .short_name = 'c',
                                      .long_name = "config",
                                      .description = "config file",
                                      .type = ARGUMENT_TYPE_POSITIONAL,
                                      .required = 1,
                                  });
    argparse_add_argument(parser, (struct argparse_options){
                                      .short_name = 'f',
                                      .long_name = "file",
                                      .description = "just a file",
                                      .type = ARGUMENT_TYPE_POSITIONAL,
                                      .required = 0,
                                  });
    argparse_add_argument(parser, (struct argparse_options){
                                      .short_name = 'o',
                                      .long_name = "output",
                                      .description = "optional output file",
                                      .type = ARGUMENT_TYPE_VALUE,
                                      .required = 0,
                                  });

    if (argparse_parse(parser, argc, argv) != 0) {
        return_defer(1);
    }

    char *number = argparse_get_value(parser, "number");
    char *file = argparse_get_value(parser, "file");

    if (number) {
        printf("number: %s\n", number);
    }

    printf("file: %s\n", file);

defer:
    argparse_free(parser);
    return result;
}
