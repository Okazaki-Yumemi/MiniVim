#include "Command.hpp"
#include <limits>

namespace sjtu {

EditorAction NormalModeParser::Feed(KeyEvent key) {
    //根据传入的key生成Action,在Basic部分中你应该直接调用GenerateMotion
    if (key.code_ == KeyCode::Escape) {
        // pending和prefix设置为none
        pending = false;
        prefix = "";

        //需要清除状态
        count_ = 0;
        has_count_ = false;
        return {ActionKind::None};
    }

    if(prefix != ""){
        std::string cmd = prefix + key.value_;

        if(cmd == "gg"){
            pending = false;
            prefix = "";
            return GenerateMotion(Motion::MoveToFirstRowgg);
        }else if(cmd == "ge"){
            pending = false;
            prefix = "";
            return GenerateMotion(Motion::Move_ge);
        }else if(cmd == "gE"){
            pending = false;
            prefix = "";
            return GenerateMotion(Motion::Move_gE);
        }else{
            pending = false;
            prefix = "";

            // 清空状态
            count_ = 0;
            has_count_ = false;
            return {};
        }
    }


    if (key.code_ == KeyCode::Character) {
        auto value = key.value_;

        //处理数字
        if( value >= '1' && value<= '9'){
            size_t digit = static_cast<size_t>(value - '0');

            if(count_ > (std::numeric_limits<size_t>::max() - digit) / 10){
                count_ = std::numeric_limits<size_t>::max();
            }else{
                count_ = count_ * 10 + digit;
            }
        }

        if( value == '0' && has_count_){
            if(count_ > (std::numeric_limits<size_t>::max() - 0)/ 10){
                count_ = std::numeric_limits<size_t>::max();
            }else{
                count_ *= 10;
            }
            return {};
        }


        switch (value) {
        //你需要填写这里
        case 'h':
            return GenerateMotion(Motion::Left);
        case 'j':
            return GenerateMotion(Motion::Down);
        case 'k':
            return GenerateMotion(Motion::Up);
        case 'l':
            return GenerateMotion(Motion::Right);
        case '0':
            return GenerateMotion(Motion::MoveToLineHead);
        case '$':
            return GenerateMotion(Motion::MoveToLineEnd);
        case '^':
            return GenerateMotion(Motion::MoveToFirstNoneEmpty);
        case 'G':
            return GenerateMotion(Motion::MoveToLastRowG);
        case 'g':
            pending = true;
            prefix = "g";
            return {};
        case 'w':
            return GenerateMotion(Motion::Move_w);
        case 'W':
            return GenerateMotion(Motion::Move_W);
        case 'e':
            return GenerateMotion(Motion::Move_e);
        case 'E':
            return GenerateMotion(Motion::Move_E);
        case 'b':
            return GenerateMotion(Motion::Move_b);
        case 'B':
            return GenerateMotion(Motion::Move_B);
        case 'i':
            return GenerateCommand(ActionKind::InsertBefore);
        case 'a':
            return GenerateCommand(ActionKind::InsertAfter);
        case ':':
            return GenerateCommand(ActionKind::EnterCommandLine);
        case 'I':
            return GenerateCommand(ActionKind::InsertFirstNoneBlank);
        case 'A':
            return GenerateCommand(ActionKind::AppendLineEnd);
        case 'O':
            return GenerateCommand(ActionKind::OpenAbove);
        case 'o':
            return GenerateCommand(ActionKind::OpenBelow);
        case 'x':
            return GenerateCommand(ActionKind::DeleteCurrChar);
        case 'X':
            return GenerateCommand(ActionKind::DeleteBeforeChar);
        case '~':
            return GenerateCommand(ActionKind::ChangeCase);
        
        default:
            break;
        }
        return {};
    }

    //switch (key.code_) {
    //case KeyCode::ArrowLeft:
        //return GenerateMotion(Motion::Left);
    //case KeyCode::ArrowRight:
        //return GenerateMotion(Motion::Right);
    //case KeyCode::ArrowUp:
        //return GenerateMotion(Motion::Up);
    //case KeyCode::ArrowDown:
        //return GenerateMotion(Motion::Down);
    //default:
        //return {};
    //}
    //虽然不要求这些按键,但是我们给你的Terminal.hpp可以处理这些你键盘上的特殊按键并把他们放在了keycode里
    //在Vim中,它们对应着hjkl.

    return {};
}


EditorAction NormalModeParser::GenerateMotion(Motion motion) {
    EditorAction action;
    action.kind_ = ActionKind::Move;
    action.motion_ = motion;

    if(has_count_){
        action.count_ = count_;
        action.has_count_ = true;
    } else{
        action.count_ = 1;
        action.has_count_ = false;
    }

    count_ = 0;
    has_count_ = false;
    //清空状态

    return action;
}

EditorAction NormalModeParser::GenerateCommand(ActionKind kind) {
    EditorAction action;
    action.kind_ = kind;

    if(has_count_){
        action.count_ = count_;
        action.has_count_ = true;
    }else{
        action.count_ = 1;
        action.has_count_ = false;
    }

    count_ = 0;
    has_count_ = false;

    return action;
}

} // namespace sjtu
