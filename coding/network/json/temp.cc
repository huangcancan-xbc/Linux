#include <iostream>
#include <jsoncpp/json/json.h>

int main()
{
    // 序列化示例
    Json::Value data;
    data["name"] = "张三";
    data["age"] = 18;
    
    Json::Value skills;
    skills.append("C++");
    skills.append("Python");
    skills.append("Linux");
    data["skills"] = skills;
    
    Json::FastWriter writer;
    std::string jsonStr = writer.write(data);
    std::cout << "生成的JSON: " << jsonStr << std::endl;
    
    // 反序列化示例
    Json::Value parsedData;
    Json::Reader reader;
    if (reader.parse(jsonStr, parsedData))
    {
        std::cout << "解析出的姓名: " << parsedData["name"].asString() << std::endl;
        std::cout << "技能列表: ";
        for (const auto& skill : parsedData["skills"])
        {
            std::cout << skill.asString() << " ";
        }
    }
    
    return 0;
}
