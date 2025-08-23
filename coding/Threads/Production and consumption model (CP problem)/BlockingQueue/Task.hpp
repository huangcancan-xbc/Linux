#pragma once
#include <iostream>
#include <string>
using namespace std;

// 错误代码枚举
enum
{
    SUCCESS = 0,                                                     // 成功
    DIV_ERROR = 1,                                                   // 除零错误
    MOD_ERROR = 2,                                                   // 取模零错误
};

class Task
{
private:
    int _x, _y;                                                      // 两个操作数
    int _ret;                                                        // 运算结果
    char _op;                                                        // 操作符
    int _code;                                                       // 错误代码

public:
    // 构造函数：初始化任务参数
    Task(int x = 0, int y = 0, char op = '+')
        :_x(x),
        _y(y),
        _op(op),
        _ret(0),
        _code(SUCCESS)
    {
    }

    // 执行运算任务
    void run()
    {
        switch(_op)
        {
            case '+':
                _ret = _x + _y;
                break;
            case '-':
                _ret = _x - _y;
                break;
            case '*':
                _ret = _x * _y;
                break;
            case '/':
                if(_y == 0)
                {
                    _code = DIV_ERROR;                               // 除零错误
                    _ret = 0;
                }
                else
                {
                    _ret = _x / _y;
                }
                break;
            case '%':
                if(_y == 0)
                {
                    _code = MOD_ERROR;                               // 取模零错误
                    _ret = 0;
                }
                else
                {
                    _ret = _x % _y;
                }
                break;
            default:
                _code = MOD_ERROR;                                   // 未知操作符错误
                _ret = 0;
                break;
        }
    }

    // 重载函数调用操作符，使Task对象可以像函数一样调用
    void operator()()
    {
        run();                                                       // 执行运算
    }

    // 获取任务描述字符串
    string get_task() const
    {
        return to_string(_x) + _op + to_string(_y) + "= ???";        // 格式：x op y = ???
    }

    // 获取运算结果字符串
    string get_ret() const
    {
        string ret = to_string(_x) + _op + to_string(_y) + "=" + to_string(_ret) + 
                     " [错误代码]：" + to_string(_code);
        return ret;
    }
};