#include "Platform.h"

#include <FL/platform.H>

namespace MapAreaCalculator
{
    void errorMessage(std::string_view message)
    {
        HWND hwnd = GetActiveWindow();
        MessageBox(hwnd, message.data(), "Error", MB_OK | MB_ICONERROR);
    }

    std::string importImagePath()
    {
        OPENFILENAME ofn;
        char         file[MAX_PATH] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = NULL;
        ofn.lpstrFilter = "Picture\0*.bmp;*.jpg;*.jpeg;*.png;*.pnm;*.pbm;*.pgm;*.ppm\0";
        ofn.lpstrFile   = file;
        ofn.nMaxFile    = sizeof(file);
        ofn.Flags       = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        GetOpenFileName(&ofn);

        return file;
    }

    std::string importVerticesPath()
    {
        OPENFILENAME ofn;
        char         file[MAX_PATH] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = NULL;
        ofn.lpstrFilter = "Text\0*.txt\0";
        ofn.lpstrFile   = file;
        ofn.nMaxFile    = sizeof(file);
        ofn.Flags       = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        GetOpenFileName(&ofn);

        return file;
    }

    std::string exportImagePath()
    {
        OPENFILENAME ofn;
        char         file[MAX_PATH] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = NULL;
        ofn.lpstrFilter = ".png\0*.png\0";
        ofn.lpstrFile   = file;
        ofn.nMaxFile    = sizeof(file);
        ofn.Flags       = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        GetSaveFileName(&ofn);

        std::string filePath = file;
        if (filePath.length() < 4 || filePath.substr(filePath.length() - 4) != ".png")
            filePath += ".png";

        return filePath;
    }

    std::string exportVerticesPath()
    {
        OPENFILENAME ofn;
        char         file[MAX_PATH] = {0};
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner   = NULL;
        ofn.lpstrFilter = ".txt\0*.txt\0";
        ofn.lpstrFile   = file;
        ofn.nMaxFile    = sizeof(file);
        ofn.Flags       = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        GetSaveFileName(&ofn);

        std::string filePath = file;
        if (filePath.length() < 4 || filePath.substr(filePath.length() - 4) != ".txt")
            filePath += ".txt";

        return filePath;
    }

} // namespace MapAreaCalculator