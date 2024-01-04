#ifndef __CONFIG_FILE_READER_H__
#define __CONFIG_FILE_READER_H__

#include <map>
#include <string>

class CConfigFileReader
{
public:
    CConfigFileReader(const char *filename);
    ~CConfigFileReader() = default;

    char *getConfigName(const char *name);
    int setConfigValue(const char *name, const char *value);

    std::map<std::string, std::string> const &getConfigMap() const
    {
        return m_config_map;
    }

private:
    void loadFile(const char *filename);
    int writeFile(const char *filename = NULL);
    void parseLine(char *line);
    char *trimSpace(char *name);

    bool m_load_ok;
    std::map<std::string, std::string> m_config_map;
    std::string m_config_file;
};

#endif //!__CONFIG_FILE_READER_H__