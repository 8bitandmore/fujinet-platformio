#include "atari_1025.h"
#include "../../include/debug.h"

void atari1025::pdf_handle_char(byte c)
{
    if (escMode)
    {
        // Atari 1025 escape codes:
        // ESC CTRL-T - 16.5 char/inch        0x14
        // ESC CTRL-O - 10 char/inch          0x0F
        // ESC CTRL-N - 5 char/inch           0x0E
        // ESC L - long line 80 char/line     0x4C
        // ESC S - short line 64 char/line    0x53
        // ESC 6 - use 6 lines per inch       0x36
        // ESC 8 - use 8 lines per inch       0x38
        // ESC CTRL-W - start international   0x17 23
        // ESC CTRL-X - stop international    0x18 24

        switch (c)
        {
        case 0x0E:
            // change font to elongated like
            if (fontNumber != 2)
            {
                _file.printf(")]TJ\n/F2 12 Tf [(");
                charWidth = 14.4; //72.0 / 5.0;
                fontNumber = 2;
                fontUsed[1]=true;
            }
            break;
        case 0x0F:
            // change font to normal
            if (fontNumber != 1)
            {
                _file.printf(")]TJ\n/F1 12 Tf [(");
                charWidth = 7.2; //72.0 / 10.0;
                fontNumber = 1;
                // fontUsed[0]=true; // redundant
            }
            break;
        case 0x14:
            // change font to compressed
            if (fontNumber != 3)
            {
                _file.printf(")]TJ\n/F3 12 Tf [(");
                charWidth = 72.0 / 16.5;
                fontNumber = 3;
                fontUsed[2]=true;
            }
            break;
        case 0x17: // 23
            intlFlag = true;
            break;
        case 0x18: // 24
            intlFlag = false;
            break;
        case 0x36:             // '6'
            lineHeight = 12.0; //72.0/6.0;
            break;
        case 0x38:            // '8'
            lineHeight = 9.0; //72.0/8.0;
            break;
        case 0x4c: // 'L'
            /* code */
            // for long and short lines, i think we end line, ET, then set the leftMargin and pageWdith and begin text
            // challenge is to not skip a line if we're at the beginning of a line
            // could also add a state variable so we don't unnecessarily change the line width
            if (shortFlag)
            {
                if (!BOLflag)
                    pdf_end_line();   // close out string array
                _file.printf("ET\n"); // close out text object
                // set new margins
                leftMargin = 18.0;  // (8.5-8.0)/2*72
                printWidth = 576.0; // 8 inches
                pdf_begin_text(pdf_Y);
                // start text string array at beginning of line
                _file.printf("[(");
                BOLflag = false;
                shortFlag = false;
            }
            break;
        case 0x53: // 'S'
            // for long and short lines, i think we end line, ET, then set the leftMargin and pageWdith and begin text
            if (!shortFlag)
            {
                if (!BOLflag)
                    pdf_end_line();   // close out string array
                _file.printf("ET\n"); // close out text object
                // set new margins
                leftMargin = 75.6;  // (8.5-6.4)/2.0*72.0;
                printWidth = 460.8; //6.4*72.0; // 6.4 inches
                pdf_begin_text(pdf_Y);
                // start text string array at beginning of line
                _file.printf("[(");
                BOLflag = false;
                shortFlag = true;
            }
            break;
        default:
            break;
        }

        escMode = false;
    }
    else if (c == 27)
        escMode = true;
    else
    { // maybe printable character
        //printable characters for 1027 Standard Set + a few more >123 -- see mapping atari on ATASCII
        if (intlFlag && (c < 32 || c == 96 || c == 123 || c == 126 || c == 127))
        {
            bool valid = false;
            byte d = 0;

            if (c < 27)
            {
                d = intlchar[c];
                valid = true;
            }
            else if (c > 27 && c < 32)
            {
                // Codes 28-31 are arrows
                d = c;
                valid = true;
            }
            else
                switch (c)
                {
                case 96:
                    d = byte(161);
                    valid = true;
                    break;
                case 123:
                    d = byte(196);
                    valid = true;
                    break;
                case 126:
                    d = byte(182); // service manual shows EOL ATASCII symbol
                    valid = true;
                    break;
                case 127:
                    d = byte(171); // service manual show <| block arrow symbol
                    valid = true;
                    break;
                default:
                    valid = false;
                    break;
                }
            if (valid)
            {
                _file.write(d);
                pdf_X += charWidth; // update x position
            }
        }
        else if (c > 31 && c < 127)
        {
            if (c == '\\' || c == '(' || c == ')')
                _file.write('\\');
            _file.write(c);
            pdf_X += charWidth; // update x position
        }
    }
}

void atari1025::initPrinter(FS *filesystem)
{
    printer_emu::initPrinter(filesystem);

    shortname = "a1025";

    pageWidth = 612.0;
    pageHeight = 792.0;
    leftMargin = 18.0;
    bottomMargin = 0;
    printWidth = 576.0; // 8 inches
    lineHeight = 12.0;
    charWidth = 7.2;
    fontNumber = 1;
    fontSize = 12;
    
    pdf_header();
    intlFlag = false;
    escMode = false;
}