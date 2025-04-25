
#include <string>
#include <stdio.h>
#include <iostream>
#include "json/json.h"

template <typename T>
struct reflect_trait {
    template <typename Func>
    static constexpr void for_each_member(T& obj, Func&& func)
    {
        obj.for_each_member(func);
    } 
};

#define EXPAND(x) EXPAND_2(x)
#define EXPAND_2(x) x
#define CONCAT(x, y) CONCAT_2(x, y)
#define CONCAT_2(x, y) x##y

#define REFLECT_TYPE_START(CType) \
template <> \
struct reflect_trait<CType> \
{ \
    template <typename Func> \
    static constexpr void for_each_member(CType& obj, Func&& func) \
    { \

#define REFLECT_TYPE_END \
    } \
}; \

#define REFLECT_TYPE_MEMBER(x) \
    func(#x, obj.x);

#define REFLECT_MEMBER(x) \
    func(#x, x);

#define REFLECT_PP_FOREACH_1(f, _1) f(_1)
#define REFLECT_PP_FOREACH_2(f, _1, _2) f(_1) f(_2)
#define REFLECT_PP_FOREACH_3(f, _1, _2, _3) f(_1) f(_2) f(_3)
#define REFLECT_PP_FOREACH_4(f, _1, _2, _3, _4) f(_1) f(_2) f(_3) f(_4)
#define REFLECT_PP_FOREACH_5(f, _1, _2, _3, _4, _5) f(_1) f(_2) f(_3) f(_4) f(_5)
#define REFLECT_PP_FOREACH_6(f, _1, _2, _3, _4, _5, _6) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6)
#define REFLECT_PP_FOREACH_7(f, _1, _2, _3, _4, _5, _6, _7) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7)
#define REFLECT_PP_FOREACH_8(f, _1, _2, _3, _4, _5, _6, _7, _8) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8)
#define REFLECT_PP_FOREACH_9(f, _1, _2, _3, _4, _5, _6, _7, _8, _9) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9)
#define REFLECT_PP_FOREACH_10(f, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) f(_1) f(_2) f(_3) f(_4) f(_5) f(_6) f(_7) f(_8) f(_9) f(_10)

// calculate n
#define REFLECT_CAL_ARGS_N_IMPL(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) N
#define REFLECT_CAL_ARGS_N(...) REFLECT_CAL_ARGS_N_IMPL(__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)

#define REFLECT_PP_FOREACH_n(f, ...) \
    EXPAND(CONCAT(REFLECT_PP_FOREACH_, REFLECT_CAL_ARGS_N(__VA_ARGS__))(f, __VA_ARGS__))
    
#define REFLECT_TYPE(CType, ...) \
REFLECT_TYPE_START(CType) \
REFLECT_PP_FOREACH_n(REFLECT_TYPE_MEMBER, __VA_ARGS__) \
REFLECT_TYPE_END

#define REFLECT_START(...) \
    template <typename Func> \
    constexpr void for_each_member(Func&& func) \
    { \

#define REFLECT_END \
    } \


#define REFLECT(...) \
    REFLECT_START(__VA_ARGS__)\
    REFLECT_PP_FOREACH_n(REFLECT_MEMBER, __VA_ARGS__) \
    REFLECT_END


template <typename T>
concept is_struct = requires (T t)
{
    t.for_each_member([](const char*, auto&) {});
};

std::string jsonToStr(Json::Value const& root)
{
    return root.toStyledString();
}

template <typename T> requires (!is_struct<T>)
Json::Value serialize(T& obj)
{
    return obj;
}

template <typename T> requires is_struct<T>
Json::Value serialize(T& obj)
{
    Json::Value root;
    reflect_trait<T>::for_each_member(obj, [&](const char* key, auto& value){
        root[key] = serialize(value);
    });

    return root;
}


Json::Value strToJson(std::string const& json)
{
    Json::Value root;
    Json::Reader reader;

    reader.parse(json, root);
    return root;
}

template <typename T> requires (!is_struct<T>)
T deserialize(Json::Value root)
{
    return root.as<T>();
}

template <typename T> requires (is_struct<T>)
T deserialize(Json::Value root)
{
    T obj;
    
    reflect_trait<T>::for_each_member(obj, [&](const char* key, auto& value){
        value = deserialize<std::decay_t<decltype(value)>>(root[key]);
    });

    return obj;
}
////////////////////

class Person
{
public:
    std::string name;
    int age;

    REFLECT(name, age)
};


class Address
{
public:
    std::string country;
    std::string province;
    std::string city;
    std::string street;

    REFLECT(country, province, city, street)
};

class Student
{
public:
    std::string name;
    int age;
    int grade;
    Address addr;

    REFLECT(name, age, grade, addr)
};

class Dog
{
public:
    std::string name;
    int age;
    std::string breed;
    std::string hair_color;

    REFLECT(name, age, breed, hair_color)
};

// REFLECT_TYPE(Person, name, age)
// REFLECT_TYPE(Student, name, age, grade, addr)
// REFLECT_TYPE(Dog, name, age, breed, hair_color)

int main()
{
    // Person p = {
    //     .name = "Huang",
    //     .age = 16,
    // };
    // std::string pp = jsonToStr(serialize(p));
    // std::cout << pp; std::endl(std::cout);

    Student s = {
        .name = "Ginko",
        .age = 16,
        .grade = 12,
        .addr = {
            .country = "China",
            .province = "Jiangxi",
            .city = "Nanchang",
            .street = "Gemin",
        },
    };

    std::string ss = jsonToStr(serialize(s));
    std::cout << ss << std::endl;

    Student new_s = deserialize<Student>(strToJson(ss));
    std::cout << new_s.name << " " << new_s.age << new_s.grade << std::endl;
    std::cout << new_s.addr.country;
    std::cout << new_s.addr.province;
    std::cout << new_s.addr.city;
    std::cout << new_s.addr.street;

    // Dog d = {
    //     .name = "John",
    //     .age = 2,
    //     .breed = "Alaskan",
    //     .hair_color = "black and white"
    // };

    // std::cout << jsonToStr(serialize(d)) << std::endl;
}
