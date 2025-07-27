// This is a basic C example that uses `argparse.h` to parse command-line
// arguments. We will take a look at how to set up a parser, add arguments, and
// parse them.

// First you will need to define `ARGPARSE_IMPLEMENTATION` before including
// `argparse.h` to include the implementation in this file.
#define ARGPARSE_IMPLEMENTATION
#include "../argparse.h"

int main(int argc, char *argv[]) {
    Argparse_Parser parser = {0};

    // We initialize the parser with a name, description, and version.
    // This will be used to display help and version information.
    argparse_parser_init(&parser, "example", "An example program using argparse", "1.0");

    // We add a positional argument that requires a value.
    argparse_add_argument(&parser, (Argparse_Options){
        // Short name for the argument, this one is really needed in the case of
        // using flags, so for value arguments it does nothing, but still has
        // to be set.
        .short_name = 'n',
        // The long name for the argument will be used to get the value after parsing.
        .long_name = "name",
        // A description of the argument, this will be shown in the help message.
        .description = "Your name",
        // The type of the argument, in this case it is a value argument.
        .type = ARGUMENT_TYPE_POSITIONAL,
        // Whether this argument is required or not, in this case it is required.
        // Usually you would want to set this to 1 in case of positional arguments
        // but it is not strictly necessary.
        .required = 1
    });

    // Let's add a value argument that is optional and has a default value.
    argparse_add_argument(&parser, (Argparse_Options){
        .short_name = 'a',
        .long_name = "age",
        .description = "Your age",
        // This is a value argument, it requires a value to be set.
        .type = ARGUMENT_TYPE_VALUE,
        // This argument is not required, it has a default value.
        .required = 0
    });

    // We add a flag argument that does not require a value.
    argparse_add_argument(&parser, (Argparse_Options){
        .short_name = 'v',
        .long_name = "verbose",
        .description = "Enable verbose output",
        // A flag argument is a boolean argument with a default value of 0 (false).
        // if the flag is set, the value will be 1 (true).
        // You can use either the short name or the long name to set the flag.
        // In this case, `-v` or `--verbose` will enable verbose output.
        .type = ARGUMENT_TYPE_FLAG,
        // The required flag must be set to 0 for flag arguments, as they are optional.
        .required = 0
    });

    // We add a positional argument that can take multiple values.
    argparse_add_argument(&parser, (Argparse_Options){
        .short_name = 'f',
        .long_name = "files",
        .description = "Files to process",
        // This is a positional rest argument, it can take multiple values.
        // This will collect all remaining positional arguments after the
        // required positional arguments.
        // You cannot add other positional arguments after this one.
        .type = ARGUMENT_TYPE_POSITIONAL_REST,
        // We can set the required flag to 0, for this example
        .required = 0
    });

    // Now we parse the command line arguments.
    if (argparse_parse(&parser, argc, argv) != ARG_OK) {
        // If parsing fails, we can print the help message and exit.
        argparse_print_help(&parser);
        return 1;
    }

    // After parsing, we can get the values of the arguments.
    // We can be assured that the required positional argument is set,
    // otherwise the parse would have failed.
    char *name = argparse_get_value(&parser, "name");
    char *age= argparse_get_value_or_default(&parser, "age", "69");
    unsigned int verbose = argparse_get_flag(&parser, "verbose");
    char *files[ARGPARSE_CAPACITY] = {0};
    unsigned long file_count = argparse_get_values(&parser, "files", files);

    // Now we can use the values we got from the parser.
    printf("Hello, %s!\n", name);
    printf("Your age is: %s\n", age);
    if (verbose) {
        printf("Verbose output enabled.\n");
    }
    for (unsigned long i = 0; i < file_count; i++) {
        printf("File %lu: %s\n", i + 1, files[i]);
    }
}
