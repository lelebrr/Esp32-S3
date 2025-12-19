
# optimize_images.py
import os
import glob
import subprocess

def optimize_images():
    print("Optimization of images starting...")
    
    # Path to images - adjusting based on finding
    # User said data/img, we will try to find where pngs are.
    # We will search recursively for pngs in data/
    
    # Check if data/img exists
    img_dir = os.path.join(os.getcwd(), "data", "img")
    if not os.path.exists(img_dir):
        # Fallback to embedded_resources
        img_dir = os.path.join(os.getcwd(), "embedded_resources", "web_interface", "assets")
        if not os.path.exists(img_dir):
            print("Cannot find image directory 'data/img'")
            return

    print(f"Processing images in {img_dir}")
    
    files = glob.glob(os.path.join(img_dir, "*.png"))
    for f in files:
        output = f.replace(".png", ".c")
        print(f"Converting {f} to {output}...")
        # Assuming lv_img_conv.py is in PATH or accessible. If not, this might fail unless user has it.
        # User script: lv_img_conv.py $f --format indexed4 --output ${f%.png}.c
        try:
            cmd = ["python", "lv_img_conv.py", f, "--format", "indexed4", "--output", output]
            subprocess.run(cmd, check=True)
        except Exception as e:
            print(f"Failed to convert {f}: {e}")
            # Try calling it directly if in path
            try:
                cmd = ["lv_img_conv.py", f, "--format", "indexed4", "--output", output]
                subprocess.run(cmd, check=True)
            except Exception as e2:
                 print(f"Failed again: {e2}")

if __name__ == "__main__":
    optimize_images()
