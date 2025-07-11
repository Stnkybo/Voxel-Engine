//
// Created by Lamad on 11/07/2025.
//

#ifdef _WIN32
#include <windows.h>

// Exist to remove the fugly console window that appears by default in windows when using MSVC 😭😭😭
int main(int argc, char** argv);

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int nCmdShow) {
    // You can convert lpCmdLine to argc/argv if needed, or just call main with defaults
    // For simplicity, just call main with zero args here
    return main(0, nullptr);
}
#endif
