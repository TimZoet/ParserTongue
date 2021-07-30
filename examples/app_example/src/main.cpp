#include <iostream>

#include "parsertongue/parser.h"

int main(int argc, char** argv)
{
    // Create parser.
    auto parser = pt::parser(argc, argv);

    // Set application info.
    parser.set_name("My App");
    parser.set_version("1.42");
    parser.set_description("Some short description");

    // Add some flag arguments.
    auto flag0 = parser.add_flag('a', "longName");
    auto flag1 = parser.add_flag('b');

    // Add some value arguments.
    auto value0 = parser.add_value<int32_t>('x', "valueX");
    auto value1 = parser.add_value<float>('y');
    value0->add_options(10, 100, 1000);
    value1->set_default(33.33f);

    // Add some list arguments.
    auto list0 = parser.add_list<std::string>('f', "filenames");
    auto list1 = parser.add_list<double>('d', "doubles");

    // Set help on some of the arguments.
    flag0->set_help("This flag does something", "Long help");
    flag1->set_help("This flag does something else");
    value0->set_help("A very very very very very very very very long help string for this value");

    // Run the parser and test for unexpected errors.
    std::string e;
    if (!parser(e))
    {
        std::cout << "Internal parsing error: " << e << std::endl;
        return 0;
    }

    // If help was requested, print to console and terminate.
    if (parser.display_help(std::cout)) return 0;

    // If there were parse errors, print to console.
    if (!parser.get_errors().empty()) parser.display_errors(std::cout);

    // Print parsed arguments.
    if (flag0->is_set()) std::cout << "Flag a was set\n";
    if (flag1->is_set()) std::cout << "Flag b was set\n";
    if (value0->is_set()) std::cout << "Value x = " << value0->get_value() << "\n";
    if (value1->is_set()) std::cout << "Value y = " << value1->get_value() << "\n";
    if (list0->is_set())
    {
        std::cout << "List 0 = \n";
        for (const auto& v : list0->get_values()) std::cout << "  " << v << std::endl;
    }
    if (list1->is_set())
    {
        std::cout << "List 1 = \n";
        for (const auto& v : list1->get_values()) std::cout << "  " << v << std::endl;
    }
    if (!parser.get_operands().empty())
    {
        std::cout << "Operands = \n";
        for (const auto& o : parser.get_operands()) std::cout << "  " << o << std::endl;
    }

    return 0;
}
