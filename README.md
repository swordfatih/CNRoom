# CNRoom
CNRoom (Chats Noirs Room) is a header-only key-value type database library using C++17 and `std::filesystem`. It is actually in development by Fatih#6810 (accfldekur@gmail.com) from *Moonlit Black Cats*.
***

### How to use

CNRoom is header-only, you just need a compiler that supports C++17 and `std::filesystem` and to include `CNRoom/Room.hpp` after downloading files in the include directory. You may also need to link `libstdc++fs` to your project to use the `std::filesystem` library that CNRoom depends on.

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
    
    CNRoom::Room room;

    //Connect to a base directory (optional, current path by default)
    if(room.connect("database", true) != CNRoom::Invalid)
    {
        //Enter in a room (optional)
        if(room.enter("players", true) != CNRoom::Invalid)
        {
            //Open a drawer (optional)
            if(room.open("lebgdu92.txt", true) != CNRoom::Invalid)
            {
                //Put files in the drawer
                room.put({"mail", {"lebgdu92@gmail.com"s, true}});

                CNRoom::Key sword{"sword", {"L'Epee du Guerrier"s, false, 4.85, 100}};
                room.put(sword);

                //Read files from the drawer

                //Method 1: Take whole key
                sword = room.take("sword");

                for(const auto& it: sword.values)
                {
                    //Converting values to string and showing them
                    std::cout << sword.getValue(it) << ' ';
                }

                //Method 2: Read a value by index
                auto&& mail = room.read("mail", 0); //Starting from 0

                if(auto value = std::get_if<std::string>(&mail))
                {
                    //Showing the value
                    std::cout << *value << std::endl;
                }
            }
        }
    }

    return 0;
}
```

Created file's path: "database\players\lebgdu92.txt"
Content:
```
mail:lebgdu92@gmail.com,true
sword:L'Epee du Guerrier,false,4.850000,100
```

***

### Help

`CNRoom/Room.hpp` file provides some useful tools. 

Class | Description
------- | -----------
`Key` | Struct that represents a key with a name and a vector of values.
`add<Component>(entity)` | Add a component to an entity.
`remove<Component>(entity)` | Remove a component from an entity.
`get<Component>(entity)` | Get a specified component of an entity.
`has<Component>(entity)` | Check if an entity has a specified component.
`erase(entity)` | Erase an entity.
`for_each<Component>(function)` | Execute a given function for each entities containing specified components.
`acquire<Component>()` | Get a vector of entities containing specified components.

***


