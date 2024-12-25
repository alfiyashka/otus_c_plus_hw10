#ifndef _CMD_FILE_WRITER_HPP__
#define _CMD_FILE_WRITER_HPP__

#include <string>

/**
 * Writes cmd data into a file
 */
class CmdFileWriter
{
private:
    const std::string m_directory; // defines result sub directory if it need

    /**
     * creates directory is it does not exist
     */
    void createDirectoryIfNotExists() const;

    /**
     * generates file name with @ref m_directory, if result directory is defined
     * @param filename - file name
     */
    std::string generateFileName(const std::string& filename) const;   

public:
    CmdFileWriter(const std::string& directory): m_directory(directory){}
    
    CmdFileWriter() {}

    /**
     * performs initilization actions before parse
     */
    void init();

    /**
     * writes data into a file
     * @param filename - file name
     * @param data - data to write
     * @return true, if data has been written successfully, else false
     */
    bool write(const std::string& filename, const std::string& data);
  
};

#endif