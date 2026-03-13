# 遊戲素材自動化處理流程 (Sprite Processor Workflow)

本工具旨在快速將從網站獲取的 `.webp` 或其他格式的大型 Sprite Sheet (大圖) 自動裁切為去背的 `.png` 個別動作幀，並快速整合進遊戲資源目錄。

## 1. 目錄結構 (Directory Structure)

*   `Tools/SpriteProcessor/`
    *   `input/`: **[用戶操作]** 放置原始大圖檔案（支援 `.webp`, `.png`, `.jpg`）。
    *   `output/`: **[系統生成]** 存放切割後的暫存圖片（命名為 `0.png`, `1.png`...）。
    *   `split.py`: **[核心腳本]** 負責自動偵測透明邊界並精確裁切。
    *   `README.md`: 本規格文件。

## 2. 環境要求 (Prerequisites)

*   **Python 3.11+**
*   **必要套件**: `opencv-python`, `numpy`, `pillow`
    *   *安裝指令*: `pip install opencv-python numpy pillow`

## 3. 標準作業程序 (Standard Operating Procedure)

### 第一階段：自動切割 (Auto-Split)
1.  用戶將大圖放入 `input/`。
2.  用戶指令：`開始切割 [檔名]`。
3.  Agent 執行 `split.py`：
    *   自動偵測 Alpha 通道（去背區域）。
    *   依照「從上到下、從左到右」的順序對物體進行編號。
    *   將結果輸出至 `output/`。

### 第二階段：人工辨識與整合 (Identification & Integration)
1.  用戶檢查 `output/` 中的圖片內容。
2.  用戶指令範例：
    *   「將 0~3 設為 BasicCat 的 Walk」
    *   「將 4, 5 設為 BasicCat 的 Attack」
    *   「將 10 設為 TankCat 的 Icon」
3.  Agent 執行自動搬移：
    *   依照指令將檔案移動至 `Resources/Units/{Team}/{UnitName}/{State}/{Index}.png`。
    *   若目標目錄不存在，Agent 會自動建立。
    *   搬移完成後自動清理 `output/` 暫存區。

## 4. 命名規範 (Conventions)

*   **單位目錄**: `Resources/Units/{陣營}/{單位名稱}/`
*   **動作子目錄**: `Walk/` (走路), `Attack/` (攻擊), `Dead/` (死亡)。
*   **幀檔名**: 必須從 `0.png` 開始遞增編號。
*   **單位數據**: 整合完成後，需在 `src/UnitFactory.cpp` 中註冊該單位的動畫幀數與數值。

## 5. 優勢
*   **零手動裁切**: 不再需要使用 Photoshop 手動選取範圍。
*   **像素精確**: 腳本自動貼合不透明邊界，確保圖片沒有多餘空白。
*   **快速疊代**: 從拿到素材到遊戲中看到單位跑動，過程只需不到一分鐘。
