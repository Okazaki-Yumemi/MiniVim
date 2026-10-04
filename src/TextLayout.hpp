/*
TextLayout.hpp
这些辅助函数负责字符下标和显示列之间的换算,你可以在任意文件使用它们
*/

#ifndef MINIVIM_TEXT_LAYOUT_HPP
#define MINIVIM_TEXT_LAYOUT_HPP

#include <string_view>

namespace sjtu {

constexpr size_t tabStop = 4; //Tab对齐到下一个4的倍数列,不一定总占4列

inline size_t NextScreenColumn(size_t column, char value) {
    //返回从column开始显示value后的位置:Tab跳到下一个制表位,普通字符占1列
    if (value == '\t') {
        return column + (tabStop - (column % tabStop)); 
    }

    return column + 1;
}

inline size_t RenderColumnToBufferColumn(std::string_view line, size_t render_column) {
    //把整行展开后的显示列转换为字符下标
    //显示列落在Tab占据的任意一列时,都返回该Tab的字符下标
    //非空行中,显示列超过展开后的行尾时返回line.size()
    if (line.empty()) {
        return 0;
    }

    size_t current = 0;
    for (size_t index = 0; index < line.size(); ++index) {
        size_t next = NextScreenColumn(current, line[index]);
        if (render_column < next) {
            return index;
        }
        current = next;
    }

    return line.size();
}

//展示布局
//Insert 光标位置
//字符起始位置计算

inline size_t BufferColumnToRenderColumn(std::string_view line, size_t buffer_column) {
    //把字符下标转换为该字符在整行展开后的起始显示列
    //调用方须保证buffer_column在[0, line.size()]内
    //传入line.size()返回整行的显示宽度
    size_t column = 0;
    for (size_t index = 0; index < buffer_column; ++index) {
        column = NextScreenColumn(column, line[index]);
    }

    return column;
}


// Normal cursor / curswant 在哪里
//Normal 模式下 desired_column_
//Normal 模式 cursor 的视觉位置
inline size_t BufferColumnToNormalCursorColumn(std::string_view line, size_t buffer_column){
    size_t start = BufferColumnToRenderColumn(line,buffer_column);

    if(buffer_column < line.size() && line[buffer_column] == '\t' ){
        return NextScreenColumn(start , '\t') - 1;
    }
    return start;
}


} 

#endif // MINIVIM_TEXT_LAYOUT_HPP
