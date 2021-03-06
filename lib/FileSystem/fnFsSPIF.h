#ifndef _FN_FSSPIF_
#define _FN_FSSPIF_

#include <dirent.h>
#include "fnFS.h"
class FileSystemSPIFFS : public FileSystem
{
private:
    DIR * _dir;
public:
    bool start();
    
    fsType type() override { return FSTYPE_SPIFFS; };
    const char * typestring() override { return type_to_string(FSTYPE_SPIFFS); };

    virtual bool is_global() override { return true; };    

    FILE * file_open(const char* path, const char* mode = FILE_READ) override;

    bool exists(const char* path) override;

    bool remove(const char* path) override;

    bool rename(const char* pathFrom, const char* pathTo) override;

    bool dir_open(const char * path) override;
    fsdir_entry *dir_read() override;
    void dir_close() override;

    uint64_t total_bytes();
    uint64_t used_bytes();
};

extern FileSystemSPIFFS fnSPIFFS;

#endif // _FN_FSSPIF_
