import subprocess

# Path to ILMerge executable
ilmerge_path = "C:\\Program Files (x86)\\Microsoft\\ILMerge\\ILMerge.exe"

# Path to your main executable (e.g., YourApp.exe)
main_executable = "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\Tic-Tac-Toe.exe"

# List of DLLs to merge
dlls_to_merge = [
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\combase.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\COMCTL32.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\comdlg32.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\DNSAPI.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\dwmapi.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\DWrite.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\GDI32.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\gdi32full.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\gdiplus.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\HID.DLL",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\IMM32.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\IPHLPAPI.DLL",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\KERNEL32.DLL",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\KERNELBASE.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libatk-1.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libbrotlicommon.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libbrotlidec.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libbz2-1.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libcairo-2.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libcairo-gobject-2.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libdatrie-1.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libepoxy-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libexpat-1.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libffi-8.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libfontconfig-1.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libfreetype-6.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libfribidi-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgcc_s_seh-1.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgdk_pixbuf-2.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgdk-3-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgio-2.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libglib-2.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgmodule-2.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgobject-2.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgraphite2.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libgtk-3-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libharfbuzz-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libiconv-2.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libintl-8.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libpango-1.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libpangocairo-1.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libpangoft2-1.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libpangowin32-1.0-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libpcre2-8-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libpixman-1-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libpng16-16.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libstdc++-6.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libthai-0.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\libwinpthread-1.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\ucrtbase.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\USER32.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\USP10.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\win32u.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\WINMM.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\WINSPOOL.DRV",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\WS2_32.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\zlib1.dll",
    "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\ADVAPI32.dll",
]

# Output merged executable
output_executable = "C:\\Users\\user\\Desktop\\C-C++programming\\C_programming\\GTK+\\TicTacToe\\bin\\MergedApp.exe"

# Construct the command to run ILMerge
command = [
    ilmerge_path,
    f"/out:{output_executable}",
    main_executable,
] + dlls_to_merge

# Run the ILMerge command
subprocess.run(command)

print("Merging completed.")
