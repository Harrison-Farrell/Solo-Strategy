# This is a standard header for Solo-Strategy.
# It contains license information and ownership.
# 2026 Harrison Farrell.

#!/usr/bin/env python3
import os
import sys
import argparse
from pathlib import Path

def get_comment_blocks(content, file_ext, filename):
    """Returns the content formatted as a comment block based on file type."""
    lines = content.strip().split('\n')
    
    # Hash style for Python and CMake
    if file_ext in ['.py', '.sh', '.cmake', '.template'] or filename == 'CMakeLists.txt':
        return '\n'.join([f"# {line}" if line.strip() else "#" for line in lines]) + '\n\n'
    
    # C-style block for C++
    if file_ext in ['.h', '.hpp', '.cpp', '.c', '.cc']:
        return '/*\n' + '\n'.join([f" * {line}" if line.strip() else " *" for line in lines]) + '\n */\n\n'
    
    return None

def process_file(file_path, header_comment):
    """Prepends the header comment to the file if not already present."""
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            current_content = f.read()
            
        if current_content.startswith(header_comment.strip()):
            print(f"Skipping (already present): {file_path}")
            return

        with open(file_path, 'w', encoding='utf-8') as f:
            f.write(header_comment + current_content)
        print(f"Updated: {file_path}")
        
    except Exception as e:
        print(f"Error processing {file_path}: {e}")

def main():
    parser = argparse.ArgumentParser(description="Add header comments to source files.")
    parser.add_argument("header_file", help="Path to the .txt file containing the header content")
    parser.add_argument("root_dir", help="Root directory of the project")
    args = parser.parse_args()

    header_path = Path(args.header_file)
    if not header_path.exists():
        print(f"Error: Header file {args.header_file} not found.")
        sys.exit(1)

    with open(header_path, 'r', encoding='utf-8') as f:
        raw_header = f.read()

    root_path = Path(args.root_dir).resolve()
    
    target_extensions = {'.py', '.sh', '.cmake', '.h', '.hpp', '.cpp', '.c', '.cc', '.template'}
    target_filenames = {'CMakeLists.txt'}

    for root, dirs, files in os.walk(root_path):
        # Skip hidden directories like .git
        dirs[:] = [d for d in dirs if not d.startswith('.')]
        
        for name in files:
            file_path = Path(root) / name
            ext = file_path.suffix
            
            if ext in target_extensions or name in target_filenames:
                comment_block = get_comment_blocks(raw_header, ext, name)
                if comment_block:
                    process_file(file_path, comment_block)

if __name__ == "__main__":
    main()
