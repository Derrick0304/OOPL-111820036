# 遊戲素材自動化處理流程 (Sprite Processor Workflow)

本工具旨在快速將從網站獲取的 `.webp` 或其他格式的大型 Sprite Sheet (大圖) 自動裁切為去背的 `.png` 個別動作幀，並快速整合進遊戲資源目錄。

## 1. 目錄結構 (Directory Structure)

*   `Tools/SpriteProcessor/`
    *   `input/`: **[用戶操作]** 放置原始大圖檔案（支援 `.webp`, `.png`, `.jpg`）。
    *   `output/`: **[系統生成]** 存放切割後的暫存圖片（命名為 `0.png`, `1.png`...）。
    *   `split.py`: **[核心腳本]** 負責自動偵測透明邊界並進行「貼身裁切 (Tight Crop)」。
    *   `README.md`: 本規格文件。

## 2. 環境要求 (Prerequisites)

*   **Python 3.11+**
*   **必要套件**: `opencv-python`, `numpy`, `pillow`
    *   *安裝指令*: `pip install opencv-python numpy pillow`

## 3. 標準作業程序 (Standard Operating Procedure)

### 第一階段：自動切割與修整 (Auto-Split & Crop)
1.  將原始大圖或單張素材放入 `input/`。
2.  執行指令：`python Tools/SpriteProcessor/split.py [檔名]`。
3.  **關鍵點**：即使是只有一張圖的素材（如 Icon 或 基地），也建議通過此腳本處理，以消除不可見的透明邊距，確保遊戲內的「基準點 (Pivot)」計算正確。

### 第二階段：分類整合 (Categorization & Integration)
根據素材性質，將 `output/` 中的檔案搬移至對應目錄：

#### A. 戰鬥單位 (Units)
*   路徑：`Resources/Units/{Cats|Enemies}/{UnitName}/`
*   子目錄：`Walk/` (走路), `Attack/` (攻擊), `Dead/` (死亡)。
*   單張：`icon.png` (按鈕圖示，**務必使用切割後的版本**)。

#### B. 建築與基地 (Towers)
*   路徑：`Resources/Towers/{TowerName}/`
*   檔案：`base.png` (主體)。

#### C. 使用者介面 (UI)
*   路徑：`Resources/UI/{Category}/`
*   範例：`Buttons/BlackMask.png`, `Bars/BlueBar.png`。

#### D. 環境背景 (Backgrounds)
*   路徑：`Resources/Backgrounds/`

## 4. 命名與數據規範 (Conventions)

1.  **序列幀**：必須從 `0.png` 開始遞增編號（例如 `0.png`, `1.png`, `2.png`）。
2.  **單位數據註冊**：
    *   修改 `Resources/Data/Units.json`：定義 HP、ATK、速度、成本及動畫幀數。
    *   修改 `src/UIManager.cpp`：將新的貓咪名稱加入 `catNames` 列表以顯示在 UI。
3.  **資源格式**：一律轉換為帶 Alpha 通道的 `.png`。

## 5. 注意事項
*   **基準點對齊**：目前遊戲引擎以「圖片底部中央」作為 Pivot。若素材裁切不精確，會導致角色在動畫切換時跳動或浮空。
*   **圖示對齊**：所有出兵按鈕圖示應保持相近的視覺比例。若 icon 尺寸差異過大，應在 `input/` 階段先行縮放或在整合時確認像素大小。
