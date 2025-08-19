#!/usr/bin/env python3
"""
Simple script to create a basic ICO file for the application.
This creates a 32x32 pixel icon with a folder representation.
"""

from PIL import Image, ImageDraw
import os

def create_icon():
    # Create a 32x32 image with transparent background
    size = (32, 32)
    img = Image.new('RGBA', size, (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Draw a simple folder icon
    # Folder body
    draw.rectangle([4, 10, 28, 26], fill=(255, 206, 84, 255), outline=(0, 0, 0, 255))
    
    # Folder tab
    draw.rectangle([4, 6, 16, 10], fill=(255, 206, 84, 255), outline=(0, 0, 0, 255))
    
    # Add some files inside to represent desktop icons
    draw.rectangle([8, 14, 12, 18], fill=(255, 255, 255, 255), outline=(0, 0, 0, 255))
    draw.rectangle([14, 14, 18, 18], fill=(255, 255, 255, 255), outline=(0, 0, 0, 255))
    draw.rectangle([20, 14, 24, 18], fill=(255, 255, 255, 255), outline=(0, 0, 0, 255))
    
    # Save as ICO file
    icon_path = os.path.join('resources', 'icon.ico')
    os.makedirs('resources', exist_ok=True)
    img.save(icon_path, format='ICO', sizes=[(32, 32)])
    print(f"Icon created: {icon_path}")

if __name__ == "__main__":
    try:
        create_icon()
    except ImportError:
        print("PIL (Pillow) not available. You can create the icon manually or install Pillow:")
        print("pip install Pillow")
        print("Then run this script again.")
    except Exception as e:
        print(f"Error creating icon: {e}")
        print("You can create a simple icon manually or use any icon editor.")
