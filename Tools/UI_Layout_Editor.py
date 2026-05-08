import json
import tkinter as tk
from tkinter import messagebox, simpledialog
import os

# 專案路徑設定 (根據目前結構)
LAYOUT_FILE = "Resources/Data/UI_Layout.json"
CANVAS_WIDTH = 1280
CANVAS_HEIGHT = 720

class UILayoutEditor:
    def __init__(self, root):
        self.root = root
        self.root.title("Battle Cats UI Layout Editor")
        
        # 上方控制列
        self.ctrl_frame = tk.Frame(root)
        self.ctrl_frame.pack(side=tk.TOP, fill=tk.X, padx=10, pady=5)

        tk.Label(self.ctrl_frame, text="Scene:").pack(side=tk.LEFT)
        self.scene_var = tk.StringVar()
        self.scene_menu = tk.OptionMenu(self.ctrl_frame, self.scene_var, "", command=self.on_scene_change)
        self.scene_menu.pack(side=tk.LEFT, padx=5)

        tk.Button(self.ctrl_frame, text="Add Element", command=self.add_element).pack(side=tk.LEFT, padx=5)
        tk.Button(self.ctrl_frame, text="Save Layout", command=self.save_data, bg="green", fg="white").pack(side=tk.RIGHT, padx=5)

        # 建立畫布 (1280x720)
        self.canvas = tk.Canvas(root, width=CANVAS_WIDTH, height=CANVAS_HEIGHT, bg="#333333")
        self.canvas.pack(pady=10)

        # 繪製原點參考線 (0,0)
        self.canvas.create_line(CANVAS_WIDTH/2, 0, CANVAS_WIDTH/2, CANVAS_HEIGHT, fill="#555555", dash=(4,4))
        self.canvas.create_line(0, CANVAS_HEIGHT/2, CANVAS_WIDTH, CANVAS_HEIGHT/2, fill="#555555", dash=(4,4))
        
        self.elements = {} # 儲存畫布物件與其資料
        self.dragging_item = None
        self.data = {}
        
        self.load_data()

        # 綁定事件
        self.canvas.bind("<Button-1>", self.on_click)
        self.canvas.bind("<B1-Motion>", self.on_drag)
        self.canvas.bind("<ButtonRelease-1>", self.on_release)
        self.canvas.bind("<Button-3>", self.on_right_click) # 右鍵刪除

    def screen_to_game(self, x, y):
        game_x = x - CANVAS_WIDTH / 2
        game_y = (CANVAS_HEIGHT / 2) - y
        return game_x, game_y

    def game_to_screen(self, x, y):
        screen_x = x + CANVAS_WIDTH / 2
        screen_y = (CANVAS_HEIGHT / 2) - y
        return screen_x, screen_y

    def load_data(self):
        if not os.path.exists(LAYOUT_FILE):
            self.data = {"BattleScene": {}}
        else:
            with open(LAYOUT_FILE, "r", encoding="utf-8") as f:
                self.data = json.load(f)
        
        # 更新選單
        scenes = list(self.data.keys())
        if not scenes:
            scenes = ["BattleScene"]
            self.data["BattleScene"] = {}
            
        menu = self.scene_menu["menu"]
        menu.delete(0, "end")
        for scene in scenes:
            menu.add_command(label=scene, command=lambda s=scene: self.scene_var.set(s) or self.on_scene_change(s))
        
        self.scene_var.set(scenes[0])
        self.render_elements()

    def on_scene_change(self, scene_name):
        self.render_elements()

    def render_elements(self):
        self.canvas.delete("ui_element")
        self.elements.clear()
        
        scene_name = self.scene_var.get()
        scene_data = self.data.get(scene_name, {})
        
        for key, val in scene_data.items():
            color = "orange"
            w, h = 80, 80
            if "Grid" in key or "List" in key:
                color = "cyan"
                w, h = 200, 100
            elif "Panel" in key:
                color = "purple"
                w, h = 150, 150
            
            self.create_rect(key, val.get("x", 0), val.get("y", 0), w, h, color)

    def create_rect(self, name, gx, gy, w, h, color):
        sx, sy = self.game_to_screen(gx, gy)
        rect_id = self.canvas.create_rectangle(sx-w/2, sy-h/2, sx+w/2, sy+h/2, 
                                             fill=color, stipple="gray50", tags="ui_element", outline="white")
        text_id = self.canvas.create_text(sx, sy, text=name, fill="white", tags="ui_element", font=("Arial", 10, "bold"))
        
        self.elements[rect_id] = {"name": name, "text_id": text_id, "w": w, "h": h}
        self.elements[text_id] = {"rect_id": rect_id}

    def add_element(self):
        name = simpledialog.askstring("Add Element", "Enter element name (e.g., PauseButton):")
        if name:
            scene_name = self.scene_var.get()
            if name in self.data[scene_name]:
                messagebox.showwarning("Warning", "Element already exists!")
                return
            
            self.data[scene_name][name] = {"x": 0.0, "y": 0.0}
            self.render_elements()

    def on_click(self, event):
        item = self.canvas.find_closest(event.x, event.y)
        if "ui_element" in self.canvas.gettags(item):
            if item in self.elements and "rect_id" in self.elements[item]:
                self.dragging_item = self.elements[item]["rect_id"]
            else:
                self.dragging_item = item[0]

    def on_drag(self, event):
        if self.dragging_item:
            x, y = event.x, event.y
            info = self.elements.get(self.dragging_item)
            if info:
                w, h = info["w"], info["h"]
                self.canvas.coords(self.dragging_item, x-w/2, y-h/2, x+w/2, y+h/2)
                self.canvas.coords(info["text_id"], x, y)

    def on_release(self, event):
        if self.dragging_item:
            gx, gy = self.screen_to_game(event.x, event.y)
            name = self.elements[self.dragging_item]["name"]
            scene_name = self.scene_var.get()
            
            self.data[scene_name][name]["x"] = round(float(gx), 1)
            self.data[scene_name][name]["y"] = round(float(gy), 1)
            self.dragging_item = None

    def on_right_click(self, event):
        item = self.canvas.find_closest(event.x, event.y)
        if "ui_element" in self.canvas.gettags(item):
            if item in self.elements and "rect_id" in self.elements[item]:
                target_id = self.elements[item]["rect_id"]
            else:
                target_id = item[0]
                
            name = self.elements[target_id]["name"]
            if messagebox.askyesno("Delete", f"Delete element '{name}'?"):
                scene_name = self.scene_var.get()
                del self.data[scene_name][name]
                self.render_elements()

    def save_data(self):
        try:
            with open(LAYOUT_FILE, "w", encoding="utf-8") as f:
                json.dump(self.data, f, indent=2)
            messagebox.showinfo("Saved", f"Layout saved to {LAYOUT_FILE}")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save: {e}")

if __name__ == "__main__":
    root = tk.Tk()
    editor = UILayoutEditor(root)
    root.mainloop()
