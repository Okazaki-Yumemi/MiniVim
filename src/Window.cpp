#include "Window.hpp"
#include "TextLayout.hpp"

#include <algorithm>
#include <cctype>

namespace sjtu {


void Window::Resize(ScreenSize terminal_size) {
    //底部留一行给命令或提示,其余作为正文区域;正文行数和列数都至少取1
    //修改视口即可
    viewport_.rows_ = terminal_size.rows_ > 1 ? terminal_size.rows_ - 1 : 1;

    viewport_.columns_ = terminal_size.columns_ > 0 ? terminal_size.columns_ : 1;
}

void Window::ApplyMotion(const Buffer& buffer, Motion motion) {
    //1. 根据方向调用对应的移动函数,Basic中每次移动一步,在Advanced中你可以改变count/添加别的case
    //2. 将行列限制在Normal模式的合法范围内(Buffer应始终至少有一行)
    //3. 调整视口,让移动后的光标可见(EnsureCursorVisible)
    if(motion == Motion::Left){
        MoveLeft(buffer,1);
    }else if(motion == Motion::Right){
        MoveRight(buffer,1);
    }else if(motion == Motion::Up){
        MoveUp(buffer,1);
    }else if(motion == Motion::Down){
        MoveDown(buffer,1);
    }else if(motion == Motion::MoveToLineHead){
        MoveToLineHead(buffer);
    }else if(motion == Motion::MoveToLineEnd){
        MoveToLineEnd(buffer);
    }else if (motion == Motion::MoveToFirstNoneEmpty){
        MoveToFirstNoneEmpty(buffer);
    }else if (motion == Motion::MoveToLastRowG){
        MoveToLastRowG(buffer);
    }else if(motion == Motion:: MoveToFirstRowgg){
        MoveToFirstRowgg(buffer);
    }else if(motion == Motion:: Move_w){
        OpWw(buffer,true);
    }else if(motion == Motion:: Move_W){
        OpWw(buffer,false);
    }else if(motion == Motion:: Move_e){
        OpEe(buffer,true);
    }else if(motion == Motion:: Move_E){
        OpEe(buffer,false);
    }else if(motion == Motion :: Move_B){
        OpBb(buffer,false);
    }else if(motion == Motion :: Move_b){
        OpBb(buffer,true);
    }else if(motion == Motion :: Move_gE){
        OpgegE(buffer,false);
    }else if(motion == Motion :: Move_ge){
        OpgegE(buffer,true);
    }

    cursor_.row_ = cursor_.row_ > buffer.GetLineCount() - 1? buffer.GetLineCount() - 1: cursor_.row_;

    if(buffer.GetLineAt(cursor_.row_).size() == 0){
        cursor_.column_ = 0;
    }else{
        cursor_.column_ = cursor_.column_ > buffer.GetLineAt(cursor_.row_).size() - 1? buffer.GetLineAt(cursor_.row_).size() - 1:cursor_.column_;
    }
    EnsureCursorVisible(buffer);
}

void Window::EnsureCursorVisible(const Buffer& buffer) {
    //1. 光标高于或低于可见区域时,调整top_,使光标刚好进入区域
    //2. 把光标的字符下标换算成显示列,再用相同思路调整left_
    
    if(cursor_.row_ < viewport_.top_){
        viewport_.top_ = cursor_.row_;
    }
    if(cursor_.row_ >= viewport_.top_ + viewport_.rows_){
        viewport_.top_ = cursor_.row_ - viewport_.rows_ + 1;
    }

    size_t render_column =  BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);

    if(render_column < viewport_.left_){
        viewport_.left_ = render_column;
    }
    if(render_column >= viewport_.left_ + viewport_.columns_ ){
        viewport_.left_ = render_column - viewport_.columns_ + 1;
    }

}

const Position& Window::GetCursor() const {
    //返回当前光标位置的只读引用
    return cursor_;
}

const Viewport& Window::GetViewport() const {
    //返回当前可见区域的只读引用,供Renderer绘制
    return viewport_;
}


void Window::SetCursor(const Buffer& buffer, Position position, bool allow_line_end) {
    //1. 先限制行号,再根据该行长度和allow_line_end限制列号
    //2. 用新位置更新上下移动时的目标显示列
    //3. 调整视口,保证光标可见
    cursor_.row_ = position.row_;
    cursor_.column_ = position.column_;

    size_t extra = allow_line_end? 1:0;

    cursor_.row_ = cursor_.row_ > buffer.GetLineCount() - 1? buffer.GetLineCount() - 1: cursor_.row_;

    if(buffer.GetLineAt(cursor_.row_).size() == 0){
        cursor_.column_ = 0;
    }else{
        cursor_.column_ = cursor_.column_ > buffer.GetLineAt(cursor_.row_).size() - 1 + extra? buffer.GetLineAt(cursor_.row_).size() - 1 + extra:cursor_.column_;
    }

    desired_column_ = BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);


    EnsureCursorVisible(buffer);

}


void Window::MoveLeft(const Buffer& buffer, std::size_t count) {
    //向左移动count个字符,最多到行首,并更新目标显示列
    if(cursor_.column_ > count){
        cursor_.column_ -= count;
    }else{
        cursor_.column_ = 0;
    }

    desired_column_ =  BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
}

void Window::MoveRight(const Buffer& buffer, std::size_t count) {
    //向右移动count个字符,最多到最后一个字符,并更新目标显示列
    if(buffer.GetLineAt(cursor_.row_).size() == 0){
        cursor_.column_ = 0;
    }else{
        if(cursor_.column_ + count < buffer.GetLineAt(cursor_.row_).size() - 1){
            cursor_.column_ += count;
        }else{
            cursor_.column_ = buffer.GetLineAt(cursor_.row_).size() - 1;
        }
    }
    desired_column_ =  BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
}


void Window::MoveUp(const Buffer& buffer, std::size_t count) {
    //先算目标行,最多到第一行,再将期望的显示列换算成目标行的字符下标
    //经过短行时不要更新desired_screen_column_,这样继续移动到长行时能回到原来的列
    if(cursor_.row_ > count){
        cursor_.row_ -= count;
    }else{
        cursor_.row_ = 0;
    }
    size_t column = RenderColumnToBufferColumn(buffer.GetLineAt(cursor_.row_),desired_column_);

    if(buffer.GetLineAt(cursor_.row_).size() == 0){
        column = 0;
    }else{
        column = column < buffer.GetLineAt(cursor_.row_).size() - 1? column: buffer.GetLineAt(cursor_.row_).size()-1;
    }

    cursor_.column_ = column;
}

void Window::MoveDown(const Buffer& buffer, std::size_t count) {
    //先算目标行,最多到最后一行,再根据desired_screen_column_寻找目标字符
    //与向上移动一样,保留期望显示列
    if(cursor_.row_ + count < buffer.GetLineCount() - 1){
        cursor_.row_ += count;
    }else{
        cursor_.row_ = buffer.GetLineCount() - 1;
    }
    size_t column = RenderColumnToBufferColumn(buffer.GetLineAt(cursor_.row_),desired_column_);

    if(buffer.GetLineAt(cursor_.row_).size() == 0){
        column = 0;
    }else{
        column = column < buffer.GetLineAt(cursor_.row_).size() - 1? column: buffer.GetLineAt(cursor_.row_).size()-1;
    }

    cursor_.column_ = column;
}

void Window::MoveToLineHead(const Buffer& buffer){
    //移动到行首
    // 要修改期望显示列
    cursor_.column_ = 0;
    desired_column_ = BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_), cursor_.column_);
}

void Window::MoveToLineEnd(const Buffer& buffer){
    //移动到行尾
    //要修改期望显示列
    std::string s = buffer.GetLineAt(cursor_.row_);
    if(s.size() < 1){
        cursor_.column_ = 0;
    }else{
        cursor_.column_ = buffer.GetLineAt(cursor_.row_).size() - 1;
    }
    desired_column_ = BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
}

void Window::MoveToFirstNoneEmpty(const Buffer& buffer){
    //移动到第一个非空
    std::string s = buffer.GetLineAt(cursor_.row_);
    size_t right = 0;

    if(s.size() == 0){
        cursor_.column_ = 0;
        desired_column_ = BufferColumnToRenderColumn(s,0);
        return;
    }


    while (right < s.size())
    {
        //非空
        if(s[right] != '\t' && s[right] != ' '){
            break;
            //right即为下标
        }
        //后移动，看错题了
        right ++;
    }

    // 全blank特判 //去你妈的单 =。。。
    if(right == s.size()){
        cursor_.column_ = right -1;
    }else{
        cursor_.column_ = right;
    }
    desired_column_ = BufferColumnToRenderColumn(s,cursor_.column_);
}

void Window::MoveToLastRowG(const Buffer& buffer){

    //特判 /空文件
    if(buffer.GetLineCount() == 0){
        cursor_.row_ = 0;
    }else{
        cursor_.row_ = buffer.GetLineCount() - 1;
    }

    //直接用
    MoveToFirstNoneEmpty(buffer);
}

void Window::MoveToFirstRowgg(const Buffer& buffer){
    cursor_.row_ = 0;

    MoveToFirstNoneEmpty(buffer);
}


std::string Window::Classify(char ch , bool mode){
    //mode = true : word
    if(mode){
        if(ispunct(ch) && ch != '_'){ //下划线不算
            return "Punctuation";
        }else if(isspace(ch)){
            return "Blank";
        }else{
            return "WordChar";
        }
    }else{
        if(isspace(ch)){
            return "Blank";
        }else{
            return "NoneBlank";
        }
    }
}

void Window::OpWw(const Buffer& buffer,bool mode){
    cursor_ = NextStart(buffer,mode);
    desired_column_ = BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
}



// 先写一个试试手
// 只找位置不修改
Position Window::NextStart(const Buffer& buffer,bool mode){
    std::string s  =  buffer.GetLineAt(cursor_.row_);
    size_t pos = cursor_.column_;
    std::string initial_state;
    bool changed_row = false;
    if(s.size() == 0){
        // 如果第一个就是空的，那我们的initial_state就是找一个非blank的就行
        initial_state = "Blank";
        changed_row = true;
    }else{
        initial_state = Classify(s[pos],mode);
    } 
    
    size_t row,col ;
    row = cursor_.row_;

    //后面还有
    //不走出文件
    while (row < buffer.GetLineCount()){
        std::string s = buffer.GetLineAt(row);
        while (pos + 1 < s.size()){ // 后面还有字符
            
            if(Classify(s[pos + 1], mode) != initial_state){
                //找到
                if(Classify(s[pos + 1], mode) != "Blank"){
                    col = pos + 1;
                    return {row,col};
                }else{
                    //把状态切换为blank，去扫下一个word
                    initial_state = "Blank";
                }
                
            }else{
                pos ++;
            }
            
        }
        // 退出循环，走到头了换行
        // 换行之后应该重置 initial_state，因为我们换了之后，额，换了之后无论下一个是啥都是新的单词了
        // 因为空行不算word
        pos = 0;
        row ++ ;
        changed_row = true;
        initial_state = "Blank";
        
        

        //如果是没字符的(empty, not blank)
        if(row < buffer.GetLineCount() && buffer.GetLineAt(row).size() == 0){
            // 停在这里
            col = pos;
            return {row,col} ;
        }

        if(row < buffer.GetLineCount() &&changed_row){
            s = buffer.GetLineAt(row);
                if(Classify(s[pos],mode) != initial_state){
                    //第一个就是
                    col = 0;
                    return {row,col};
                }else{
                    //第一个不是
                    changed_row = false;
                    //state不用改。反正换了行就是false
                    //fallback到else让他自己扫就行了
                }
            }
    }

    //前面都没return，证明完全没找到.
    row = buffer.GetLineCount() - 1;
    if(buffer.GetLineAt(row).size() == 0){
        col = 0;
    }else{
        col = buffer.GetLineAt(row).size() -1 ;
    }
    return {row,col};

    /*if(pos < s.size()){
        //后面一个和当前这个一样，证明是同一个块
        if(current_state == Classify(s[pos],mode)){
            while(pos + 1 < s.size() && Classify(s[pos],mode) == Classify(s[pos+1],mode)){
                //当下一个和现在这个一样，没走到头，继续往下走
                pos++;
            }
            //走完了，看情况
            if(pos + 1 == s.size()){
                //走到头也没找到
                // 那就是下一行的第一个
            }else{
                //找到了
                col = pos + 1;
            }
        }
    }*/
}

Position Window::NextEnd(const Buffer& Buffer , bool mode){
    //我草我突然想到我们先找到下一个单词的词头再给他移动到词尾不就行了
    Position next_head = NextStart(Buffer,mode);


    //现在next_head是下个单词的词头
    std::string s = Buffer.GetLineAt(next_head.row_);
    
    if(s.empty()){
        //停在的位置是空行
        //不动
        return next_head;
    }else{
        //停的位置不是空的

        //停在文件尾巴
        if(next_head.column_ == s.size() - 1){
            return next_head; // 不动
        }else{
            //开始扫描这个单词
            //不会再发生换行
            size_t col = next_head.column_;
            std::string initial_state = Classify(s[col],mode);

            while(col < s.size() && initial_state == Classify(s[col],mode)){
                col ++; //还一样就继续往下走
            }

            // 走出来两种情况，第一种就是走到词尾，第二种是出界，无论如何都能用col -1 修复
            col = col -1;
            return {next_head.row_ , col};
        }
    }

}

void Window::OpEe(const Buffer& buffer,bool mode){
    cursor_ = NextEnd(buffer , mode);
    desired_column_ = BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
}


Position Window::PrevStart(const Buffer& Buffer , bool mode){
    std::string s = Buffer.GetLineAt(cursor_.row_);
    size_t pos = cursor_.column_;
    std::string initial_state = Classify(s[pos],mode);

    //先不管空行判断先.

    size_t row, col;
    row = cursor_.row_;

    if(pos > 0 && Classify(s[pos - 1],mode) == initial_state){
        //这个地方的意思是pos指向的位置就是一个普通的中间的地方，找这个地方最前面的字母就行
        // 无需换行
        while (pos > 0 && Classify(s[pos - 1],mode) == initial_state )
        {
            pos --;
        }
        //走完之后，pos要么是0, 要么是词头
        return {row,pos};
    }else{
        //这个地方pos要么是0，要么其已经是词头
        //先去找前面一个词的词尾。
        //找前面词尾的代码直接先去写PrevEnd了
        Position prev_end = PrevEnd(Buffer,mode);
        s = Buffer.GetLineAt(prev_end.row_);
        //找到前面一个词尾了
        pos = prev_end.column_;
        initial_state = Classify(s[pos],mode);

        if(pos == 0){
            //是在第一个，证明这个地方是空行
            return prev_end; //一样的，直接返回
        }else{
            //不在第一个
            while(pos > 0 && Classify(s[pos],mode) == initial_state){
                pos --;
            }
            return {row,pos};
        }
    }
}


void Window::OpBb(const Buffer& Buffer, bool mode){
    cursor_ = PrevStart(Buffer,  mode);

    desired_column_ = BufferColumnToRenderColumn(Buffer.GetLineAt(cursor_.row_), cursor_.column_);
}


Position Window::PrevEnd(const Buffer& Buffer, bool mode){
    size_t row = cursor_.row_;

    std::string s = Buffer.GetLineAt(row);

    size_t before = 0;

    if(s.size() != 0){
        //非空
        size_t pos = cursor_.column_;

        if(Classify(s[pos],mode) == "Blank"){
            //现在在空白
            before = pos;
        } else{
            //现在在词内
            std::string current_state = Classify(s[pos],mode);

            while(pos > 0 && Classify(s[pos - 1], mode) == current_state){
                pos--;
            }

            before = pos;
            
        }
        //现在pos要么在 0，要么在自己的词头
        while(before > 0 && Classify(s[before - 1],mode) == "Blank"){
            //跳过blank
            before --;
        }

        if(before > 0){
            return {row, before - 1};
        }
    }

    // 到头了，往前走
    while(row > 0){
        row --;
        //更新
        s = Buffer.GetLineAt(row);


        if(s.empty()){
            //空的
            return {row,0};
        }

        size_t right = s.size();

        while( right > 0 && Classify(s[right -1], mode) == "Blank"){
            right --;
        }

        //最后看看right位置
        if(right > 0){
            return {row,right -1};
        }
        //没找到，fall back
    }
    return {0,0}; // 全都没找到
    
}

void Window::OpgegE(const Buffer& Buffer, bool mode){
    cursor_ = PrevStart(Buffer,  mode);

    desired_column_ = BufferColumnToRenderColumn(Buffer.GetLineAt(cursor_.row_), cursor_.column_);
}


} // namespace sjtu
