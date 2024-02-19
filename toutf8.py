# -*- coding: utf-8 -*-
import os
import chardet
import codecs

def convert_to_utf8(file_path):
    with open(file_path, 'rb') as file:
        raw_data = file.read()
        result = chardet.detect(raw_data)
        encoding = result['encoding'] if result['encoding'] else 'utf-8'  # Utiliser 'utf-8' par défaut
        
        try:
            content = raw_data.decode(encoding)
            with codecs.open(file_path, 'w', encoding='utf-8', errors='replace') as output_file:
                output_file.write(content)
        except UnicodeDecodeError:
            print(f"Unable to decode {file_path} with detected encoding {encoding}")

def convert_folder_to_utf8(folder_path):
    converted_files = []  # List to store converted file paths
    for root, dirs, files in os.walk(folder_path):
        for file_name in files:
            if file_name.lower().endswith('.c'):
                file_path = os.path.join(root, file_name)
                convert_to_utf8(file_path)
                converted_files.append(file_path)

    return converted_files

if __name__ == "__main__":
    folder_path = r"C:\Kali-Zero-Firmware"
    converted_files = convert_folder_to_utf8(folder_path)

    if converted_files:
        print("Files converted to UTF-8:")
        for file_path in converted_files:
            print(file_path)
    else:
        print("No files were converted.")
