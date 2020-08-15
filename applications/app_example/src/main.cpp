#include <iostream>

#include "parsertongue/parser.h"

#include <thread>


int main(int argc, char** argv)
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    auto parser = pt::parser(argc, argv);

    parser.name("My App");
    parser.version("1.42");
    parser.description("Some short description");

    auto flag0  = parser.flag('f', "longName");
    auto flag1  = parser.flag('t');
    auto value0 = parser.value<int32_t>('x', "valueX");
    auto value1 = parser.value<float>('y');
    auto list0  = parser.list<std::string>('l', "filenamesfilenamesfilenames");
    auto list1  = parser.list<float>('w', "ft");
    list0->set_delimiter(';');

    flag0->help("This flag does something", "Long help");
    flag1->help("This flag does something else");
    value0->help("A very very very very very very very very long help string for this value");
    list0->help("A very very very very very very very very long help string for this value");

    std::string e;
    if (!parser(e))
    {
        std::cout << "Internal parsing error: " << e << std::endl;
        return 0;
    }

    if (parser.display_help(std::cout)) return 0;

    if (!parser.errors().empty()) parser.display_errors(std::cout);

    if (flag0->is_set()) std::cout << "Flag f was set\n";
    if (flag1->is_set()) std::cout << "Flag t was set\n";
    if (value0->is_set()) std::cout << "Value x = " << value0->get_value() << "\n";
    if (value1->is_set()) std::cout << "Value y = " << value1->get_value() << "\n";
    if (list0->is_set())
    {
        std::cout << "List 0 = ";
        for (const auto& v : list0->get_values()) std::cout << v << std::endl;
    }
    if (list1->is_set())
    {
        std::cout << "List 1 = ";
        for (const auto& v : list1->get_values()) std::cout << v << std::endl;
    }
    for (const auto& o : parser.operands()) std::cout << o << std::endl;
    return 0;
}
