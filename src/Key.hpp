/*
Key.hpp
用KeyEvent包装了输入的按键,KeyEvent包含输入按键的种类和ASCII值
*/
#ifndef MINIVIM_KEY_HPP
#define MINIVIM_KEY_HPP

namespace sjtu {

constexpr char ControlKey(char key) noexcept {
    //一个C++风格的经典宏,其实就是 #define C(k) (k & 0x1FU)
    //它是用来将字母键转换为对应的 ASCII 控制字符(即模拟按下 Ctrl+字母 的组合键效果)
    return key & 0x1FU;
}

enum class KeyCode {
    Character,
    Escape,
    Enter,
    Backspace,
    Delete,
    //后面的在整个项目中都不会被用到,它们存在只是因为我们给的终端读取机制比较完整...
    Home,
    End,
    PageUp,
    PageDown,
    ArrowLeft,
    ArrowRight,
    ArrowUp,
    ArrowDown,
};

struct KeyEvent {
    KeyCode code_{KeyCode::Character};
    char value_{0};

    static constexpr KeyEvent Character(char value) noexcept {
       //工厂函数,从Char生成一个KeyEvent
       return {KeyCode::Character,value};
    }

    constexpr bool IsCharacter(char expected) const noexcept {
        //判断当前KeyEvent是不是expected
        
        return (code_ == KeyCode::Character && value_ == expected);
    }

    constexpr bool IsControl(char expected) const noexcept {
        //判断当前KeyEvent是不是Ctrl-expected
        return (code_ == KeyCode::Character && value_ == ControlKey(expected));
    }
};

} // namespace sjtu

#endif // MINIVIM_KEY_HPP
