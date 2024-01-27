#define ARGPARSE_IMPLEMENTATION
#include "argparse.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct argparse_parser *parser =
        argparse_new("test", "test description", "0.0.1");
    argparse_add_argument(parser, 'v', "version", "print version",
                          ARGUMENT_TYPE_FLAG);
    argparse_add_argument(parser, 'h', "help", "print help",
                          ARGUMENT_TYPE_FLAG);
    argparse_add_argument(parser, 'f', "file", "file name",
                          ARGUMENT_TYPE_VALUE);

    argparse_parse(parser, argc, argv);

    unsigned int version = argparse_get_flag(parser, "version");
    unsigned int help = argparse_get_flag(parser, "help");
    char *file = argparse_get_value(parser, "file");

    if (version) {
        argparse_print_version(parser);

        argparse_free(parser);
        return 0;
    }

    if (help) {
        argparse_print_help(parser);

        argparse_free(parser);
        return 0;
    }

    if (file) {
        printf("file: %s\n", file);
    }

    argparse_free(parser);
    return 0;
}
