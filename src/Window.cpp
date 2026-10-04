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

void Window::ApplyMotion(const Buffer& buffer, Motion motion,size_t count , bool has_count) {
    //1. 根据方向调用对应的移动函数,Basic中每次移动一步,在Advanced中你可以改变count/添加别的case
    //2. 将行列限制在Normal模式的合法范围内(Buffer应始终至少有一行)
    //3. 调整视口,让移动后的光标可见(EnsureCursorVisible)
    if(motion == Motion::Left){
        MoveLeft(buffer,count);
    }else if(motion == Motion::Right){
        MoveRight(buffer,count);
    }else if(motion == Motion::Up){
        MoveUp(buffer,count);
    }else if(motion == Motion::Down){
        MoveDown(buffer,count);
    }else if(motion == Motion::MoveToLineHead){
        MoveToLineHead(buffer);
    }else if(motion == Motion::MoveToLineEnd){
        MoveToLineEnd(buffer,count);
    }else if (motion == Motion::MoveToFirstNoneEmpty){
        MoveToFirstNoneEmpty(buffer);
    }else if (motion == Motion::MoveToLastRowG){
        MoveToLastRowG(buffer,count,has_count);
    }else if(motion == Motion:: MoveToFirstRowgg){
        MoveToFirstRowgg(buffer,count , has_count);
    }else if(motion == Motion:: Move_w){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpWw(buffer,true);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
    }else if(motion == Motion:: Move_W){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpWw(buffer,false);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
    }else if(motion == Motion:: Move_e){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpEe(buffer,true);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
        
    }else if(motion == Motion:: Move_E){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpEe(buffer,false);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
    }else if(motion == Motion :: Move_B){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpBb(buffer,false);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
        
    }else if(motion == Motion :: Move_b){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpBb(buffer,true);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
    }else if(motion == Motion :: Move_gE){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpgegE(buffer,false);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
        
    }else if(motion == Motion :: Move_ge){
        for(size_t i = 0 ; i < count ; i++){
            Position before = cursor_;
            OpgegE(buffer,true);

            if(cursor_.row_ == before.row_ && cursor_.column_ == before.column_){
                break;
            }
        }
    }

    cursor_.row_ = cursor_.row_ > buffer.GetLineCount() - 1? buffer.GetLineCount() - 1: cursor_.row_;

    if(buffer.GetLineAt(cursor_.row_).size() == 0){
        cursor_.column_ = 0;
    }else{
        cursor_.column_ = cursor_.column_ > buffer.GetLineAt(cursor_.row_).size() - 1? buffer.GetLineAt(cursor_.row_).size() - 1:cursor_.column_;
    }
    EnsureCursorVisible(buffer,true);
}

void Window::EnsureCursorVisible(const Buffer& buffer,bool normal_cursor) {
    //1. 光标高于或低于可见区域时,调整top_,使光标刚好进入区域
    //2. 把光标的字符下标换算成显示列,再用相同思路调整left_
    
    if(cursor_.row_ < viewport_.top_){
        viewport_.top_ = cursor_.row_;
    }
    if(cursor_.row_ >= viewport_.top_ + viewport_.rows_){
        viewport_.top_ = cursor_.row_ - viewport_.rows_ + 1;
    }

    const std::string& s = buffer.GetLineAt(cursor_.row_);
    size_t render_column;

    if(normal_cursor){
        render_column =
            BufferColumnToNormalCursorColumn(s,cursor_.column_);
    }else{
        render_column =
            BufferColumnToRenderColumn(s,cursor_.column_);
    }

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

    if(allow_line_end){
        desired_column_ = BufferColumnToRenderColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
    }else{
        desired_column_ = BufferColumnToNormalCursorColumn(buffer.GetLineAt(cursor_.row_), cursor_.column_);
    }
    


    EnsureCursorVisible(buffer, !allow_line_end);

}


void Window::MoveLeft(const Buffer& buffer, std::size_t count) {
    //向左移动count个字符,最多到行首,并更新目标显示列
    Position before = cursor_;
    if(cursor_.column_ > count){
        cursor_.column_ -= count;
    }else{
        cursor_.column_ = 0;
    }

    desired_column_ =  BufferColumnToNormalCursorColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
    if(cursor_.column_ != before.column_){
        desired_eol_ = false;
    }
}

void Window::MoveRight(const Buffer& buffer, std::size_t count) {
    //向右移动count个字符,最多到最后一个字符,并更新目标显示列
    const std::string& s = buffer.GetLineAt(cursor_.row_);
    Position before = cursor_;

    if(s.empty()){
        cursor_.column_ = 0;
    }else{
        size_t last = s.size() -1;
        size_t remaining = last - cursor_.column_;

        if(count >= remaining){
            cursor_.column_ = last;
        }else{
            cursor_.column_ += count; //防止溢出
        }
    }

    desired_column_ = BufferColumnToNormalCursorColumn(s,cursor_.column_);
    if(cursor_.column_ != before.column_){
        desired_eol_ = false;
    }
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
    
    if(desired_eol_){
        if(buffer.GetLineAt(cursor_.row_).size() == 0){
            column = 0;
        }else{
            column = buffer.GetLineAt(cursor_.row_).size() - 1;
        }
    }


    cursor_.column_ = column;
}

void Window::MoveDown(const Buffer& buffer, std::size_t count) {
    //先算目标行,最多到最后一行,再根据desired_screen_column_寻找目标字符
    //与向上移动一样,保留期望显示列
    size_t last_row = buffer.GetLineCount() - 1;
    size_t remaining = last_row - cursor_.row_;

    if(count >= remaining){
        cursor_.row_ = last_row;
    }else{
        cursor_.row_ += count;
    }

    size_t column = RenderColumnToBufferColumn(buffer.GetLineAt(cursor_.row_),desired_column_);

    std::string s = buffer.GetLineAt(cursor_.row_);

    if(s.empty()){
        column = 0;
    }else{
        column = std::min(column, s.size() - 1);
    }

    if(desired_eol_){
        if(buffer.GetLineAt(cursor_.row_).size() == 0){
            column = 0;
        }else{
            column = buffer.GetLineAt(cursor_.row_).size() - 1;
        }
    }

    cursor_.column_ = column;
}

void Window::MoveToLineHead(const Buffer& buffer){
    //移动到行首
    // 要修改期望显示列
    cursor_.column_ = 0;
    desired_column_ = BufferColumnToNormalCursorColumn(buffer.GetLineAt(cursor_.row_), cursor_.column_);
    desired_eol_ = false;
}

void Window::MoveToLineEnd(const Buffer& buffer, size_t count){
    //移动到行尾
    //要修改期望显示列
    size_t last_row = buffer.GetLineCount() - 1;

    //向下count -1 行
    size_t down = count - 1;
    size_t remaining = last_row - cursor_.row_;
    size_t actual_down = std::min(down,remaining);


    //已经到了最后一行且count > 1，不动
    if(down > 0 && actual_down == 0){
        desired_eol_ = true;
        return;
    }

    cursor_.row_ += actual_down;

    const std::string s = buffer.GetLineAt(cursor_.row_);
    if(s.empty()){
        cursor_.column_ =  0;
    }else{
        cursor_.column_ = s.size() - 1;
    }

    desired_eol_ = true;
}

void Window::MoveToFirstNoneEmpty(const Buffer& buffer){
    //移动到第一个非空
    std::string s = buffer.GetLineAt(cursor_.row_);
    size_t right = 0;

    if(s.size() == 0){
        cursor_.column_ = 0;
        desired_column_ = BufferColumnToNormalCursorColumn(s,0);
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
    desired_column_ = BufferColumnToNormalCursorColumn(s,cursor_.column_);
    desired_eol_ = false;
}

void Window::MoveToLastRowG(const Buffer& buffer,size_t count , bool has_count){

    //特判 /空文件
    if(buffer.GetLineCount() == 0){
        cursor_.row_ = 0;
    }else{
        //没有count
        if(!has_count){
            cursor_.row_ = buffer.GetLineCount() -1;
        }else{
            if(count > buffer.GetLineCount()){
                cursor_.row_ = buffer.GetLineCount() -1;
            }else{
                cursor_.row_ = count - 1;
            }
        }
    }

    //直接用
    MoveToFirstNoneEmpty(buffer);
}

void Window::MoveToFirstRowgg(const Buffer& buffer,size_t count , bool has_count){
    if(buffer.GetLineCount() == 0){
        cursor_.row_ = 0;
    }else{
        //没有count
        if(!has_count){
            cursor_.row_ = 0;
        }else{
            if(count > buffer.GetLineCount()){
                cursor_.row_ = buffer.GetLineCount() -1;
            }else{
                cursor_.row_ = count - 1;
            }
        }
    }

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
    desired_column_ = BufferColumnToNormalCursorColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
    desired_eol_ = false;
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
    size_t row = cursor_.row_;
    size_t pos = cursor_.column_;

    std::string s = Buffer.GetLineAt(row);

    if(s.size() != 0){
        std::string current_state = Classify(s[pos], mode);

        //如果当前就在一个word里面，那直接去自己的结尾
        if(current_state != "Blank"){
            if(pos + 1 < s.size() && Classify(s[pos+1], mode) == current_state){
                // pos + 1 没有越界， pos+1的状态和自己相同，自己在内部
                while(pos + 1 < s.size() && Classify(s[pos + 1], mode) == current_state){
                    pos ++;
                }
                return {row,pos};
            }
            // pos + 1
            pos ++;                
        }
        
    }else{
        //空行
        row ++;
        pos  = 0;
    }

    while(row < Buffer.GetLineCount()){
        //找下面的
        s = Buffer.GetLineAt(row);

        if(s.empty()){
            //还要继续跳
            row ++;
            pos = 0;
            continue;
        }

        //扫描
        while(pos < s.size() && Classify(s[pos], mode) == "Blank"){
            pos ++;
        }
        //找到词了
        if(pos < s.size()){
            std::string state = Classify(s[pos], mode);

            while(pos + 1 < s.size() && Classify(s[pos + 1], mode) == state){
                pos ++;
            }

            return {row, pos};
        }
        //没找到
        row  ++;
        pos = 0;
    }
    //没找到
    row = Buffer.GetLineCount() - 1;
    s = Buffer.GetLineAt(row);

    if(s.empty()){
        return {row, 0};
    }
    return {row, s.size() -1 };

}

void Window::OpEe(const Buffer& buffer,bool mode){
    cursor_ = NextEnd(buffer , mode);
    desired_column_ = BufferColumnToNormalCursorColumn(buffer.GetLineAt(cursor_.row_),cursor_.column_);
    desired_eol_ = false;
}


Position Window::PrevStart(const Buffer& Buffer , bool mode){
    size_t row = cursor_.row_;
    size_t pos = cursor_.column_;

    std::string s = Buffer.GetLineAt(row);

    if(s.size() != 0){
        std::string current_state = Classify(s[pos], mode);
        //blank不行
        if(current_state != "Blank" && pos > 0 && Classify(s[pos-1],mode) == current_state){
            //现在是在一个词内

            while(pos > 0 && Classify(s[pos - 1], mode) == current_state){
                pos --;
            }

            return {row , pos};
        }
    }

    //上面找到了词内的情况，那现在我们都要找前一个word了
    Position prev_end = PrevEnd(Buffer, mode);

    s = Buffer.GetLineAt(prev_end.row_);

    if(s.empty()){
        //前面的词也是空的
        return prev_end; // 一样
    }

    pos = prev_end.column_;

    std::string state = Classify(s[pos], mode);
    // 从前面的end到head
    while (pos > 0 && Classify(s[pos - 1],mode) == state)
    {
        pos --;
    }
    return {prev_end.row_, pos};
    
}


void Window::OpBb(const Buffer& Buffer, bool mode){
    cursor_ = PrevStart(Buffer,  mode);

    desired_column_ = BufferColumnToNormalCursorColumn(Buffer.GetLineAt(cursor_.row_), cursor_.column_);
    desired_eol_ = false;
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
    cursor_ = PrevEnd(Buffer,  mode);

    desired_column_ = BufferColumnToNormalCursorColumn(Buffer.GetLineAt(cursor_.row_), cursor_.column_);
    desired_eol_ = false;
}


} // namespace sjtu
