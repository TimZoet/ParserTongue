# ParserTongue
A modern C++20 argument parser.

* An unambiguous interface, both for the user of the command line and you, the developer.
* Parsing the arguments passed by the user will never throw an exception. Instead, all parse errors can be inspected 
  and you can decide whether and how the application continues.
* At the same time, constructing an invalid parser (with duplicate argument names or other such problems) will most 
  definitely throw exceptions to prevent nasty bugs.

3 types of values (flag, value, list). Anything that can be converted from string or stringstream can be used as value.
Nice help functions, etc.

## Parser
Step one is creating a `parser` object. The constructor takes the argument count and strings as parameters. After that,
you can add all flags, values, etc. that you need.

With the parser set up, just invoke the object. If an unexpected internal error occurred, the parser will return false
and the `what()`of whatever exception occurred. 

If the user requested the version information or help, you should not run your program. You can check this with the 
`display_help` method, which also prints out the requested information.

If the user passed invalid arguments (unknown flags, missing values after `=`, or anything else that could not be 
parsed) the error is recorded. All errors can be retrieved using the `errors` method. You could then inspect them, log 
them, display them to the user, etc.

```cpp
int main(int argc, char** argv)
{
    // Create a parser.
    auto parser = pt::parser(argc, argv);

    // Here you will create all flags, values, etc.
    ...
    
    // Run the parser.
    std::string e;
    if (!parser(e))
    {
        std::cout << "Internal parsing error: " << e << std::endl;
        return 0;
    }
    
    // User requested help or version, don't run.
    if (parser.display_help(std::cout))
        return 0;
    
    // There might have been non-fatal parse errors because the user passed invalid arguments.
    // You can inspect them, display them, or perhaps terminate the program.
    if (!parser.errors().empty())
        parser.display_errors(std::cout);
    
    // Do something with the parsed values and run your program.
    ...
}
```
Also, trying to retrieve any parsed value before invoking the parser, invoking the parser 
a second time, and adding new flags or changing parser properties after parsing will throw an exception.

If you want you can retrieve the full string that was passed by the user.
```cpp
std::string s = parser.full_string();
```
On practically all platforms the program name is the first value in `argv`. Just in case that isn't true for the 
platform you work with, there is an optional third parameter that, if set to true, will deal with that problem.
```cpp
auto parser = pt::parser(argc, argv, true);
```
## Argument Names
To all argument names (whether they are flags, values, or lists) the same set of naming rules applies. You can provide 
either a short or long name, or both. The short name must be a single alphabetic character. If you do not want the 
argument to have a short name, simply provide a `\0` character. The long name must start with an alphabetic character, 
be at least 2 characters long, and is allowed to contain underscores. For both the short and long name lower- and 
uppercase characters are allowed. The short names `v` and `h`, as well as the long names `version` and `help` are 
reserved.
```cpp
// Valid argument names.
auto flag0 = parser.flag('x', "longName");          // Argument with short and long name.
auto flag1 = parser.flag('y');                      // Argument with just a short name.
auto flag2 = parser.flag('\0', "longest_name");     // Argument without short name and underscores in long name.
                                                    
// These names will result in an exception.
auto badFlag = parser.flag('5');                    // Not alphabetic.
auto badFlag = parser.flag(..., "some5Name%");      // Not alphabetic + underscores.
auto badFlag = parser.flag('x');                    // Already in use.
auto badFlag = parser.flag(..., "longName");        // Already in use.
auto badFlag = parser.flag(..., "a");               // Length below 2.
auto badFlag = parser.flag('\0', "");               // Both names empty.
auto badFlag = parser.flag('h', "help");            // Reserved names.
```

## Flags
Flags are boolean arguments that are set when the user passes their short or long name. They can be added using the 
`flag` method which returns a pointer to access the state after parsing:
```cpp
auto flagX = parser.flag('x', "longName");
auto flagY = parser.flag('y');
```
Checking if a flag was set is straightforward:
```cpp
if (flagX->is_set()) std::cout << "Flag x was set\n";
if (flagY->is_set()) std::cout << "Flag y was set\n";
```
It is not a problem if the user passes a flag multiple times, whether that is using the short or long name. Also, flags
can be chained together and passed in any order. The following all results in the same output:
```
> app -x -y
*or*
> app --longName -y
*or*
> app -xy
*or*
> app -y -x -y -x -y --longName -yxyxxyyx
Flag x was set
Flag y was set
```

## Values
Values are arguments of any type that can be converted from a string. They can be added using the `value` method which
takes a template parameter defining what the string value is parsed as:
```cpp
auto stringValue = parser->value<std::string>('s');
auto intValue    = parser->value<int>('i', "integer");
```
Before getting the value you must first verify that it was set:
```cpp
std::cout << (stringValue->is_set() ? stringValue->get_value() : "String was not set" ) << std::endl;
std::cout << (intValue->is_set()    ? intValue->get_value()    : "Integer was not set") << std::endl;
```
Setting a value multiple times will result in the last parsed value being returned:
```
> app -i 42
*or*
> app -i=42
*or*
> app --integer 42
*or*
> app --integer 313 --integer=42
String was not set
42
```
You can create a value argument of any type that satisfies the `parsable` concept, which requires the type to be
convertible from string or readable from a stringstream:
```cpp
template<typename T>
concept parsable = std::convertible_to<T, std::string> || requires(T val, std::stringstream s)
{
    {s >> val};
};
```
With the `set_default` method you can assign a default value that is returned when the user assigns none:
```cpp
intValue->set_default(42);
```
```
> app
String was not set
42
```
Passing values containing whitespace is handled automatically by the command line application when you use quotes (or 
at least, it should be): 
```
> app -s "foo bar"
*or*
> app -s="foo bar"
foo bar
Integer was not set
```

## Lists
Lists are arguments to which more than one value can be assigned. They can be added using the `list` method 
which takes a template parameter defining what the string value is parsed as:

```cpp
auto files = parser->list<std::string>('f', "files");
```
Before getting the values you must first verify that anything was set:
```cpp
if (files->is_set())
{
    for (const auto& f : files->get_value())
        std::cout << f << std::endl;
}
```
When setting values with an `=` after the argument name, you must use the correct delimiter to ensure they are added to
the argument. When not using `=`, all whitespace-separated values after an argument name but before the next argument 
name are added to the list.
```
> app -f foo.txt bar.txt
*or*
> app -f=foo.txt,bar.txt
*or*
> app --files foo.txt bar.txt --someOtherArg foo
*or*
> app --files=foo.txt,bar.txt
foo.txt
bar.txt
```
You can override the delimiter using the `set_delimiter` method:
```cpp
files->set_delimiter(';');
```
```
> app --files=foo.txt;bar.txt
foo.txt
bar.txt
```
Whitespace should again be handled automatically by the command line:
```
> app --files="path/with space/foo.txt;bar.txt"
path/with space/foo.txt
bar.txt
```

## Operands
Operands are all values passed by the user that do not start with a `-` and are not assigned to an argument. They can be
retrieved with the `operands` method:
```cpp
for (const auto& op : parser.operands())
    std::cout << op << std::endl;
```
Operands are not automatically converted to any type. You can only retrieve them as strings.
```
> app op0 --files=foo.txt op1 op2 -f op3
op0
op1
op2
op3
```

## Version Info
There are several properties you can set that are displayed when the user requests version information: `name`, 
`version` and `description`. As stated before, you should end the program when the user requests the version:
```cpp
// Create parser, flags etc.
...

// Set version info.
parser.name("My App");
parser.version("3.0");
parser.description("A short description of what the application does.");

// Run parser.
...

// If version is requested, terminate.
if (parser.display_help(std::cout))
    return 0;
```
The version information can be displayed in the usual way:
```
> app -v
*or*
> app --version
*or*
> app version
My App
3.0
A short description of what the application does.
```
When one of the properties is not set, it is not printed. When none of the properties are set, nothing is printed. All
arguments after the version argument are ignored, nothing is parsed: hence why you should not continue the program.

## Help
You can set a help string on each argument that is displayed when the user requests the help. You can also provide an 
optional longer help string. This string is displayed instead of the short string when the user requests the help for
a specific argument: 
```cpp
auto flag = parser.flag('f', "flag");
auto list = parser.list('\0', "filenames");
flag->help("Short help for flag", "Longer, more detailed help for flag");
list->help("Short help for filenames");
```
The user can request the help in a similar manner as the version information:
```
> app -h
*or*
> app --help
*or*
> app help
Available arguments:
-f --flag           Short help for flag
--filenames         Short help for filenames
```
You can control the width of the two columns using the parameters of the `display_help` method:
```
if (parser.display_help(std::cout), 20, 60)
    return 0;
```
The user can request the help for a specific argument by passing its name. For convenience, the name can be in any form: 
short, long, with or without preceding dash(es).If no long help string was set, the short help string is displayed.
```
> app help f
*or*
> app help -f
*or*
> app help flag
*or*
> app help --flag
Longer, more detailed help for flag
```