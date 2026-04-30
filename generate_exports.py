import pefile
import os

def generate_pragmas(dll_path, target_dll_name):
    # Check if the file exists
    if not os.path.exists(dll_path):
        print(f"Error: Could not find {dll_path}")
        return

    try:
        # Load the PE (Portable Executable) file
        pe = pefile.PE(dll_path)
        
        # Check if the DLL actually exports any functions
        if not hasattr(pe, 'DIRECTORY_ENTRY_EXPORT'):
            print(f"No exports found in {dll_path}.")
            return

        print(f"// ==========================================")
        print(f"// Auto-generated pragma exports for {dll_path}")
        print(f"// Routing to target DLL: {target_dll_name}.dll")
        print(f"// ==========================================\n")

        # Loop through all exported symbols and format them
        for exp in pe.DIRECTORY_ENTRY_EXPORT.symbols:
            # Some exports might only be by ordinal (no name), so we check if a name exists
            if exp.name:
                func_name = exp.name.decode('utf-8')
                print(f'#pragma comment(linker, "/export:{func_name}={target_dll_name}.{func_name}")')
                
    except Exception as e:
        print(f"Error processing DLL: {e}")

if __name__ == "__main__":
    # The path to the original game DLL
    original_dll_file = "bass.dll" 
    
    # The name you plan to give the original file (without the .dll extension)
    target_forward_name = "bass_real" 
    
    generate_pragmas(original_dll_file, target_forward_name)