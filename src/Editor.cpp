#include "Editor.hpp"

#include <algorithm>
#include <cctype>
#include <exception>

//以下提示均可用于Basic部分的实现,部分函数在Advanced部分中需要修改
//在该文件中,有些函数我们完整保留了正确的实现,有些函数去掉了一些,其余的完全需要你自己填写
namespace sjtu {

namespace {
//匿名namespace,提供了只给当前文件使用的辅助函数

std::string Trim(std::string value) {
    //去掉字符串两端的空白,保留中间的内容;全部是空白时返回空字符串
    //你可以分别从两端找到第一个非空白字符,注意反向迭代器转回正向迭代器时的边界
    size_t left = 0;
    size_t right = value.size();

    if(right == 0){
        return "";
    }
    
    while (left < value.size())
    {
        if(value[left] != ' ' && value[left] != '\t'){
            break;
        }
        left ++;
    }
    // left是第一个不是空格的下标
    
    while (right > 0)
    {
        right --;
        if(value[right] != ' ' && value[right] != '\t'){
            break;
        }
    }
    // right是最后一个不是空格的下标
    if(right == 0 && (value[right] == ' ' || value[right] == '\t')){
        return "";
    }else{
        //切片，长度为 right - left + 1
        return value.substr(left, right -left + 1);
    }
}

//判断是否为ASCII可打印字符,Tab由插入模式另外处理
bool IsPrintable(char value) { return value >= 0x20U && value < 0x7FU; }

} // namespace

//用path初始化文件缓冲区,Terminal构造时会准备好终端输入环境
Editor::Editor(const std::filesystem::path& path) : buffer_(path), terminal_() {}

void Editor::Run() {
    //每轮先刷新画面,再读取并处理一个按键;退出循环后清屏
    while (running_) {
        RefreshScreen();
        ProcessKey(terminal_.ReadKey());
    }
    terminal_.ClearScreen();
}

//返回编辑器是否还需要继续运行
bool Editor::IsRunning() const noexcept { return running_; }

void Editor::RefreshScreen() {
    //1. 获取终端大小(GetScreenSize),更新窗口可显示的范围,并让光标落在可见区域内
    //2. 把当前模式、命令和提示打包成RenderState
    //3. 让Renderer生成一帧字符串,再交给Terminal输出
    //这里是你唯一需要调用Terminal中的接口的地方,请调用WriteOutput

    // 1
    ScreenSize terminal_size = terminal_.GetScreenSize();

    window_.Resize(terminal_size);
    window_.EnsureCursorVisible(buffer_);
    // 2

    RenderState renderState = {mode_,command_,message_};

    std::string frame = renderer_.Render(buffer_,window_,renderState);

    // 3
    terminal_.WriteOutput(frame);

}

void Editor::ProcessKey(KeyEvent key) {
    //1. (可选)先处理所有模式都能使用的Ctrl-Q,用于紧急退出
    //2. 按当前模式分发给命令行或插入模式的处理函数
    //3. Normal模式下清除旧提示,将按键交给parser,再执行生成的Action
    if(key.IsControl('q')){
        running_ = false;
        return;
    }
    if(mode_ == Mode::Insert){
        HandleInsert(key);
        return;
    }
    if(mode_ == Mode::CommandLine){
        HandleCommandLine(key);
        return;
    }
    if(mode_ == Mode::Normal){
        message_.clear();

        EditorAction action = normal_parser_.Feed(key);
        Execute(action);
        return;
    }

}

void Editor::Execute(const EditorAction& action) {
    //根据Action的种类调用对应模块
    switch (action.kind_) {
    case ActionKind::None:{
        return;
        }
    case ActionKind::Move:{
        //交给Window吧
        window_.ApplyMotion(buffer_,*action.motion_);
        return;
        }
    case ActionKind::InsertBefore:{
        //进入InsertMode,Editor自己就有对应方法
        Position cursor =  window_.GetCursor();
        EnterInsert(cursor);
        return;
        }
    case ActionKind::InsertAfter: {
        //从当前Char之后进入InsertMode
        //特判:如果Buffer表示当前Cursor所在的行是空的怎么办?
        Position cursor = window_.GetCursor();

        if(buffer_.GetLineAt(cursor.row_).size() == 0){
            cursor.column_ = 0;
        }else{
            cursor.column_ ++;
        }
        EnterInsert(cursor);

        return;
        }
    case ActionKind::EnterCommandLine:{
        //进入Command Mode
        //记得清空当前的message之类的遗留状态
        command_.clear();
        message_.clear();
        mode_ = Mode::CommandLine;
        return;
        }
    case ActionKind::InsertFirstNoneBlank:{
        EnterFirstNoneEmptyInsert();
        return;
        }
    
    case ActionKind::AppendLineEnd:{
        EnterLastWordInsert();
        return;
        }
    case ActionKind::OpenBelow:{
        OpenBelow();
        return;
    }
    case ActionKind::OpenAbove:{
        OpenAbove();
        return;
    }
    }

}

//Insert模式下Editor对于KeyEvent的处理.
void Editor::HandleInsert(KeyEvent key) {
    //在该函数中你需要同时照顾Buffer和Window的状态
    //1. 若是Escape退出插入模式
    //2. Enter在光标处分行,光标移动到新行开头
    //3. Backspace删除前一个字符;若在行首且不是第一行,则与上一行合并
    //4. 可打印字符和Tab插入当前位置,光标向后移动一列
    //修改内容后记得同步Window中的光标,插入模式允许光标位于line.size()
    if(key.code_ == KeyCode::Escape){
        LeaveInsert();
        return;
    }

    if(key.code_ == KeyCode::Enter){
        Position cursor = window_.GetCursor();
        buffer_.SplitLine(cursor.row_,cursor.column_);
        cursor.row_++;
        cursor.column_= 0;
        window_.SetCursor(buffer_,cursor,true);
        return;
    }

    if(key.code_ == KeyCode::Backspace){
        Position cursor = window_.GetCursor();
        if(cursor.column_ == 0){
            if(cursor.row_ != 0){
                cursor.column_ = buffer_.GetLineAt(cursor.row_ -1).size();
                buffer_.JoinLine(cursor.row_ - 1);
                cursor.row_ --;                
            }
        }else{
            buffer_.EraseCharacter(cursor.row_, cursor.column_-1);
            cursor.column_ --;
        }
        window_.SetCursor(buffer_,cursor,true);
        return;

    }

    if(key.code_ == KeyCode::Delete){
        Position cursor = window_.GetCursor();

        std::string s = buffer_.GetLineAt(cursor.row_);

        // cursor在行尾
        if(cursor.column_ == s.size()){
            // row不是最后一行
            if(cursor.row_ != buffer_.GetLineCount() -1){
                //合并
                buffer_.JoinLine(cursor.row_);
            }
            //是最后一行，则什么都不做
        }else{
            //不在行尾，直接删除
            buffer_.EraseCharacter(cursor.row_, cursor.column_ );
        }
        window_.SetCursor(buffer_,cursor,true);
        return;
    }


    if(key.code_ == KeyCode::Character &&  (IsPrintable(key.value_) || key.value_ == '\t')){
        Position cursor = window_.GetCursor();
        buffer_.InsertCharacter(cursor.row_,cursor.column_, key.value_);
        cursor.column_ ++;
        window_.SetCursor(buffer_,cursor,true);
        return;
    }
}

void Editor::EnterInsert(Position position) {
    //切换到Insert模式,设置插入位置并清除旧提示;允许光标停在行尾字符之后
    message_.clear();
    mode_ = Mode::Insert;
    window_.SetCursor(buffer_,position,true);
    return;
}

void Editor::EnterFirstNoneEmptyInsert(){
    //切换到First Insert 模式
    Position cursor = window_.GetCursor();
    const std::string s = buffer_.GetLineAt(cursor.row_);

    size_t pos = 0;
    while(pos < s.size() && (s[pos] == ' ' || s[pos] == '\t')){
        pos ++;
    }

    //普通行停在第一个非空
    //全空留在行尾
    cursor.column_ = pos;
    
    EnterInsert(cursor);
}

void Editor::EnterLastWordInsert(){
    //Last Insert
    window_.ApplyMotion(buffer_,Motion::MoveToLineEnd);

    Position cursor = window_.GetCursor();
    //在最后一个字符之后
    if(buffer_.GetLineAt(cursor.row_).size() != 0){
        cursor.column_ ++;
    }

    EnterInsert(cursor);
    return;
}

void Editor::OpenBelow(){
    //移动到行尾,split
    Position cursor = window_.GetCursor();
    
    size_t end = buffer_.GetLineAt(cursor.row_).size();

    buffer_.SplitLine(cursor.row_,end);

    cursor.column_ = 0;
    cursor.row_ ++;
    EnterInsert(cursor);
    return;
}

void Editor::OpenAbove(){
    //移动到行首，split
    Position cursor = window_.GetCursor();

    buffer_.SplitLine(cursor.row_,0);

    cursor.column_ = 0;

    EnterInsert(cursor);
    return;
}


void Editor::LeaveInsert() {
    //从插入位置回到Normal模式的字符位置:不在行首时先左移一列,再限制光标范围
    if(window_.GetCursor().column_ != 0){
        window_.ApplyMotion(buffer_,Motion::Left);
    }

    window_.SetCursor(buffer_,window_.GetCursor(),false);
    mode_ = Mode::Normal;
    return;
}



void Editor::HandleCommandLine(KeyEvent key) {
    //命令内容保存在command_中,不修改Buffer
    //Escape取消命令,Enter执行命令,Backspace/Delete删除末尾字符,可打印字符追加到末尾
    //注意空命令不能再删除字符
    if(key.code_ == KeyCode::Escape){
        LeaveCommandLine();
        return;
    }
    if(key.code_ == KeyCode::Enter){
        ExecuteCommandLine();
        return;
    }
    if(key.code_ == KeyCode::Backspace || key.code_ == KeyCode::Delete){
        if(command_.size() > 0) command_.pop_back();
        return;
    }
    if(key.code_ == KeyCode::Character && IsPrintable(key.value_)){
        command_ += key.value_;
        return;
    }

}

void Editor::ExecuteCommandLine() {
    //1. 保存去掉首尾空白后的命令(用trim),再退出命令行模式,因为退出会清空command_
    //2. 空命令直接返回,否则按第一个空格或Tab拆成命令名和参数
    //3. 在Basic部分中你会发现最后命令就一个命令名,直接根据要求的命令名执行
    //4. 无法识别的命令写入message_,供下一次刷新显示
    std::string command = Trim(command_);
    LeaveCommandLine();

    if (command.empty()) {
        return;
    }

    size_t pos = command.find_first_of(" \t");

    std::string name;
    std::string argument;

    if (pos == std::string::npos) {
        name = command;
    } else {
        name = command.substr(0, pos);
        argument = Trim(command.substr(pos + 1));
    }

    if (name == "q!" || name == "quit!") {
        running_ = false;
        return;
    }

    if (name == "wq") {
        std::filesystem::path path =
            argument.empty()
                ? std::filesystem::path{}
                : std::filesystem::path(argument);

        if (SaveBuffer(path)) {
            running_ = false;
        }
        return;
    }
}

void Editor::LeaveCommandLine() {
    //恢复Normal模式并清空正在输入的命令
    command_ = "";
    mode_ = Mode::Normal;
}


bool Editor::SaveBuffer(const std::filesystem::path& path) {
    //1. path为空时调用Save,否则调用SaveAs
    //2. 捕获保存时的异常,把错误写入message_并返回false
    //3. 成功后生成包含文件名和行数的提示,返回true,供wq判断是否可以退出
    try
    {
        if(path.empty()){
            buffer_.Save();
        }else{
            buffer_.SaveAs(path);
        }
    }
    catch(const std::exception& e)
    {
        message_ += e.what();
        return false;
    }
    
    message_ += buffer_.GetDisplayName() ;
    message_ += std::to_string(buffer_.GetLineCount());

    return true;

}

} // namespace sjtu
