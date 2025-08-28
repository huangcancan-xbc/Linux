#include "UDP_Server.hpp"
#include <memory>
#include <vector>

void Usage(string proc)
{
    cout << "\n\r用法: " << proc << " 端口号[1024+]\n" << "说明: 端口号建议使用1024以上的端口，避免与系统保留端口冲突\n" << "示例: " << proc << " 8080\n" << endl;
}

// 对命令进行安全检查
bool SafeCheck(const string& cmd)
{
    // 可能不安全的命令，可自行添加
    vector<string> unsafe = {
        "rm",
        "mv",
        "cp",
        "kill",
        "sudo",
        "unlink",
        "uninstall",
        "yum",
        "top",
        "while"
    };

    for(const auto& danger_cmd : unsafe)
    {
        // 检查命令中是否包含危险命令
        if(cmd.find(danger_cmd) != string::npos)
        {
            return false;  // 找到危险命令，返回false
        }
    }
    return true;
}

// 处理收到的消息
string Handler(const string& s)
{
    string ret = "服务器收到一条消息: ";
    ret += s;
    cout << ret << endl;

    return ret;
}

// 可以直接执行命令的函数
string ExcuteCommand(const string& cmd)
{
    cout << "获取一个请求命令: " << cmd << endl;
    if(!SafeCheck(cmd))
    {
        cout << "这是一个不安全的命令，已经发现，并且拒绝执行！" << endl;
        return "Unsafe commands!";
    }

    // popen 用于创建管道并读取命令，参数：要执行的命令，打开管道的模式（"r"：读）
    FILE *fp = popen(cmd.c_str(), "r");
    if(nullptr == fp)
    {
        perror("popen");
        return "error";
    }

    string ret;
    char buffer[4096];

    while(true)
    {
        char *ok = fgets(buffer, sizeof(buffer), fp);
        if(ok == nullptr)
        {
            break;
        }
        ret += buffer;
    }

    pclose(fp);                                                 // 关闭管道

    return ret;
}

int main(int argc, char* argv[])
{
    // 检查命令行参数数量，argc != 2 的原因：
    // argv[0]: 程序名称（如 "./udpserver"），argv[1]: 监听端口号（如 "8080"）
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(0);
    }

    // 解析端口号参数
    uint16_t port = std::stoi(argv[1]);                         // 将字符串端口号转换为整数

    unique_ptr<UDP_Server> svr(new UDP_Server(port));           // 使用智能指针创建UDP服务器对象,自动内存管理，避免内存泄漏
    svr->Init();                                                // 初始化服务器
    svr->Run(Handler);                                          // 启动服务器，可选择的函数：Handler、ExcuteCommand

    return 0;
}