# CNRoom 1.1
CNRoom (Chats Noirs Room) is a header-only key-value type database library using C++17 and `std::filesystem`. It is actually in development by Fatih#6810 (accfldekur@gmail.com) from *Moonlit Black Cats*.
***

### How to use

CNRoom is header-only, you just need a compiler that supports C++17 and `std::filesystem` and to include `CNRoom/Room.hpp` after downloading files in the include directory. You may also need to link `libstdc++fs` to your project to use the `std::filesystem` library that CNRoom depends on.

**Library vocabulary**

I use the arrangement metaphor, you can imagine a room (base directory) with several drawers (files), inside these drawers you can tidy up papers (keys) with informations written on (values). 

**Example code**

An example code showing how CNRoom is easy to use!
```cpp
//CNRoom
#include "CNRoom/Room.hpp"

//Standard
#include <iostream>

int main()
{
    using namespace std::string_literals; ///< String literals (operator""s)

    ////////////////////////////////////////////////////////////
    /// Stream class
    ////////////////////////////////////////////////////////////
    CNRoom::Stream stream("database/lebgdu92.hkn", true); ///< Open the file pointed by the path, create new if doesn't exist

    stream << CNRoom::Key{"mail", {"lebgdu92@gmail.com"s, true}}; ///< Write to the stream with operator<<
    stream >> "mail"; ///< Read from the stream with operator>>
    std::cout << std::get<std::string>(stream().values[0]) << std::endl; ///< Retreive key with Stream::operator() and std::get
    stream.remove("mail"); ///< Remove a key

    ////////////////////////////////////////////////////////////
    /// Room helper class
    ////////////////////////////////////////////////////////////
    CNRoom::Room room;

    room.connect("database", true); ///< Set a base directory (optional, current path by default)

    room.open("lebgdu92.hkn", [](auto& stream) ///< Open a file
    {
        stream.write(CNRoom::Key{"sword", {"Sword of the Warrior"s, false, 4.85, 0}}); ///< Write to the stream with function

        for(const auto& it: stream.read("sword").values) ///< Read from the stream with function
        {
            std::visit([](auto const& value){ std::cout << value << ' '; }, it); ///< Visit the values to show them
        }

    }, true);

    room.destroy("lebgdu92.hkn"); ///< Delete a file

    ////////////////////////////////////////////////////////////
    /// Shortcut functions
    ////////////////////////////////////////////////////////////
    room.quick_write("lebgdu92.hkn", {"sword", {"Sword of the Warrior"s, false, 4.85, 0}}); ///< Write shortcut
    auto key = room.quick_read("lebgdu92.hkn", "sword"); ///< Read shortcut
    std::cout << key.string(key.values[2]) << std::endl; ///< Convert value to string
    std::cout << std::get<bool>(room.quick_value("lebgdu92.hkn", "sword", 1)) << std::endl; ///< Retreive value shortcut

    return 0;
}
```

Output:
```
Sword of the Warrior Sword of the Warrior false false 4.85 4.850000 0 0 Sword of the Warrior
```

Created file: "database\lebgdu92.txt". 

Content:
```
mail:"lebgdu92@gmail.com",true
```

***

### Help

Classes provided in `CNRoom/Room.hpp`. 

Class | Description
------- | -----------
`Key` | Struct that represents a key with a name and a vector of values.
`Stream` | Stream class to operate files.
`Room` | Stream wrapper class for your database.

Different useful functions member of the classes above.

Functions | Description
------- | -----------
`Key::string(index)` | Convert a value to a string.
`Stream::operator<<` | Write a key to the stream.
`Stream::operator>>` | Read a key from the stream by its name.
`Stream::operator()` | Returns a reference on the output key.
`Stream::write(key)` | Write a key to the stream.
`Stream::read(name)` | Read a key from the stream with its name.
`Room::connect(path)` | Set the base directory of all drawers. Current path by default so optional. 
`Room::open(file, function)` | Open a drawer and call an operation function.
`Room::destroy(file)` | Delete a drawer.
`Room::quick_write(file, key)` | Helper function, short way to write a key in a drawer.
`Room::quick_read(file, name)` | Helper function, short way to read a key from a drawer.
`Room::quick_value(file, name, index)` | Helper function, short way to retrieve a value of a key from a drawer.


