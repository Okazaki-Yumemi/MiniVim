/*
Editor.hpp
Editor负责协调各个模块,保存当前模式,并根据输入调用Buffer和Window完成编辑.
你可以沿着Run -> ProcessKey -> 各模式的处理函数阅读程序,再看RefreshScreen如何把结果显示出来.
*/
#ifndef MINIVIM_EDITOR_HPP
#define MINIVIM_EDITOR_HPP

#include <filesystem>
#include <string>

#include "Buffer.hpp"
#include "Command.hpp"
#include "Renderer.hpp"
#include "Terminal.hpp"
#include "Types.hpp"
#include "Window.hpp"


namespace sjtu {

class Editor {

public:
    explicit Editor(const std::filesystem::path& path = {});

    //编辑器的主循环,直到用户要求退出
    void Run();
    bool IsRunning() const noexcept;

private:
    //屏幕刷新,生成新的一帧,在Run循环中第一个调用
    void RefreshScreen();

    //总执行逻辑,根据Key和Mode分发给下属函数来处理
    void ProcessKey(KeyEvent key);

    //ProcessKey的下属函数之一,Editor对NormalMode下的Action的执行,这个action是从parser传进来的
    void Execute(const EditorAction& action);

    //ProcessKey的下属函数之一,Editor在InsertMode下对于KeyEvent的处理逻辑
    //FAQ:为啥InsertMode和CommandMode的处理直接属于Editor.hpp/cpp,而NormalMode则分出去一个单独的文件(Command.hpp/cpp)?
    //NormalMode的命令解析部分比较复杂所以我们单独解析,而这两部分逻辑需要Editor直接统筹全局其他模块且比较简明
    void EnterInsert(Position position);
    void EnterFirstNoneEmptyInsert();
    void EnterLastWordInsert();
    void LeaveInsert();
    void OpenBelow();
    void OpenAbove();

    void DeleteCurrChar();
    void DeleteBeforeChar();

    void ChangeCase();

    void HandleInsert(KeyEvent key);

    //ProcessKey的下属函数之一,Editor在CommandMode下对于KeyEvent的处理逻辑
    void HandleCommandLine(KeyEvent key);
    void ExecuteCommandLine();
    void LeaveCommandLine();

    bool SaveBuffer(const std::filesystem::path& path = {});

    Buffer buffer_; //文件的实际内容
    Terminal terminal_; //读取按键,向终端输出,管理终端设置
    Window window_; //光标和当前可见区域
    Renderer renderer_; //根据内容和状态生成要输出的画面
    NormalModeParser normal_parser_; //把Normal模式的按键转换成EditorAction

    Mode mode_{Mode::Normal}; //当前编辑模式
    std::string command_; //命令行模式下已经输入的内容,不含开头的冒号
    std::string message_; //底部的提示或错误信息
    bool running_{true}; //设为false后结束主循环
};

} // namespace sjtu

#endif // MINIVIM_EDITOR_HPP
