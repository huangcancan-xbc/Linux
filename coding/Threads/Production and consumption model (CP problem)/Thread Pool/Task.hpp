#pragma once
#include <iostream>
#include <string>
using namespace std;

const string opers = "+-*/%";                                        // 支持的操作符集合

// 错误代码枚举定义
enum
{
    SUCCESS = 0,                                                     // 成功
    DIV_ERROR = 1,                                                   // 除零错误
    MOD_ERROR = 2,                                                   // 取模零错误
    UNKNOWN_ERROR = 3                                                // 未知操作符错误
};

class Task
{
public:
    // 默认构造函数
    Task()
        :_x(0), _y(0), _op('+'), _ret(0), _code(SUCCESS)
    {}
    
    // 带参数构造函数：初始化任务参数
    Task(int x, int y, char op = '+')
        :_x(x),
        _y(y),
        _op(op),
        _ret(0),
        _code(SUCCESS)
    {
        // 验证操作符是否合法
        if(op != '+' && op != '-' && op != '*' && op != '/' && op != '%')
        {
            _op = '+';                                               // 默认为加法
            _code = UNKNOWN_ERROR;                                   // 设置错误码
        }
    }

    // 执行运算任务
    void run()
    {
        // 重置结果和错误码（避免重复调用时的问题）
        _ret = 0;
        _code = SUCCESS;
        
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
                _code = UNKNOWN_ERROR;                               // 未知操作符错误
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
                     " [错误代码：" + to_string(_code) + "]";
        return ret;
    }

    // 获取操作符
    char get_operator() const
    {
        return _op;
    }

    // 获取第一个操作数
    int get_first_operand() const
    {
        return _x;
    }

    // 获取第二个操作数
    int get_second_operand() const
    {
        return _y;
    }

    // 获取运算结果
    int get_result() const
    {
        return _ret;
    }

    // 获取错误代码
    int get_error_code() const
    {
        return _code;
    }

    // 析构函数
    ~Task()
    {
    }

private:
    int _x, _y;                                                      // 两个操作数
    int _ret;                                                        // 运算结果
    char _op;                                                        // 操作符
    int _code;                                                       // 错误代码
};