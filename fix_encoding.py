# -*- coding: utf-8 -*-
"""
Script to fix UTF-8 encoding issues (mojibake) in web interface files.
Detects and corrects double-encoded UTF-8 characters.
"""

import os
import re
from pathlib import Path

# Mojibake replacement map - UTF-8 bytes misinterpreted as Latin-1
REPLACEMENTS = {
    # Portuguese accented lowercase
    b'\xc3\x83\xc2\xa1': 'a',  # a with acute
    b'\xc3\x83\xc2\xa9': 'e',  # e with acute  
    b'\xc3\x83\xc2\xad': 'i',  # i with acute
    b'\xc3\x83\xc2\xb3': 'o',  # o with acute
    b'\xc3\x83\xc2\xba': 'u',  # u with acute
    b'\xc3\x83\xc2\xa3': 'a',  # a with tilde
    b'\xc3\x83\xc2\xb5': 'o',  # o with tilde
    b'\xc3\x83\xc2\xa2': 'a',  # a with circumflex
    b'\xc3\x83\xc2\xaa': 'e',  # e with circumflex
    b'\xc3\x83\xc2\xae': 'i',  # i with circumflex
    b'\xc3\x83\xc2\xb4': 'o',  # o with circumflex
    b'\xc3\x83\xc2\xa7': 'c',  # c with cedilla
    b'\xc3\x83\xc2\xa0': 'a',  # a with grave
    b'\xc3\x83\xc2\xa8': 'e',  # e with grave
    b'\xc3\x83\xc2\xb9': 'u',  # u with grave
}

def fix_file_encoding(filepath):
    """Fix encoding issues in a single file."""
    try:
        # Read file as bytes
        with open(filepath, 'rb') as f:
            content = f.read()
        
        original = content
        
        # Try to decode as UTF-8, if it fails the file might have issues
        try:
            text = content.decode('utf-8')
        except UnicodeDecodeError:
            print(f"  ERROR: Cannot decode {filepath}")
            return False
        
        # Check for common mojibake patterns and fix them
        # These are UTF-8 characters that were encoded twice
        
        # Pattern: The text shows as garbage because UTF-8 bytes were 
        # interpreted as Latin-1 and then re-encoded as UTF-8
        
        fixed_text = text
        changes = 0
        
        # Fix Portuguese accented characters (most common issue)
        replacements = [
            # Lowercase accented
            ('\u00c3\u00a1', '\u00e1'),  # a
            ('\u00c3\u00a9', '\u00e9'),  # e
            ('\u00c3\u00ad', '\u00ed'),  # i
            ('\u00c3\u00b3', '\u00f3'),  # o
            ('\u00c3\u00ba', '\u00fa'),  # u
            ('\u00c3\u00a3', '\u00e3'),  # a tilde
            ('\u00c3\u00b5', '\u00f5'),  # o tilde
            ('\u00c3\u00a2', '\u00e2'),  # a circumflex
            ('\u00c3\u00aa', '\u00ea'),  # e circumflex
            ('\u00c3\u00ae', '\u00ee'),  # i circumflex
            ('\u00c3\u00b4', '\u00f4'),  # o circumflex
            ('\u00c3\u00a7', '\u00e7'),  # c cedilla
            ('\u00c3\u00a0', '\u00e0'),  # a grave
            ('\u00c3\u00a8', '\u00e8'),  # e grave
            ('\u00c3\u00ac', '\u00ec'),  # i grave
            ('\u00c3\u00b2', '\u00f2'),  # o grave
            ('\u00c3\u00b9', '\u00f9'),  # u grave
            ('\u00c3\u00b1', '\u00f1'),  # n tilde
            ('\u00c3\u00bb', '\u00fb'),  # u circumflex
            # Uppercase accented
            ('\u00c3\u0080', '\u00c0'),  # A grave
            ('\u00c3\u0088', '\u00c8'),  # E grave
            ('\u00c3\u008c', '\u00cc'),  # I grave
            ('\u00c3\u0092', '\u00d2'),  # O grave
            ('\u00c3\u0099', '\u00d9'),  # U grave
            ('\u00c3\u0083', '\u00c3'),  # A tilde
            ('\u00c3\u0095', '\u00d5'),  # O tilde
            ('\u00c3\u0082', '\u00c2'),  # A circumflex
            ('\u00c3\u008a', '\u00ca'),  # E circumflex
            ('\u00c3\u008e', '\u00ce'),  # I circumflex
            ('\u00c3\u0094', '\u00d4'),  # O circumflex
            ('\u00c3\u009b', '\u00db'),  # U circumflex
            ('\u00c3\u0087', '\u00c7'),  # C cedilla
            ('\u00c3\u0089', '\u00c9'),  # E acute
            ('\u00c3\u009a', '\u00da'),  # U acute
            ('\u00c3\u0081', '\u00c1'),  # A acute
            ('\u00c3\u008d', '\u00cd'),  # I acute
            ('\u00c3\u0093', '\u00d3'),  # O acute
            # Degree symbol
            ('\u00c2\u00b0', '\u00b0'),  # degree
        ]
        
        for bad, good in replacements:
            if bad in fixed_text:
                fixed_text = fixed_text.replace(bad, good)
                changes += 1
        
        if changes > 0 or fixed_text != text:
            # Write back as UTF-8
            with open(filepath, 'w', encoding='utf-8', newline='\n') as f:
                f.write(fixed_text)
            return True
        
        return False
        
    except Exception as e:
        print(f"  ERROR processing {filepath}: {e}")
        return False


def main():
    web_interface_path = Path(r"C:\Users\leleb\OneDrive\Desktop\Cyd_Lele\embedded_resources\web_interface")
    
    # Extensions to process
    extensions = {'.html', '.js', '.css'}
    
    # Files to skip
    skip_patterns = ['.min.', '.gz']
    
    fixed_count = 0
    processed_count = 0
    
    print("=" * 60)
    print("UTF-8 Encoding Fixer")
    print("=" * 60)
    
    for filepath in web_interface_path.rglob('*'):
        if not filepath.is_file():
            continue
        
        if filepath.suffix.lower() not in extensions:
            continue
            
        if any(pattern in filepath.name for pattern in skip_patterns):
            continue
        
        processed_count += 1
        relative_path = filepath.relative_to(web_interface_path)
        
        if fix_file_encoding(filepath):
            print(f"  FIXED: {relative_path}")
            fixed_count += 1
        else:
            print(f"  OK: {relative_path}")
    
    print("=" * 60)
    print(f"Processed: {processed_count} files")
    print(f"Fixed: {fixed_count} files")
    print("=" * 60)


if __name__ == "__main__":
    main()
