import os
import shutil

dll_paths = """
libharfbuzz-0.dll => /mingw64/bin/libharfbuzz-0.dll (0x7ffbdd4b0000)
libjpeg-8.dll => /mingw64/bin/libjpeg-8.dll (0x7ffbb1f50000)
libjxl.dll => /mingw64/bin/libjxl.dll (0x7ffb27430000)
libfreetype-6.dll => /mingw64/bin/libfreetype-6.dll (0x7ffbbd270000)
libpng16-16.dll => /mingw64/bin/libpng16-16.dll (0x7ffbdd460000)
libtiff-6.dll => /mingw64/bin/libtiff-6.dll (0x7ffbb1aa0000)
libwebp-7.dll => /mingw64/bin/libwebp-7.dll (0x7ffbb1680000)
libglib-2.0-0.dll => /mingw64/bin/libglib-2.0-0.dll (0x7ffba9b10000)
libgraphite2.dll => /mingw64/bin/libgraphite2.dll (0x7ffbdd430000)
libintl-8.dll => /mingw64/bin/libintl-8.dll (0x7ffbd8000000)
libbrotlidec.dll => /mingw64/bin/libbrotlidec.dll (0x7ffbdd0c0000)
libbrotlienc.dll => /mingw64/bin/libbrotlienc.dll (0x7ffbaaaa0000)
libbz2-1.dll => /mingw64/bin/libbz2-1.dll (0x7ffbc6900000)
libhwy.dll => /mingw64/bin/libhwy.dll (0x7ffbc6810000)
liblcms2-2.dll => /mingw64/bin/liblcms2-2.dll (0x7ffbb67f0000)
zlib1.dll => /mingw64/bin/zlib1.dll (0x7ffbc67e0000)
libdeflate.dll => /mingw64/bin/libdeflate.dll (0x7ffbc67c0000)
libjbig-0.dll => /mingw64/bin/libjbig-0.dll (0x69500000)
libLerc.dll => /mingw64/bin/libLerc.dll (0x7ffb53620000)
liblzma-5.dll => /mingw64/bin/liblzma-5.dll (0x7ffbbd9a0000)
libzstd.dll => /mingw64/bin/libzstd.dll (0x7ffb2afb0000)
libsharpyuv-0.dll => /mingw64/bin/libsharpyuv-0.dll (0x7ffbc6680000)
libpcre2-8-0.dll => /mingw64/bin/libpcre2-8-0.dll (0x7ffbb3370000)
libiconv-2.dll => /mingw64/bin/libiconv-2.dll (0x7ffb279a0000)
libbrotlicommon.dll => /mingw64/bin/libbrotlicommon.dll (0x7ffbbd240000)
libbrotlicommon.dll => /mingw64/bin/libbrotlicommon.dll (0x202ba030000)
"""

def copy_dll_to_script_folder(dll_path):
    # Extract the DLL file name
    dll_filename = os.path.basename(dll_path)
    
    # Convert Unix-style path to Windows path
    windows_dll_path = os.path.join(*dll_path.split('/'))
    
    # Get the directory of the Python script
    script_directory = os.path.dirname(os.path.abspath(__file__))
    
    try:
        # Copy the DLL to the script folder
        shutil.copy(windows_dll_path, os.path.join(script_directory, dll_filename))
        print(f"{dll_filename} successfully copied to {script_directory}")
    except FileNotFoundError:
        print(f"Error: DLL file not found at {windows_dll_path}")
    except Exception as e:
        print(f"Error: {e}")

# Split the input into lines and process each line
for line in dll_paths.strip().split('\n'):
    # Extract the path from each line
    dll_path = "H:\msys64"
    dll_path += line.split(' => ')[1].split()[0]
    
    # Copy the DLL to the script folder
    copy_dll_to_script_folder(dll_path)