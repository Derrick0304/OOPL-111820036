# 🚀 StageSelect 介面復刻開發計畫書

## 📝 總體目標
將目前的簡化版選關介面，升級為與《貓咪大戰爭》原版一致的高度整合 UI，包含地圖背景、經濟系統顯示、道具快捷列與體力系統。

---

## 🟢 第一階段：核心視覺與交互強化 (High Priority)
**目標：** 替換現有的文字按鈕，優化選關卡片，建立基本的視覺框架。

### 1. 程式修改
*   **按鈕圖片化**：將 `Battle!` 與 `Back` 按鈕由 `ImageTextButton` (LONG) 替換為專用的圖片資源。
*   **卡片樣式升級**：在 `StageSelectScene` 的 Carousel 中，為每個關卡卡片加入「體力消耗」與「通關次數」的文字標籤。
*   **背景圖層**：加入世界地圖底圖，並調整 Z-Index 確保它位於最底層。

### 2. 需要收集的素材
*   `Map_Background.png`: 世界地圖底圖。
*   `Btn_Attack_Base.png`: 黃色的大型出擊按鈕底圖。
*   `Btn_Back_Circle.png`: 左下角的黃色圓形返回按鈕。
*   `StageCard_Header.png`: 關卡卡片上方的白色標題框。

---

## 🟡 第二階段：經濟系統與體力機制 (Medium Priority)
**目標：** 實作頂部資訊欄，並讓體力 (Energy) 真正影響遊戲流程。

### 1. 程式修改
*   **頂部資訊欄 (Top Bar)**：
    *   在 `App` 中新增 `m_TotalXP` 與 `m_CatFood` 全域變數。
    *   在 `StageSelectScene` 頂部顯示 XP 與 貓罐頭 圖示與數字。
*   **體力條系統 (Energy Bar)**：
    *   實作體力自動回復邏輯。
    *   點擊 `Battle!` 時，檢查 `TotalEnergy >= StageCost`，不足時彈出提示或禁止進入。
*   **隊伍編號顯示**：在出擊按鈕旁顯示目前的 `Slot` (隊伍編號) 圖示。

### 2. 需要收集的素材
*   `Icon_XP.png`, `Icon_CatFood.png`: 頂部資源小圖示。
*   `Energy_Bar_Frame.png`: 體力條的外框。
*   `Energy_Bar_Fill.png`: 體力條的藍色/黃色填充圖。
*   `Btn_Plus.png`: 資源旁邊的小加號按鈕。
*   `Icon_Slot_Equip.png`: 顯示 Slot 3 / Equip 的小方塊圖標。

---

## 🔵 第三階段：道具列與氛圍裝飾 (Low Priority)
**目標：** 補齊底部的道具快捷列與裝飾性元素，使畫面達到 100% 復刻感。

### 1. 程式修改
*   **道具快捷列**：
    *   在畫面下方排列出 6-7 個道具圖示（如加速、寶物雷達）。
    *   目前可僅做視覺顯示，下方配上 `x99` 等虛假數字或讀取 `UserInventory.json`。
*   **動態特價看板**：
    *   實作左下角「On Sale」的閃爍動畫與倒數計時器。
*   **關卡標題裝飾**：
    *   為已通關的卡片加上「Clear!」的斜角絲帶圖案。

### 2. 需要收集的素材
*   `Item_Icons.png`: 包含加速、電腦、狙擊手等一整組道具圖示。
*   `Ribbon_Clear.png`: 紅色的 Clear! 絲帶圖片。
*   `Banner_Sale.png`: 左下角的綠色特價橫幅。
*   `Icon_Store.png`: 購物車圖標。

---

## 🛠 技術實作建議 (Technical Notes)

1.  **UI 座標同步**：所有的元件座標必須先在 `UI_Layout.json` 中定義坑位，嚴禁在 C++ 中 `SetTranslation(123, 456)`。
2.  **物件層級 (Z-Index)**：
    *   `Map`: -20
    *   `UI Elements`: 10
    *   `Floating Icons / Text`: 20
3.  **封裝組件**：建議建立一個 `ResourceDisplay` 類別，專門負責處理「圖示 + 背景框 + 數字文字」這種重複出現的 UI 結構。
