#include "../include/cmd.parse/CmdFileWriter.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>

void CmdFileWriter::createDirectoryIfNotExists() const
{
    if (!std::filesystem::exists(m_directory))
    {
        auto res = std::filesystem::create_directory(m_directory);
        if (!res)
        {
            throw std::runtime_error("cannot create directory");
        }
    }
}
std::string CmdFileWriter::generateFileName(const std::string &filename) const
{
    if (m_directory.empty())
    {
        return filename;
    }
    if (m_directory[m_directory.length() - 1] == '/')
    {
        return m_directory + filename;
    }
    return m_directory + "/" + filename;
}

void CmdFileWriter::init()
{
    if (!m_directory.empty())
    {
        createDirectoryIfNotExists();
    }
}

bool CmdFileWriter::write(const std::string &filename, const std::string &data)
{
    if (filename.empty())
    {
        std::cerr << "Cannot write the input data, because filename is not defined" << std::endl;
        return true;
    }
    if (data.empty())
    {
        std::cerr << "A file with name " << filename << " nas no data" << std::endl;
        return false;
    }
    const auto generatedFilename = generateFileName(filename);
    std::ofstream file;
    file.open(generatedFilename, std::fstream::out);
    file << data;
    file.close();
    return true;
}