/* =========================================================================================

   This is an auto-generated file: Any edits you make may be overwritten!

*/

#pragma once

namespace BinaryData
{
    extern const char*   Grand_staff_02_svg;
    const int            Grand_staff_02_svgSize = 7667;

    extern const char*   InconsolataBold_ttf;
    const int            InconsolataBold_ttfSize = 102152;

    extern const char*   Whole_note_svg;
    const int            Whole_note_svgSize = 1327;

    extern const char*   Grand_staff_DM_03_svg;
    const int            Grand_staff_DM_03_svgSize = 7548;

    extern const char*   Natural_svg;
    const int            Natural_svgSize = 1739;

    extern const char*   Sharp_svg;
    const int            Sharp_svgSize = 1505;

    extern const char*   Flat_svg;
    const int            Flat_svgSize = 7869;

    extern const char*   InconsolataRegular_ttf;
    const int            InconsolataRegular_ttfSize = 101752;

    // Number of elements in the namedResourceList and originalFileNames arrays.
    const int namedResourceListSize = 8;

    // Points to the start of a list of resource names.
    extern const char* namedResourceList[];

    // Points to the start of a list of resource filenames.
    extern const char* originalFilenames[];

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding data and its size (or a null pointer if the name isn't found).
    const char* getNamedResource (const char* resourceNameUTF8, int& dataSizeInBytes);

    // If you provide the name of one of the binary resource variables above, this function will
    // return the corresponding original, non-mangled filename (or a null pointer if the name isn't found).
    const char* getNamedResourceOriginalFilename (const char* resourceNameUTF8);
}
