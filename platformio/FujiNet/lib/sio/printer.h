#ifndef PRINTER_H
#define PRINTER_H
#include <Arduino.h>
#include <string.h>
#include <FS.h>

#include "sio.h"

#define EOL 155
#define BACKSLASH 92
#define LEFTPAREN 40
#define RIGHTPAREN 41
#define BUFN 40

enum printer_t
{
    A820,
    A822,
    A825,
    A1020,
    A1025,
    A1027,
    EMX80
};

enum paper_t
{
    RAW,
    TRIM,
    ASCII,
    PDF
};

static byte intlchar[27] = {225,249,209,201,231,244,242,236,163,239,252,228,214,250,243,246,220,226,251,238,233,232,241,234,229,224,197};

class sioPrinter : public sioDevice
{
private:
    byte buffer[40];
    void sio_write();
    void sio_status() override;
    void sio_process() override;

    paper_t paperType = PDF;
    int pageWidth = 612;
    int pageHeight = 792;
    int leftMargin = 18;
    int bottomMargin = 2;
    int maxLines = 66;
    int maxCols = 80;
    int lineHeight = 12;
    int fontSize = 12;
    const char *fontName = "Courier";
    int pdf_lineCounter = 0;
    u_long pdf_offset = 0;    // used to store location offset to next object
    u_long objLocations[137]; // reference table storage - set >=2*maxLines+5
    int pdf_objCtr = 0;    // count the objects
    bool eolFlag = false;
    bool intlFlag = false;
    bool uscoreFlag = false;
    bool escMode = false;

    void processBuffer(byte *B, int n);

    void pdf_header();
    void pdf_xref();
    void pdf_add_line(std::u16string L);
    std::u16string buffer_to_string(byte *S);
    std::u16string output;
    int j;

    File *_file;

public:
    //sioDisk(){};
    //sioDisk(int devnum=0x31) : _devnum(devnum){};
    // void mount(File *f);
    // void handle();
    void initPrinter(File *f, paper_t ty);
    void initPrinter(File *f);
    void pageEject();
};

#endif // guard