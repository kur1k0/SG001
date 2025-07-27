##################################################################
# sjpeg converter script version 1.0
# Dependencies: (PYTHON-3)
##################################################################
SJPG_FILE_FORMAT_VERSION = "V1.00"  #
JPEG_SPLIT_HEIGHT   = 16
##################################################################
import math, os, sys, time
from PIL import Image


OUTPUT_FILE_NAME = ""
INPUT_FILE       = ""

OUTPUT_DIR = os.path.join(os.getcwd(), "data/res/img")
os.makedirs(OUTPUT_DIR, exist_ok=True)

if len(sys.argv) == 2:
    INPUT_FILE = sys.argv[1]
    OUTPUT_FILE_NAME = os.path.splitext(os.path.basename(INPUT_FILE))[0]
else:
    print("usage:\n\t python " + sys.argv[0] + " input_file.jpg")
    sys.exit(0)

try:
    im = Image.open(INPUT_FILE)
except:
    print("\nFile not found!")
    sys.exit(0)

print("\nConversion started...\n")
start_time = time.time()
width, height = im.size

print("Input:")
print("\t" + INPUT_FILE)
print("\tRES = " + str(width) + " x " + str(height) + '\n')


lenbuf = []
block_size = JPEG_SPLIT_HEIGHT
spilts = math.ceil(height / block_size)

c_code = f'''//LVGL SJPG C ARRAY
#include "lvgl/lvgl.h"

const uint8_t {OUTPUT_FILE_NAME}_map[] = {{
'''

sjpeg_data = bytearray()
sjpeg = bytearray()

row_remaining = height
temp_files = []

for i in range(spilts):
    if row_remaining < block_size:
        crop = im.crop((0, i*block_size, width, row_remaining + i*block_size))
    else:
        crop = im.crop((0, i*block_size, width, block_size + i*block_size))

    row_remaining -= block_size
    temp_path = os.path.join(OUTPUT_DIR, f"{i}.jpg")
    crop.save(temp_path, quality=90)
    temp_files.append(temp_path)

for path in temp_files:
    with open(path, "rb") as f:
        a = f.read()
        sjpeg_data += a
        lenbuf.append(len(a))

header = bytearray()
header += bytearray("_SJPG__".encode("UTF-8"))
header += bytearray(("\x00" + SJPG_FILE_FORMAT_VERSION + "\x00").encode("UTF-8"))
header += width.to_bytes(2, byteorder='little')
header += height.to_bytes(2, byteorder='little')
header += spilts.to_bytes(2, byteorder='little')
header += int(JPEG_SPLIT_HEIGHT).to_bytes(2, byteorder='little')

for item_len in lenbuf:
    header += item_len.to_bytes(2, byteorder='little')

sjpeg = header + sjpeg_data

# Remove temporary split files
for path in temp_files:
    os.remove(path)

# Write binary .sjpg file
sjpg_path = os.path.join(OUTPUT_DIR, OUTPUT_FILE_NAME + ".sjpg")
with open(sjpg_path, "wb") as f:
    f.write(sjpeg)

# Write C array .c file
new_line_threshold = 0
for byte in sjpeg:
    c_code += "\t" + hex(byte) + ","
    new_line_threshold += 1
    if new_line_threshold >= 16:
        c_code += "\n"
        new_line_threshold = 0

c_code += f'''
}};

lv_img_dsc_t {OUTPUT_FILE_NAME} = {{
\t.header.always_zero = 0,
\t.header.w = {width},
\t.header.h = {height},
\t.data_size = {len(sjpeg)},
\t.header.cf = LV_IMG_CF_RAW,
\t.data = {OUTPUT_FILE_NAME}_map,
}};
'''

c_file_path = os.path.join(OUTPUT_DIR, OUTPUT_FILE_NAME + ".c")
with open(c_file_path, 'w') as f:
    f.write(c_code)

time_taken = time.time() - start_time

print("Output:")
print(f"\tTime taken = {round(time_taken, 2)} sec")
print(f"\tbin size = {round(len(sjpeg)/1024, 1)} KB")
print(f"\t{sjpg_path}\t(bin file)\n\t{c_file_path}\t\t(c array)")

print("\nAll good!")
