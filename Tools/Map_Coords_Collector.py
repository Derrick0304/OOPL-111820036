import tkinter as tk
from tkinter import messagebox
from PIL import Image, ImageTk
import json
import os

# 圖片與輸出檔案路徑
IMAGE_PATH = "Resources/Backgrounds/StageSelect.png"
OUTPUT_PATH = "Resources/Data/Map_Coordinates.json"
MAP_SCALE = 4.62  # 遊戲中地圖的縮放比例

class MapCoordsCollector:
    def __init__(self, root):
        self.root = root
        self.root.title("Battle Cats World Map Coordinates Collector")

        if not os.path.exists(IMAGE_PATH):
            messagebox.showerror("Error", f"Could not find map image at {IMAGE_PATH}")
            self.root.destroy()
            return

        # 讀取地圖大小
        self.pil_img = Image.open(IMAGE_PATH)
        self.img_w, self.img_h = self.pil_img.size

        # 側邊欄控制
        self.sidebar = tk.Frame(root, width=220, bg="#e0e0e0")
        self.sidebar.pack(side=tk.LEFT, fill=tk.Y, padx=5, pady=5)

        self.info_label = tk.Label(self.sidebar, text="Stage Coordinates Collector", font=("Arial", 12, "bold"), bg="#e0e0e0")
        self.info_label.pack(anchor=tk.W, pady=5)

        self.status_label = tk.Label(self.sidebar, text="Click on the map for\nStage 1/48", font=("Arial", 10), bg="#e0e0e0", justify=tk.LEFT)
        self.status_label.pack(anchor=tk.W, pady=10)

        # 列表框顯示已點擊點
        self.listbox_label = tk.Label(self.sidebar, text="Collected Coordinates (Game Lcl):", font=("Arial", 9, "bold"), bg="#e0e0e0")
        self.listbox_label.pack(anchor=tk.W, pady=2)
        self.listbox = tk.Listbox(self.sidebar, width=28, height=20)
        self.listbox.pack(fill=tk.BOTH, expand=True, pady=5)

        # 控制按鈕
        self.btn_undo = tk.Button(self.sidebar, text="Undo Last Click (Z)", command=self.undo, width=20, bg="#ff9999")
        self.btn_undo.pack(pady=5)
        self.root.bind("<Control-z>", lambda e: self.undo())
        self.root.bind("<z>", lambda e: self.undo())

        self.btn_save = tk.Button(self.sidebar, text="Save JSON (S)", command=self.save_data, width=20, bg="#99ff99", font=("Arial", 10, "bold"))
        self.btn_save.pack(pady=5)
        self.root.bind("<s>", lambda e: self.save_data())

        # 畫布用來顯示地圖
        self.canvas = tk.Canvas(root, width=self.img_w, height=self.img_h, cursor="cross")
        self.canvas.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

        self.tk_img = ImageTk.PhotoImage(self.pil_img)
        self.canvas.create_image(0, 0, anchor=tk.NW, image=self.tk_img)

        # 已收集的像素坐標 [(px_x, px_y), ...]
        self.coords = []
        self.markers = []  # 畫布上的紅點 ID
        self.lines = []    # 畫布上的白線 ID

        # 讀取已存在的 coordinates (如果有)
        self.load_existing_data()

        # 綁定點擊事件
        self.canvas.bind("<Button-1>", self.on_click)

    def load_existing_data(self):
        if os.path.exists(OUTPUT_PATH):
            try:
                with open(OUTPUT_PATH, "r", encoding="utf-8") as f:
                    content = json.load(f)
                    game_coords = content.get("coordinates", [])
                    # 遊戲 local 坐標轉回畫布像素坐標
                    # game_x = (pixel_x - img_w/2) * scale => pixel_x = game_x / scale + img_w/2
                    # game_y = (img_h/2 - pixel_y) * scale => pixel_y = img_h/2 - game_y / scale
                    for pt in game_coords:
                        gx, gy = pt["x"], pt["y"]
                        px = gx / MAP_SCALE + self.img_w / 2
                        py = self.img_h / 2 - gy / MAP_SCALE
                        self.coords.append((px, py))
                
                self.redraw_canvas()
                self.update_listbox()
                self.update_status()
            except Exception as e:
                print(f"Failed to load existing json: {e}")

    def on_click(self, event):
        if len(self.coords) >= 48:
            messagebox.showinfo("Completed", "You have already collected 48 coordinates! Press Save to store them.")
            return

        px, py = event.x, event.y
        self.coords.append((px, py))
        
        self.redraw_canvas()
        self.update_listbox()
        self.update_status()

    def undo(self):
        if self.coords:
            self.coords.pop()
            self.redraw_canvas()
            self.update_listbox()
            self.update_status()

    def redraw_canvas(self):
        # 清除所有舊標記與線段
        for marker in self.markers:
            self.canvas.delete(marker)
        for line in self.lines:
            self.canvas.delete(line)
        self.markers.clear()
        self.lines.clear()

        # 重新繪製白線
        for i in range(len(self.coords) - 1):
            x1, y1 = self.coords[i]
            x2, y2 = self.coords[i+1]
            line_id = self.canvas.create_line(x1, y1, x2, y2, fill="white", width=2, dash=(3, 3))
            self.lines.append(line_id)

        # 重新繪製紅點
        for idx, (x, y) in enumerate(self.coords):
            # 選中最後一個點繪製成橘黃色
            color = "orange" if idx == len(self.coords) - 1 else "red"
            r = 5
            marker_id = self.canvas.create_oval(x-r, y-r, x+r, y+r, fill=color, outline="white", width=1)
            self.markers.append(marker_id)
            
            # 標註編號
            text_id = self.canvas.create_text(x, y-12, text=str(idx + 1), fill="yellow", font=("Arial", 8, "bold"))
            self.markers.append(text_id)

    def update_listbox(self):
        self.listbox.delete(0, tk.END)
        for idx, (px, py) in enumerate(self.coords):
            # 轉換為遊戲 local 坐標 (以中心點為 (0,0)，Y 軸朝上為正，並乘上 1.54 縮放)
            game_x = (px - self.img_w / 2.0) * MAP_SCALE
            game_y = (self.img_h / 2.0 - py) * MAP_SCALE
            self.listbox.insert(tk.END, f"Stage {idx+1:02d}: ({game_x:.1f}, {game_y:.1f})")

    def update_status(self):
        count = len(self.coords)
        if count < 48:
            self.status_label.config(text=f"Click on the map for\nStage {count + 1}/48\n\nTotal clicked: {count}")
        else:
            self.status_label.config(text="ALL 48 STAGES CLICKED!\nReady to Save.", fg="green")

    def save_data(self):
        if len(self.coords) < 48:
            if not messagebox.askyesno("Confirm", f"You have only clicked {len(self.coords)} stages (need 48). Do you want to save anyway?"):
                return

        # 轉換為 C++ 能加載的座標 JSON 格式
        output_data = {"coordinates": []}
        for idx, (px, py) in enumerate(self.coords):
            game_x = round((px - self.img_w / 2.0) * MAP_SCALE, 1)
            game_y = round((self.img_h / 2.0 - py) * MAP_SCALE, 1)
            output_data["coordinates"].append({
                "stageNum": idx + 1,
                "x": game_x,
                "y": game_y
            })

        os.makedirs(os.path.dirname(OUTPUT_PATH), exist_ok=True)
        with open(OUTPUT_PATH, "w", encoding="utf-8") as f:
            json.dump(output_data, f, indent=2)

        # 自動將紅點與白連線烘焙至地圖背景圖片中，提升選關介面載入效率
        try:
            self.bake_map_image()
            messagebox.showinfo("Saved", f"Coordinates saved and baked successfully to {OUTPUT_PATH}")
        except Exception as e:
            messagebox.showwarning("Warning", f"Coordinates saved, but failed to bake map image: {e}")

    def bake_map_image(self):
        backup_path = "Resources/Backgrounds/StageSelect_original.png"
        red_path = "Resources/UI/RedPoint.png"
        white_path = "Resources/UI/WhitePoint.png"

        if not os.path.exists(IMAGE_PATH):
            return
        if not os.path.exists(red_path) or not os.path.exists(white_path):
            return

        # 1. 備份原始地圖
        if not os.path.exists(backup_path):
            import shutil
            shutil.copy2(IMAGE_PATH, backup_path)

        # 2. 載入圖片元件並縮小為一半 (12x12 -> 6x6)
        base_map = Image.open(backup_path).convert("RGBA")
        red_pt = Image.open(red_path).convert("RGBA")
        white_pt = Image.open(white_path).convert("RGBA")

        red_w, red_h = red_pt.size
        white_w, white_h = white_pt.size
        red_pt = red_pt.resize((red_w // 2, red_h // 2), Image.LANCZOS)
        white_pt = white_pt.resize((white_w // 2, white_h // 2), Image.LANCZOS)

        red_w, red_h = red_pt.size
        white_w, white_h = white_pt.size

        # 3. 繪製白線點 (間距約 9.74 像素，對應遊戲中 45 距離單位)
        import math
        for i in range(len(self.coords) - 1):
            p1 = self.coords[i]
            p2 = self.coords[i+1]
            dx = p2[0] - p1[0]
            dy = p2[1] - p1[1]
            dist = math.sqrt(dx*dx + dy*dy)
            
            step_count = int(dist / 9.74)
            if step_count > 1:
                for s in range(1, step_count):
                    t = s / step_count
                    x = p1[0] + t * dx
                    y = p1[1] + t * dy
                    left = int(x - white_w / 2.0)
                    top = int(y - white_h / 2.0)
                    base_map.alpha_composite(white_pt, (left, top))

        # 4. 繪製紅點
        for (px, py) in self.coords:
            left = int(px - red_w / 2.0)
            top = int(py - red_h / 2.0)
            base_map.alpha_composite(red_pt, (left, top))

        # 5. 儲存
        output_map = base_map.convert("RGB")
        output_map.save(IMAGE_PATH, "PNG")

if __name__ == "__main__":
    root = tk.Tk()
    app = MapCoordsCollector(root)
    root.mainloop()
