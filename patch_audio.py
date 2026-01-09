import os

path = os.path.join(os.getcwd(), ".pio/libdeps/MorphNode/ESP32-audioI2S/src/Audio.cpp")
print(f"Patching {path}")

try:
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    # Fix min() type mismatch
    new_content = content.replace(
        "availableBytes = min(availableBytes, InBuff.writeSpace());",
        "availableBytes = min((size_t)availableBytes, InBuff.writeSpace());"
    )
    
    if content != new_content:
        with open(path, 'w', encoding='utf-8') as f:
            f.write(new_content)
        print("Patched successfully!")
    else:
        print("String not found, maybe already patched?")

except Exception as e:
    print(f"Error: {e}")
