#include <fstream>

#include "Buffer.hpp"

namespace sjtu {

Buffer::Buffer(const std::filesystem::path& path){
    //从path指向的文件构造Buffer,你需要打开文件并且把文件内容填充进Buffer,并正确初始化一些状态.
    //注意path可能为空的边界情况
    path_ = path;
    modified_ = false;
    std::ifstream fin(path);

    std::string line;
    while(std::getline(fin,line)){
        lines_.push_back(line);
    }

    if(lines_.empty()){
        lines_={""};
    }
}

Buffer::Buffer(std::vector<std::string> lines, std::filesystem::path path) {
    if(lines.empty()){
        lines_ = {""};
    }else{
        lines_ = lines;
    }
    path_ = path;
    modified_ = false;
}

std::size_t Buffer::GetLineCount() const {
    //返回文件行数
    return lines_.size();
}

const std::string& Buffer::GetLineAt(std::size_t row) const {
    //返回第row行的内容
    return lines_[row];
}


std::string Buffer::GetDisplayName() const {
    //返回文件名,若是新文件,返回"[No Name]"
    if(path_.empty()){
        return "[No Name]";
    }else{
        return path_.filename().string();
    }
}

bool Buffer::IsModified() const {
    //返回文件和上次保存比起来是否被修改过
    return modified_;
}

void Buffer::InsertCharacter(std::size_t row, std::size_t column, char value) {
    //在第row行第col列插入一个value, 注意越界检查
    if(row >= lines_.size()){
        throw std::runtime_error("invalid row num");
    }else if(column > lines_[row].size()){
        throw std::runtime_error("invalid column num");
    }

    std::string s = lines_[row];

    s.insert(column, 1, value);
    lines_[row] = s;

    modified_ = true;
}

void Buffer::EraseCharacter(std::size_t row, std::size_t column) {
   //在第row行第col列删除一个value
    if(row >= lines_.size()){
        throw std::runtime_error("invalid row num");
    }else if(column >= lines_[row].size()){
        throw std::runtime_error("invalid column num");
    }

    std::string s = lines_[row];
    s.erase(column, 1);
    lines_[row] = s;
    modified_ = true;
}

void Buffer::SplitLine(std::size_t row, std::size_t column) {
    //在第row行第col列分割,即在此处敲了回车键
    if(row >= lines_.size()){
        throw std::runtime_error("invalid row num");
    }else if(column >    lines_[row].size()){
        throw std::runtime_error("invalid column num");
    }

    std::string s = lines_[row];
    std::string second = s.substr(column);

    lines_[row].erase(column);

    lines_.insert(lines_.begin() + row + 1,second);

    modified_ = true;
 
}

void Buffer::JoinLine(std::size_t row) {
   //把第row + 1行合并进第row行
    if(row >= lines_.size() - 1){
        throw std::runtime_error("invalid row num");
    }

    std::string s1 = lines_[row];
    std::string s2 = lines_[row +  1];

    std::string s = s1 + s2;
    
    lines_[row] = s;

    lines_.erase(lines_.begin() + row + 1);
    

    modified_ = true;
    
}

void Buffer::Save() {
   //把文件内容保存, 直接调用WriteTo方法
    WriteTo(path_);
    modified_ = false;
}

void Buffer::SaveAs(const std::filesystem::path& path) {
    WriteTo(path);
    path_ = path;
    modified_ = false;
}


void Buffer::WriteTo(const std::filesystem::path& path) const {
   //实际将缓冲区中的内容写入path指向的文件中
    std::ofstream fout(path);

    if(!fout){
        throw std::runtime_error("file can't be open");
    }

    // 当只输入一行并且空的时候不需要存 ";\n"
    if (lines_.size() == 1 && lines_.front().empty()) {
        return;
    }

    for(size_t i = 0 ; i < lines_.size() - 1; i++){
        if(lines_[i].empty()){
            fout << ";\n";
        }else{
            fout<< lines_[i]<<"\n";
        }
    }
    if(lines_[lines_.size()-1].empty()){
        fout<<";\n";
    }else{
        fout<< lines_[lines_.size()-1]<<"\n";
    }
    
}

} // namespace sjtu
