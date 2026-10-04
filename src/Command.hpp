/*
Command.hpp
用来将NormalMode下的用户输入转化成Editor可以看懂的EditorAction
*/
#ifndef MINIVIM_COMMAND_HPP
#define MINIVIM_COMMAND_HPP

#include <optional>
#include <string>

#include "Key.hpp"
#include "Types.hpp"


namespace sjtu {

//ActionKind是给NormalMode的用户命令的简单枚举类
enum class ActionKind {
    //这些是Basic部分需要的ActionKind,None是占位符,Move表示包括hjkl在内的移动,后面分别对应i,a,:
    None,
    Move,
    InsertBefore,
    InsertAfter,
    EnterCommandLine,
    //在Advanced部分中你很可能需要添加别的ActionKind
    InsertFirstNoneBlank,
    AppendLineEnd,
};

//EditorAction是在NormalMode下我们的Editor真正收到的指令,在Editor中会根据传入的Action的不同让各模块做出对应的处理
struct EditorAction {
    ActionKind kind_{ActionKind::None};
    std::optional<Motion> motion_{};//当kind_ = Move时,motion_才有意义,在Basic中表示移动方向(Motion的定义你可以右键跳转查看)
    //EditorAction和ActionKind分层设计是为了可扩展性,Basic部分的命令都是单个按键,但在Advanced部分中你会遇到多按键命令的情况,
    //这时候就会遇到按键的pending,比如dw,你可以先构造一个EditorAction存储kind_=Delete,等用户输入motion后令motion=w.
    //在Advanced部分中你很可能需要添加新的字段,比如std::optional<Count> count_{};
};


class NormalModeParser {
//这个类用来处理用户按键的输入,对于Basic部分,每传入一个key你都可以让它生成一个完整的EditorAction(因为我们目前只支持单按键指令)
//在Advanced部分你很可能需要给已有接口添加多按键的逻辑/添加新的接口
public:
//把用户传入的新的key喂给parser,从parser生成当前的EditorAction,稍后转给Editor执行
//这是核心接口,适合扩展和沿用
    EditorAction Feed(KeyEvent key);

private:
//两个辅助函数,从motion/actionkind直接生成对应editoraction,只对某些简单的情形生效.比如h,我们可以根据motion直接得到editoraction为{ActionKind::Move, Motion::h};
//这两个接口相当局限,在Advanced中你完全可以不管这两个接口,自己创建更方便的函数来辅助Feed函数


    EditorAction GenerateMotion(Motion motion);
    EditorAction GenerateCommand(ActionKind kind);

    std::string prefix{""};
    bool pending{false};
    

};

} // namespace sjtu

#endif // MINIVIM_COMMAND_HPP
