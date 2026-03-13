import cv2
import numpy as np
from PIL import Image
import os
import sys

def split_sprite_sheet(image_name):
    input_path = os.path.join("Tools/SpriteProcessor/input", image_name)
    output_folder = "Tools/SpriteProcessor/output"
    
    # 1. 讀取圖片（含 Alpha 通道）
    img = cv2.imread(input_path, cv2.IMREAD_UNCHANGED)
    if img is None:
        print(f"錯誤: 找不到檔案 {input_path}")
        return
    
    # 2. 提取 Alpha 通道（不透明度），若無則轉為灰階
    if img.shape[2] == 4:
        alpha = img[:, :, 3]
    else:
        print("警告: 圖片不含透明通道，將嘗試以背景色偵測。")
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        _, alpha = cv2.threshold(gray, 250, 255, cv2.THRESH_BINARY_INV)
    
    # 3. 尋找輪廓（偵測不透明物體）
    contours, _ = cv2.findContours(alpha, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    
    # 清理舊的輸出
    for f in os.listdir(output_folder):
        os.remove(os.path.join(output_folder, f))

    count = 0
    # 按座標排序（從上到下，從左到右），方便辨識
    bounding_boxes = [cv2.boundingRect(c) for c in contours]
    bounding_boxes.sort(key=lambda b: (b[1] // 50, b[0])) # 允許 50px 的行誤差

    for x, y, w, h in bounding_boxes:
        if w < 5 or h < 5: continue # 過濾過小雜訊
        
        # 切割
        roi = img[y:y+h, x:x+w]
        
        # 轉為 RGBA 並儲存
        if roi.shape[2] == 4:
            roi_rgba = cv2.cvtColor(roi, cv2.COLOR_BGRA_RGBA)
        else:
            roi_rgba = cv2.cvtColor(roi, cv2.COLOR_BGR2RGBA)
            
        pil_img = Image.fromarray(roi_rgba)
        output_path = os.path.join(output_folder, f"{count}.png")
        pil_img.save(output_path)
        print(f"已生成: {output_path} ({w}x{h})")
        count += 1

if __name__ == "__main__":
    if len(sys.argv) > 1:
        split_sprite_sheet(sys.argv[1])
    else:
        # 如果沒給參數，自動處理 input 資料夾內的第一個檔案
        files = [f for f in os.listdir("Tools/SpriteProcessor/input") if f.endswith(('.webp', '.png', '.jpg'))]
        if files:
            split_sprite_sheet(files[0])
        else:
            print("請將素材放入 Tools/SpriteProcessor/input 資料夾。")
