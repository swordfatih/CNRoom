/////////////////////////////////////////////////////////////////////////////////
//
// CNRoom - Chats Noirs Room
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
#include <exception>
#include <filesystem>
#include <functional>
#include <vector>
#include <variant>
#include <fstream>
#include <string>

namespace CNRoom
{

////////////////////////////////////////////////////////////
/// \brief Class inheriting from std::variant and internally
/// converting const char* to std::string if needed
///
////////////////////////////////////////////////////////////
template <typename Type, typename... Types>
class Variant : public std::variant<Type, Types ...>
{
public:
    using std::variant<Type, Types ...>::variant;
    using std::variant<Type, Types ...>::operator=;

    ///////////////////////////////////////////////////////////
    /// \brief Check if T is same as one of the types in the parameter
    /// pack
    ///
    /// \return Value
    ///
    ////////////////////////////////////////////////////////////
    template<typename T, typename... Ts>
    static constexpr bool contains()
    {
        return std::disjunction_v<std::is_same<T, Ts>...>;
    }

    ///////////////////////////////////////////////////////////
    /// \brief Overload of operator = to convert const char* to
    /// std::string if needed before assigning
    ///
    ////////////////////////////////////////////////////////////
    constexpr Variant& operator =(const char* character)
    {
        if constexpr(!contains<const char*, Type, Types...>() && contains<std::string, Type, Types...>())
        {
            std::variant<Type, Types ...>::operator=(std::string(character));
        }
        else
        {
            std::variant<Type, Types ...>::operator=(character);
        }

        return *this;
    }

    ///////////////////////////////////////////////////////////
    /// \brief Constructor that converts const char* to
    /// std::string if needed before assigning
    ///
    ////////////////////////////////////////////////////////////
    constexpr Variant(const char* character) : std::variant<Type, Types...>()
    {
        *this = character;
    }

    ///////////////////////////////////////////////////////////
    /// \brief Convert to the standard variant
    ///
    /// \return Standard variant
    ///
    ////////////////////////////////////////////////////////////
    std::variant<Type, Types ...> standard() const
    {
        return std::variant<Type, Types ...>(*this);
    }
};

////////////////////////////////////////////////////////////
using Types = Variant<std::string, int, double, bool>; ///< Value types

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

    ///////////////////////////////////////////////////////////
    /// \brief Overload of operator [] to retreive a value by index
    ///
    /// \param index Index of the value
    ///
    /// \return Value
    ///
    ////////////////////////////////////////////////////////////
    Types&      operator [](const size_t& index)
    {
        return values[index];
    }

    ////////////////////////////////////////////////////////////
    /// \brief Convert value to string
    ///
    /// \param value Value to convert
    ///
    /// \return Converted value
    ///
    ////////////////////////////////////////////////////////////
    static std::string string(const Types& value)
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
/// \brief Stream class to operate files
///
////////////////////////////////////////////////////////////
class Stream
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Construct an empty stream
    ///
    ////////////////////////////////////////////////////////////
                Stream() : mFile("")
    {

    }

    ////////////////////////////////////////////////////////////
    /// \brief Construct a stream ready to be operate on
    ///
    /// \param file Path to the file to operate
    /// \param create Create new if file doesn't exist, false by
    /// default
    ///
    ////////////////////////////////////////////////////////////
                Stream(const std::filesystem::path& file, bool create = false) : mFile(file)
    {
        open(file, create);
    }

    ////////////////////////////////////////////////////////////
    /// \brief Default destructor
    ///
    ////////////////////////////////////////////////////////////
                ~Stream()
    {
        if(mStream)
        {
            mStream.close();
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Open a file to operate on
    ///
    /// \param file Path to the file to operate
    /// \param create Create new if file doesn't exist, false by
    /// default
    ///
    ////////////////////////////////////////////////////////////
    void        open(const std::filesystem::path& file, bool create = false)
    {
        bool exists = true;

        mFile = file;

        if(!std::filesystem::exists(file))
        {
            if(create)
            {
                std::ofstream writer(file);

                if(!writer && file.string().size() > file.filename().string().size())
                {
                    std::filesystem::create_directories(file.string().substr(0, file.string().size() - file.filename().string().size()));
                    writer.open(file);
                }

                if(writer)
                {
                    writer.close();
                }
                else
                {
                    exists = false;
                    throw std::runtime_error("Stream failed to create file on \"" + file.string() + "\"");
                }
            }
            else
            {
                exists = false;
                throw std::runtime_error("Path \"" + file.string() + "\" doesn't point to any file");
            }
        }

        if(exists)
        {
            mStream.open(file);

            if(!mStream)
            {
                throw std::runtime_error("Stream failed to open file on \"" + file.string() + "\"");
            }
        }
    }

    ///////////////////////////////////////////////////////////
    /// \brief Overload of operator << to write to the file
    ///
    ////////////////////////////////////////////////////////////
    Stream&     operator <<(Key key)
    {
        write(key);
        return *this;
    }

    ///////////////////////////////////////////////////////////
    /// \brief Overload of operator >> to read from a file
    ///
    ////////////////////////////////////////////////////////////
    Stream&     operator >>(const std::string& name)
    {
        mKey = read(name);
        return *this;
    }

    ///////////////////////////////////////////////////////////
    /// \brief Overload of operator () to access internal key
    ///
    /// \return Internal key
    ///
    ////////////////////////////////////////////////////////////
    Key&        operator ()()
    {
        return mKey;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Write a key to the stream
    ///
    /// \param key Key to write
    ///
    ////////////////////////////////////////////////////////////
    void        write(const Key& key)
    {
        if(mStream)
        {
            mStream.seekg(0, mStream.beg);

            std::vector<std::string> fields;

            std::string field;
            while(std::getline(mStream, field))
            {
                auto pos = field.find_first_of(':');

                if(key.name != field.substr(0, pos))
                {
                    fields.push_back(field);
                }
            }

            mStream.close();

            mStream.open(mFile, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

            for(const auto& it: fields)
            {
                mStream << it << '\n';
            }

            mStream << key.name << ':';

            for(size_t i = 0; i < key.values.size(); ++i)
            {
                std::holds_alternative<std::string>(key.values[i]) ? mStream << '\"' << Key::string(key.values[i]) << '\"' : mStream << Key::string(key.values[i]);

                if(i != key.values.size() - 1)
                {
                    mStream << ',';
                }
            }

            mStream.clear();
        }
        else
        {
            throw std::runtime_error("Could not write, stream failed");
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Read a key from the stream
    ///
    /// \param name Name of the key to read
    ///
    /// \return Key
    ///
    ////////////////////////////////////////////////////////////
    Key     read(const std::string& name)
    {
        Key key{name, {}};

        if(mStream)
        {
            mStream.seekg(0, mStream.beg);

            std::string field;

            bool done = false;
            while(std::getline(mStream, field) && !done)
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
                            key.values.push_back(token);
                        }
                        else if(token == "true")
                        {
                            key.values.push_back(bool(true));
                        }
                        else if(token == "false")
                        {
                            key.values.push_back(bool(false));
                        }
                        else if(token.front() == '\"' && token.back() == '\"')
                        {
                            token.pop_back();
                            token.erase(token.begin());

                            key.values.push_back(token);
                        }
                        else if(token.find('.') != std::string::npos)
                        {
                            key.values.push_back(std::stod(token));
                        }
                        else
                        {
                            key.values.push_back(std::stoi(token));
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

            mStream.clear();
        }
        else
        {
            throw std::runtime_error("Could not read, stream failed");
        }

        return key;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Remove a key in the stream
    ///
    /// \param name Name of the key to delete
    ///
    ////////////////////////////////////////////////////////////
    void  remove(const std::string& name)
    {
        if(mStream)
        {
            mStream.seekg(0, mStream.beg);

            std::vector<std::string> fields;

            std::string field;
            while(std::getline(mStream, field))
            {
                auto pos = field.find_first_of(':');

                if(name != field.substr(0, pos))
                {
                    fields.push_back(field);
                }
            }

            mStream.close();

            mStream.open(mFile, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);

            for(const auto& it: fields)
            {
                mStream << it << '\n';
            }

            mStream.clear();
        }
        else
        {
            throw std::runtime_error("Could not remove, stream failed");
        }
    }

private:
    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    std::fstream            mStream;    ///< Stream
    std::filesystem::path   mFile;      ///< Path to the file to operate
    Key                     mKey;       ///< Key
};

////////////////////////////////////////////////////////////
/// \brief Stream wrapper class for a locker-room type
/// database based on files
///
////////////////////////////////////////////////////////////
class Room
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    ////////////////////////////////////////////////////////////
            Room() : mBase(std::filesystem::current_path())
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
    /// \brief Set the base directory, current path by default
    ///
    /// \param directory Path to the directory
    /// \param create If true and directory doesn't exist create
    /// a new one, false by default
    ///
    ////////////////////////////////////////////////////////////
    void    connect(const std::filesystem::path& directory, bool create = false)
    {
        if(std::filesystem::exists(directory))
        {
            if(std::filesystem::is_directory(directory))
            {
                mBase = std::filesystem::absolute(directory);
            }
            else
            {
                throw std::runtime_error("Invalid path, must be a directory.");
            }
        }
        else if(create)
        {
            std::filesystem::create_directories(directory);

            mBase = std::filesystem::absolute(directory);
        }
        else
        {
            throw std::runtime_error("Path \"" + directory.string() + "\" doesn't point to any file");
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Open a drawer which contains keys to operate on
    ///
    /// \param file Path to the file
    /// \param function Operations
    /// \param create Create a new file if path doesn't point to
    /// any, false by default
    ///
    ////////////////////////////////////////////////////////////
    void    open(const std::filesystem::path& file, std::function<void(Stream&)> function, bool create = false)
    {
        bool done = true;

        if(!std::filesystem::exists(mBase / file))
        {
            if(create)
            {
                std::ofstream writer(mBase / file);

                if(writer)
                {
                    writer.close();
                }
                else
                {
                    done = false;
                    throw std::runtime_error("Stream failed to open file on \"" + file.string() + "\"");
                }
            }
            else
            {
                done = false;
                throw std::runtime_error("Path \"" + file.string() + "\" doesn't point to any file");
            }
        }

        if(done)
        {
            if(std::filesystem::is_regular_file(mBase / file))
            {
                Stream stream({mBase / file});
                function(stream);
            }
            else
            {
                throw std::runtime_error("Invalid path \"" + file.string() + "\", must be a regular file");
            }
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Check if file exists
    ///
    /// \param file Path to the file
    ///
    /// \return True if file exists
    ///
    ////////////////////////////////////////////////////////////
    bool    exists(const std::filesystem::path& file)
    {
        if(std::filesystem::exists(mBase / file))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Destroy a file or directory
    ///
    /// \param file Path to the file
    ///
    ////////////////////////////////////////////////////////////
    void    destroy(const std::filesystem::path& file)
    {
        if(std::filesystem::exists(mBase / file))
        {
            std::filesystem::remove_all(mBase / file);
        }
        else
        {
            throw std::runtime_error("Path \"" + file.string() + "\" doesn't point to any file");
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Short way to write a key in a drawer
    ///
    /// \param file Path to the file
    /// \param key Key to write
    /// \param create Create a new file if path doesn't point to
    /// any, false by default
    ///
    ////////////////////////////////////////////////////////////
    void    quick_write(const std::filesystem::path& file, const Key& key, bool create = false)
    {
        Stream stream(mBase / file, create);

        stream << key;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Short way to read a key from a drawer
    ///
    /// \param file Path to the file, must exist
    /// \param name Name of the key
    ///
    ////////////////////////////////////////////////////////////
    Key     quick_read(const std::filesystem::path& file, const std::string& name)
    {
        Stream stream(mBase / file);

        stream >> name;

        return stream();
    }

protected:

private:
    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    std::filesystem::path   mBase;  ///< Path to the base directory
};

} // namespace CNRoom

#endif // ROOM_HPP
