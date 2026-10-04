/*
Window.hpp
Window保存光标位置和正文的可见区域,根据Buffer的行数和行长限制移动范围.
移动只改变光标和视口,文件内容的修改由Buffer完成.
*/
#ifndef MINIVIM_WINDOW_HPP
#define MINIVIM_WINDOW_HPP

#include "Buffer.hpp"
#include "Types.hpp"


namespace sjtu {

class Window {

public:
    void Resize(ScreenSize terminal_size);

    void ApplyMotion(const Buffer& buffer, Motion motion,size_t count = 1 , bool has_count = false);

    void EnsureCursorVisible(const Buffer& buffer,bool normal_cursor = true);

    //allow_line_end为true时允许停在最后一个字符之后,供插入模式使用
    void SetCursor(const Buffer& buffer, Position position, bool allow_line_end);
    const Position& GetCursor() const ;
    const Viewport& GetViewport() const ;

private:
    void MoveLeft(const Buffer& buffer, size_t count);
    void MoveRight(const Buffer& buffer, size_t count);
    void MoveUp(const Buffer& buffer, size_t count);
    void MoveDown(const Buffer& buffer, size_t count);
    void MoveToLineHead(const Buffer& buffer);
    void MoveToLineEnd(const Buffer& buffer,size_t count = 1);
    void MoveToFirstNoneEmpty(const Buffer& buffer);
    void MoveToLastRowG(const Buffer& buffer, size_t count = 1 , bool has_count = false);
    void MoveToFirstRowgg(const Buffer& buffer,size_t count=1, bool has_count= false);
    std::string Classify(char ch, bool mode);
    Position NextStart(const Buffer& Buffer,bool mode);
    void OpWw(const Buffer& buffer,bool mode);
    Position NextEnd(const Buffer& Buffer, bool mode);
    void OpEe(const Buffer& buffer, bool mode);
    Position PrevStart(const Buffer& Buffer, bool mode);
    void OpBb(const Buffer& Buffer, bool mode);
    Position PrevEnd(const Buffer& Buffer, bool mode);
    void OpgegE(const Buffer& Buffer, bool mode);


    Position cursor_{}; //Buffer中的光标位置
    Viewport viewport_{}; //正文可见区域及其滚动偏移
    size_t desired_column_{0}; //上下移动时希望保持的显示列,经过短行时也保留这个目标
    bool desired_eol_{false};
};

} // namespace sjtu

#endif // MINIVIM_WINDOW_HPP
