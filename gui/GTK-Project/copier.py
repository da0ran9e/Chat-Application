import shutil
import os

# List of file paths to copy
files_to_copy = [
    "C:/WINDOWS/SYSTEM32/ntdll.dll",
    "C:/WINDOWS/System32/KERNEL32.DLL",
    "C:/WINDOWS/System32/KERNELBASE.dll",
    "C:/WINDOWS/System32/msvcrt.dll",
    "C:/msys64/mingw64/bin/libgobject-2.0-0.dll",
    "C:/msys64/mingw64/bin/libgdk-3-0.dll",
    "C:/msys64/mingw64/bin/libglib-2.0-0.dll",
    "C:/msys64/mingw64/bin/libgio-2.0-0.dll",
    "C:/WINDOWS/System32/ADVAPI32.dll",
    "C:/WINDOWS/System32/sechost.dll",
    "C:/WINDOWS/System32/ole32.dll",
    "C:/Windows/System32/ole32.dll",
    "C:/WINDOWS/System32/RPCRT4.dll",
    "C:/WINDOWS/System32/msvcp_win.dll",
    "C:/WINDOWS/System32/SHELL32.dll",
    "C:/WINDOWS/System32/GDI32.dll",
    "C:/WINDOWS/System32/ucrtbase.dll",
    "C:/Windows/System32/ucrtbase.dll",
    "C:/WINDOWS/System32/win32u.dll",
    "C:/WINDOWS/System32/USER32.dll",
    "C:/WINDOWS/System32/gdi32full.dll",
    "C:/WINDOWS/System32/SHLWAPI.dll",
    "C:/WINDOWS/System32/combase.dll",
    "C:/WINDOWS/System32/IMM32.dll",
    "C:/WINDOWS/System32/WS2_32.dll",
    "C:/Windows/System32/ws2_32.dll",
    "C:/WINDOWS/System32/comdlg32.dll",
    "C:/WINDOWS/System32/SETUPAPI.dll",
    "C:/WINDOWS/System32/shcore.dll",
    "C:/WINDOWS/SYSTEM32/DNSAPI.dll",
    "C:/msys64/mingw64/bin/libffi-8.dll",
    "C:/msys64/mingw64/bin/libintl-8.dll",
    "C:/WINDOWS/SYSTEM32/IPHLPAPI.DLL",
    "C:/WINDOWS/SYSTEM32/dwmapi.dll",
    "C:/WINDOWS/SYSTEM32/HID.DLL",
    "C:/msys64/mingw64/bin/libpcre2-8-0.dll",
    "C:/WINDOWS/WinSxS/amd64_microsoft.windows.common-controls_6595b64144ccf1df_6.0.22621.2506_none_270c5ae97388e100/COMCTL32.dll",
    "C:/msys64/mingw64/bin/libgmodule-2.0-0.dll",
    "C:/msys64/mingw64/bin/zlib1.dll",
    "C:/WINDOWS/SYSTEM32/WINMM.dll",
    "C:/msys64/mingw64/bin/libcairo-gobject-2.dll",
    "C:/msys64/mingw64/bin/libcairo-2.dll",
    "C:/msys64/mingw64/bin/libepoxy-0.dll",
    "C:/msys64/mingw64/bin/libfribidi-0.dll",
    "C:/msys64/mingw64/bin/libgdk_pixbuf-2.0-0.dll",
    "C:/msys64/mingw64/bin/libpangowin32-1.0-0.dll",
    "C:/msys64/mingw64/bin/libpangocairo-1.0-0.dll",
    "C:/msys64/mingw64/bin/libpango-1.0-0.dll",
    "C:/WINDOWS/SYSTEM32/WINSPOOL.DRV",
    "C:/msys64/mingw64/bin/libatk-1.0-0.dll",
    "C:/msys64/mingw64/bin/libharfbuzz-0.dll",
    "C:/msys64/mingw64/bin/libiconv-2.dll",
    "C:/WINDOWS/SYSTEM32/MSIMG32.dll",
    "C:/msys64/mingw64/bin/libgcc_s_seh-1.dll",
    "C:/msys64/mingw64/bin/libfontconfig-1.dll",
    "C:/msys64/mingw64/bin/libstdc++-6.dll",
    "C:/msys64/mingw64/bin/libfreetype-6.dll",
    "C:/WINDOWS/WinSxS/amd64_microsoft.windows.gdiplus_6595b64144ccf1df_1.1.22621.2506_none_57f74dcece1b5ace/gdiplus.dll",
    "C:/msys64/mingw64/bin/libpixman-1-0.dll",
    "C:/msys64/mingw64/bin/libpng16-16.dll",
    "C:/WINDOWS/SYSTEM32/DWrite.dll",
    "C:/msys64/mingw64/bin/libpangoft2-1.0-0.dll",
    "C:/msys64/mingw64/bin/libthai-0.dll",
    "C:/WINDOWS/SYSTEM32/USP10.dll",
    "C:/msys64/mingw64/bin/libgraphite2.dll",
    "C:/msys64/mingw64/bin/libwinpthread-1.dll",
    "C:/msys64/mingw64/bin/libexpat-1.dll",
    "C:/msys64/mingw64/bin/libbrotlidec.dll",
    "C:/msys64/mingw64/bin/libbz2-1.dll",
    "C:/msys64/mingw64/bin/libdatrie-1.dll",
    "C:/msys64/mingw64/bin/libbrotlicommon.dll"
]

# Destination directory where you want to copy the files
destination_directory = "C:/Users/user/Desktop/C-C++programming/C_programming/GTK+/TicTacToe/bin"

try:
    for file_path in files_to_copy:
        # Extract the filename from the path
        file_name = os.path.basename(file_path)
        destination_file = os.path.join(destination_directory, file_name)

        # Copy the file to the destination
        shutil.copy(file_path, destination_file)

    print(f"{len(files_to_copy)} files copied successfully to {destination_directory}.")
except Exception as e:
    print(f"An error occurred: {str(e)}")
