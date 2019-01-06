# CNRoom 1.1
CNRoom (Chats Noirs Room) is a header-only key-value type database library using C++17 and `std::filesystem`. It is actually in development by Fatih#6810 (accfldekur@gmail.com) from *Moonlit Black Cats*.
***

### How to use

CNRoom is header-only, you just need a compiler that supports C++17 and `std::filesystem` and to include `CNRoom/Room.hpp` after downloading files in the include directory. You may also need to link `libstdc++fs` to your project to use the `std::filesystem` library that CNRoom depends on.

**Library vocabulary**

I use the arrangement metaphor, you can imagine a corridor (base directory) with several rooms (directories), each rooms contains drawers (files), inside these drawers you can tidy up papers (keys) with informations written on (values). So you can enter in a room, open a drawer and take a paper to write on or to read.

**Example code**

An example code showing how CNRoom is easy to use!
```cpp
//CNRoom
#include "CNRoom/Room.hpp"

//Standard
#include <iostream>

int main()
{
    using namespace std::string_literals; //String literals (operator""s)

    try
    {
        CNRoom::Room room;

        //Connect to a base directory (optional, current path by default)
        room.connect("database", true);

        //Open a drawer
        room.open("lebgdu92.hkn", [](auto& stream)
        {
            auto& key = stream();

            //Write to the drawer
            stream << CNRoom::Key{"mail", {"lebgdu92@gmail.com"s, true}} << CNRoom::Key{"sword", {"Sword of the Warrior"s, false, 4.85, 0}};

            //Read from the drawer
            stream >> "sword";

            std::cout << std::boolalpha;

            for(const auto& it: key.values)
            {
                //Show with std::visit
                std::visit([](auto const& value){ std::cout << value << ' '; }, it);
                //Show by converting to string
                std::cout << key.string(it) << ' ';
            }

            //Show with get
            std::cout << std::get<std::string>(key.values[0]) << std::endl;

            //Remove a key in the drawer
            stream.remove("sword");
        }, true);
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
```

Created file's path: "database\players\lebgdu92.txt". Content:
```
mail:lebgdu92@gmail.com,true
sword:L'Epee du Guerrier,false,4.850000,100
```

***

### Help

Classes provided in `CNRoom/Room.hpp`. 

Class | Description
------- | -----------
`Key` | Struct that represents a key with a name and a vector of values.
`Status` | Status codes that may be returned by some functions.
`Room` | Main class for your database.

Different useful functions member of the classes above.

Functions | Description
------- | -----------
`Key::getValue(index)` | Convert a value to a string by its index.
`Key::getValue(value)` | Convert a value to a string.
`Room::getDefaultPath()` | Returns the default current path of the application.
`Room::defaultPath()` | Set current path to default, to call before using `std::fstream`.
`Room::connect(path)` | Set the base directory of all rooms and drawers. Current path by default so optional. 
`Room::enter(directory)` | Enter in a drawer, a sub directory, optional.
`Room::exit()` | Exit the actual room, optional: you can directly enter another room.
`Room::destroy(file)` | Delete a file.
`Room::open(file)` | Open a file, must call this function to use next ones.
`Room::close()` | Close the actual drawer, optional: you can directly open another drawer.
`Room::put(key)` | Put (write) a key to a drawer.
`Room::take(name)` | Take a key from the drawer.
`Room::read(name, index)` | Read a specific value of a key from the drawer.


