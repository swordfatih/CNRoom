/////////////////////////////////////////////////////////////////////////////////
//
// CNRoom - Chats Noirs Locker-room database
// Copyright (c) 2019 Fatih (accfldekur@gmail.com)
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////

#ifndef ROOM_HPP
#define ROOM_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
//Standard
#include <filesystem>
#include <fstream>
#include <vector>
#include <variant>
#include <string>

namespace CNRoom
{

////////////////////////////////////////////////////////////
using Types = std::variant<std::string, int, double, bool>; ///< Value types

////////////////////////////////////////////////////////////
/// \brief Struct that represents a key
///
////////////////////////////////////////////////////////////
struct Key
{
    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    std::string name;
    std::vector<Types> values;

    ////////////////////////////////////////////////////////////
    /// \brief Convert value to string by index
    ///
    /// \param value Index of the to convert
    ///
    /// \return Converted value
    ///
    ////////////////////////////////////////////////////////////
    std::string getValue(const size_t& index) const
    {
        std::string converted;

        if(index < values.size())
        {
            auto& value = values[index];

            if(std::holds_alternative<std::string>(value))
            {
                converted = std::get<std::string>(value);
            }
            else if(std::holds_alternative<int>(value))
            {
                converted = std::to_string(std::get<int>(value));
            }
            else if(std::holds_alternative<double>(value))
            {
                converted = std::to_string(std::get<double>(value));
            }
            else
            {
                converted = std::get<bool>(value) ? "true" : "false";
            }
        }

        return converted;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Convert value to string
    ///
    /// \param value Value to convert
    ///
    /// \return Converted value
    ///
    ////////////////////////////////////////////////////////////
    std::string getValue(const Types& value) const
    {
        std::string converted;

        if(std::holds_alternative<std::string>(value))
        {
            converted = std::get<std::string>(value);
        }
        else if(std::holds_alternative<int>(value))
        {
            converted = std::to_string(std::get<int>(value));
        }
        else if(std::holds_alternative<double>(value))
        {
            converted = std::to_string(std::get<double>(value));
        }
        else
        {
            converted = std::get<bool>(value) ? "true" : "false";
        }

        return converted;
    }
};

////////////////////////////////////////////////////////////
/// \brief Enumeration of the status codes that may be
/// returned by some functions.
///
////////////////////////////////////////////////////////////
enum Status
{
    Done,       ///< Successfully done
    None,       ///< File doesn't exist
    Invalid,    ///< Invalid path
    Unset       ///< File not set
};

////////////////////////////////////////////////////////////
/// \brief Class for a locker-room type database based on
/// files
///
////////////////////////////////////////////////////////////
class Room
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ////////////////////////////////////////////////////////////
            Room(): mPrime(std::filesystem::current_path()), mBase(mPrime), mRoom(mPrime), mDrawer("")
    {
        //ctor
    }

    ////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    ////////////////////////////////////////////////////////////
            ~Room()
    {
        //dtor
    }

    ////////////////////////////////////////////////////////////
    /// \brief Get default current path
    ///
    /// \return Default path
    ///
    ////////////////////////////////////////////////////////////
    std::filesystem::path getDefaultPath() const
    {
        return mPrime;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Set current path to default
    ///
    ////////////////////////////////////////////////////////////
    void defaultPath()
    {
        std::filesystem::current_path(mPrime);
    }

    ////////////////////////////////////////////////////////////
    /// \brief Set the base directory, current path by default
    ///
    /// \param directory Path to the directory, current path by
    /// default
    /// \param create If true and directory doesn't exist create
    /// a new one, false by default
    ///
    /// \return Status of the action
    ///
    ////////////////////////////////////////////////////////////
    Status  connect(const std::filesystem::path& directory = std::filesystem::current_path(), bool create = false)
    {
        Status status = Status::None;

        if(std::filesystem::exists(directory))
        {
            if(std::filesystem::is_directory(directory))
            {
                mBase = std::filesystem::absolute(directory);
                status = Status::Done;
            }
            else
            {
                status = Status::Invalid;
            }
        }
        else if(create)
        {
            std::filesystem::create_directories(directory);

            mBase = std::filesystem::absolute(directory);
            status = Status::Done;
        }

        return status;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Save a directory which contains files or create one
    ///
    /// \param directory Path to the directory
    /// \param create If true and directory doesn't exist create
    /// a new one, false by default
    ///
    /// \return Status of the action
    ///
    ////////////////////////////////////////////////////////////
    Status  enter(const std::filesystem::path& directory, bool create = false)
    {
        Status status = Status::None;

        std::filesystem::current_path(mBase);

        if(std::filesystem::exists(directory))
        {
            if(std::filesystem::is_directory(directory))
            {
                mRoom = std::filesystem::absolute(directory);
                status = Status::Done;
            }
            else
            {
                status = Status::Invalid;
            }
        }
        else if(create)
        {
            std::filesystem::create_directories(directory);

            mRoom = std::filesystem::absolute(directory);
            status = Status::Done;
        }

        return status;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Exit the actual room
    ///
    /// \return Status of the action
    ///
    ////////////////////////////////////////////////////////////
    Status exit()
    {
        Status status = Status::None;

        if(mRoom != mBase)
        {
            mRoom = mBase;
            status = Status::Done;
        }

        return status;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Destroy a drawer
    ///
    /// \param file Path to the file
    ///
    /// \return Status of the action
    ///
    ////////////////////////////////////////////////////////////
    Status  destroy(const std::filesystem::path& file)
    {
        Status status = Status::None;

        if(!mRoom.empty())
        {
            std::filesystem::current_path(mRoom);
        }
        else
        {
            std::filesystem::current_path(mBase);
        }

        if(std::filesystem::exists(file))
        {
            if(std::filesystem::is_regular_file(file))
            {
                std::filesystem::remove(file);
                status = Status::Done;
            }
            else
            {
                status = Status::Invalid;
            }
        }

        return status;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Save a file which contains keys or create one
    ///
    /// \param file Path to the file
    /// \param create If true and file doesn't exist create a new
    /// one, false by default
    ///
    /// \return Status of the action
    ///
    ////////////////////////////////////////////////////////////
    Status  open(const std::filesystem::path& file, bool create = false)
    {
        Status status = Status::None;

        std::filesystem::current_path(mRoom);

        if(std::filesystem::exists(file))
        {
            if(std::filesystem::is_regular_file(file))
            {
                mDrawer = std::filesystem::absolute(file);
                status = Status::Done;
            }
            else
            {
                status = Status::Invalid;
            }
        }
        else if(create)
        {
            std::ofstream writer(file);

            if(writer)
            {
                mDrawer = std::filesystem::absolute(file);
                status = Status::Done;
            }
            else
            {
                status = Status::Invalid;
            }
        }

        return status;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Close the opened drawer
    ///
    /// \return Status of the action
    ///
    ////////////////////////////////////////////////////////////
    Status close()
    {
        Status status = Status::None;

        if(!mDrawer.empty())
        {
            mDrawer = mRoom;
            status = Status::Done;
        }

        return status;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Put a key to the opened drawer
    ///
    /// \param key Key to write
    ///
    /// \return Status of the action
    ///
    ////////////////////////////////////////////////////////////
    Status  put(const Key& key)
    {
        Status status = Status::Invalid;

        if(std::filesystem::exists(mDrawer))
        {
            std::ifstream observer(mDrawer);

            if(observer)
            {
                std::vector<std::string> fields;

                std::string field;
                while(std::getline(observer, field))
                {
                    auto pos = field.find_first_of(':');

                    if(key.name != field.substr(0, pos))
                    {
                        fields.push_back(field);
                    }
                }

                observer.close();

                std::ofstream writer(mDrawer, std::ios_base::trunc);

                for(const auto& it: fields)
                {
                    writer << it << '\n';
                }

                writer << key.name << ':';

                for(size_t i = 0; i < key.values.size(); ++i)
                {
                    writer << key.getValue(i);

                    if(i != key.values.size() - 1)
                    {
                        writer << ',';
                    }
                }

                status = Status::Done;
            }
            else
            {
                status = Status::Invalid;
            }
        }

        return status;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Take a key from the opened drawer
    ///
    /// \param name Name of the key to read
    ///
    /// \return Key to read
    ///
    ////////////////////////////////////////////////////////////
    Key     take(const std::string& name)
    {
        Key key{name, {}};

        if(std::filesystem::exists(mDrawer))
        {
            std::ifstream observer(mDrawer);

            if(observer)
            {
                std::string field;

                bool done = false;
                while(std::getline(observer, field) && !done)
                {
                    if(name == field.substr(0, field.find_first_of(':')))
                    {
                        std::string values = field.substr(field.find_first_of(':') + 1);

                        size_t last = 0;
                        while(!done)
                        {
                            auto pos = values.find_first_of(',', last);

                            std::string token = values.substr(last, pos - last);

                            if(token.empty())
                            {
                                key.values.push_back(std::string(""));
                            }
                            else if(token == "true")
                            {
                                key.values.push_back(true);
                            }
                            else if(token == "false")
                            {
                                key.values.push_back(false);
                            }
                            else
                            {
                                bool digit = true;
                                bool integer = true;

                                if(token.front() != '.')
                                {
                                    for(const auto& character: token)
                                    {
                                        if(!std::isdigit(character))
                                        {
                                            if(character != '.')
                                            {
                                                digit = false;
                                            }
                                            else
                                            {
                                                integer = false;
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    digit = false;
                                }

                                if(digit)
                                {
                                    if(integer)
                                    {
                                        key.values.push_back(std::stoi(token));
                                    }
                                    else
                                    {
                                        key.values.push_back(std::stod(token));
                                    }
                                }
                                else
                                {
                                    key.values.push_back(token);
                                }
                            }

                            if(pos != std::string::npos)
                            {
                                last = pos + 1;
                            }
                            else
                            {
                                done = true;
                            }
                        }
                    }
                }

                observer.close();
            }
        }

        return key;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Read a value of a key from the opened drawer
    ///
    /// \param name Name of the key to read
    /// \param index Index of the value to read
    ///
    /// \return Value of the index
    ///
    ////////////////////////////////////////////////////////////
    Types   read(const std::string& name, const size_t& index = 0)
    {
        Types value = std::string("");

        if(std::filesystem::exists(mDrawer))
        {
            std::ifstream observer(mDrawer);

            if(observer)
            {
                std::string field;

                bool done = false;
                while(std::getline(observer, field) && !done)
                {
                    if(name == field.substr(0, field.find_first_of(':')))
                    {
                        std::string values = field.substr(field.find_first_of(':') + 1);

                        unsigned int lap = 0;

                        size_t last = 0;
                        while(!done)
                        {
                            auto pos = values.find_first_of(',', last);

                            std::string token = values.substr(last, pos - last);

                            if(lap == index)
                            {
                                if(token.empty())
                                {
                                    value = std::string("");
                                }
                                else if(token == "true")
                                {
                                    value = true;
                                }
                                else if(token == "false")
                                {
                                    value = false;
                                }
                                else
                                {
                                    bool digit = true;
                                    bool integer = true;

                                    if(token.front() != '.')
                                    {
                                        for(const auto& character: token)
                                        {
                                            if(!std::isdigit(character))
                                            {
                                                if(character != '.')
                                                {
                                                    digit = false;
                                                }
                                                else
                                                {
                                                    integer = false;
                                                }
                                            }
                                        }
                                    }
                                    else
                                    {
                                        digit = false;
                                    }

                                    if(digit)
                                    {
                                        if(integer)
                                        {
                                            value = std::stoi(token);
                                        }
                                        else
                                        {
                                           value = std::stod(token);
                                        }
                                    }
                                    else
                                    {
                                        value = token;
                                    }
                                }

                                done = true;
                            }

                            if(pos != std::string::npos)
                            {
                                last = pos + 1;
                            }
                            else
                            {
                                done = true;
                            }

                            lap++;
                        }
                    }
                }

                observer.close();
            }
        }

        return value;
    }

protected:

private:
    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    const std::filesystem::path mPrime;     ///< Default path
    std::filesystem::path       mBase;      ///< Path to the base directory
    std::filesystem::path       mRoom;      ///< Path to the actual room
    std::filesystem::path       mDrawer;    ///< Path to the opened drawer
};

} // namespace CNRoom

#endif // ROOM_HPP
