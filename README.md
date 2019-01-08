# CNRoom 1.1
CNRoom (Chats Noirs Room) is a header-only key-value type database library using C++17 (`std::filesystem` and `std::variant`). It is actually in development by Fatih#6810 (accfldekur@gmail.com) from *Moonlit Black Cats*.
***

### How to use

CNRoom is header-only, you just need a compiler that supports C++17 (`std::filesystem` and `std::variant`) and to include `CNRoom/Room.hpp` after downloading files in the include directory. You may also need to link `libstdc++fs` to your project in order to use the `std::filesystem` library that CNRoom uses.

**Library vocabulary**

I use the arrangement metaphor, you can imagine a room (base directory) with several drawers (files), inside these drawers you can tidy up papers (keys) with informations written on (values). 

**Example code**

An example/tutorial code showing how CNRoom is easy to use!
```cpp
//CNRoom
#include "CNRoom/Room.hpp"

//Standard
#include <iostream>

int main()
{
    using namespace std::string_literals; ///< String literals (operator""s)

    /// Stream class
    ////////////////////////////////////////////////////////////
    CNRoom::Stream stream("database/lebgdu92.hkn", true); ///< Open a file, create new if doesn't exist

    stream << CNRoom::Key{"mail", {"lebgdu92@gmail.com"s, true}}; ///< Write using <<
    stream >> "mail"; ///< Read using >>

    //Show method 1: std::get<Type>
    std::cout << std::get<std::string>(stream()[0]) << std::endl; ///< Retreive key using () and value using []

    stream.remove("mail"); ///< Remove a key

    /// Room helper class
    ////////////////////////////////////////////////////////////
    CNRoom::Room room;

    room.connect("database", true); ///< Base directory (current path by default)

    room.open("lebgdu92.hkn", [](auto& stream) ///< Open a file
    {
        stream.write({"sword", {"Sword of the Warrior"s, false, 4.85, 0}}); ///< Write using function write()

        for(const auto& it: stream.read("sword").values) ///< Read using function read()
        {
            //Show method 2: Visit
            std::visit([](auto const& value){ std::cout << value << ' '; }, it);
        }

    }, true);

    room.destroy("lebgdu92.hkn"); ///< Delete a file

    /// Shortcut functions
    ////////////////////////////////////////////////////////////
    room.quick_write("lebgdu92.hkn", {"sword", {"Sword of the Warrior"s, false, 4.85, 0}});

    //Show method 3: Convert to string using Key::string()
    std::cout << CNRoom::Key::string(room.quick_read("lebgdu92.hkn", "sword")[2]) << std::endl;

    return 0;
}
```

Output:
```
lebgdu92@gmail.com
Sword of the Warrior 0 4.85 0 4.850000
```

***

### Help

Struct and classes provided in `CNRoom/Room.hpp`. 

Struct & members | Description
------- | -----------
`Key` | Struct that represents a key with a name and a vector of values.
`static Key::string(value)` | Convert value to std::string.
`Key::name` | Name of the key.
`Key::values` | Vector of variant<string, int, double, bool>, access values with std::get, std::visit or Key::string.
`Key::operator[]` | Access value by index.

Class & members | Description
------- | -----------
`Stream` | Stream class to operate on files.
`Stream::operator()` | Returns a reference to the internal key.
`Stream::write(key)` | Write a key.
`Stream::operator<<` | Write a key.
`Stream::read(name)` | Read a key by name.
`Stream::operator>>` | Read a key by name.

Some helper functions are provided in the Room class.

Class & members | Description
------- | -----------
`Room` | Stream wrapper class.
`Room::connect(path)` | Set the base directory. Optional, current path by default. 
`Room::open(file, function)` | Opens a file and call the given function.
`Room::destroy(file)` | Delete a file.
`Room::quick_write(file, key)` | Short way to write a key.
`Room::quick_read(file, name)` | Short way to read a key.




