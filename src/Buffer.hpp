#ifndef MINIVIM_BUFFER_HPP
#define MINIVIM_BUFFER_HPP

#include <filesystem>
#include <vector>

namespace sjtu {

class Buffer {
/*
Buffer.cpp
Buffer即缓冲区.在我们的MiniVim中,Buffer是用来存储文件的实际内容的
我们保证在Buffer中只含有可打印字符,即[0x20, 0x7E]以及\t
我们约定,当你MiniVim打开一个不含有内容的文件时,不应该让lines_.empty(),而应该让lines含有一个空的string作为占位.即表明文件当前只有一行且是空行
*/
public:
    explicit Buffer(const std::filesystem::path& path = {});
    explicit Buffer(std::vector<std::string> lines, std::filesystem::path path = {});

    std::size_t GetLineCount() const;
    const std::string& GetLineAt(std::size_t row) const;
    void ReplaceCharacter(size_t row, size_t col, char ch);
    std::string GetDisplayName() const;
    bool IsModified() const ;

    void InsertCharacter(std::size_t row, std::size_t column, char value);
    void EraseCharacter(std::size_t row, std::size_t column);
    void SplitLine(std::size_t row, std::size_t column);
    void JoinLine(std::size_t row);
    void Save();
    void SaveAs(const std::filesystem::path& path);

private:
    void WriteTo(const std::filesystem::path& path) const;

    std::vector<std::string> lines_; //文件每行的字符内容,不包含末尾换行符
    std::filesystem::path path_;    //打开文件的路径
    bool modified_;
};

} // namespace sjtu

#endif // MINIVIM_BUFFER_HPP
