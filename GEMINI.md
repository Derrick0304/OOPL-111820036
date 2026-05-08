# Gemini CLI 開發規範與指令 (Project Mandates)

這份文件定義了本專案的開發原則、架構約束與技術標準。Gemini CLI 必須嚴格遵守以下規範，以確保開發的一致性與穩定性。

---

## 1. 核心開發守則 (Core Mandates)

*   **禁止盲目猜測 API**：在使用 `Util::` 或 `Core::` 命名空間下的 PTSD 引擎函式前，**必須**先查閱 `docs/PTSD_ENGINE_FULL_MANUAL.md`。嚴禁假設存在未列出的 Getter/Setter（例如：引擎沒有 `GetVisible()`）。
*   **UI 外部化優先**：嚴禁在 C++ 程式碼中硬編碼 (Hard-code) UI 座標。所有介面元素的位置、縮放與參數必須從 `Resources/Data/UI_Layout.json` 讀取。
*   **視覺化工具同步**：調整佈局時，必須優先使用或更新 `Tools/UI_Layout_Editor.py`。若在 C++ 中新增 UI 坑位，務必確保該元素也能在編輯器中顯示。

---

## 2. 技術架構約束 (Technical Standards)

### UI 系統
*   **按鈕組件**：主選單與一般按鈕應優先使用 `ImageTextButton`（支援邊框閃爍與縮放動畫）。除非是極簡的文字顯示，否則避免直接使用舊版的 `TextButton`。
*   **渲染管理**：若組件包含多個子物件（如按鈕與邊框），必須實作 `GetParts()` 函式，並在場景中透過 `AddChild()` 將所有零件加入渲染樹。

### 專案配置
*   **編譯管理**：新增任何 `.cpp` 或 `.hpp` 檔案時，**必須同時更新 `files.cmake`**。
*   **資源路徑**：務必使用 `RESOURCE_DIR` 宏來串接路徑，以確保跨平台建置的正確性。

---

## 3. 當前開發進度摘要 (Current State)

*   **UI 驅動**：`MainMenuScene`, `StageSelectScene`, `BattleScene` 已全面導入 JSON 佈局。
*   **JSON 格式**：`Resources/Data/UI_Layout.json` 是唯一的佈局真理來源。
*   **工具鏈**：`Tools/SpriteProcessor` 用於處理素材，`Tools/UI_Layout_Editor.py` 用於設計介面。

---

## 4. 給未來的 Gemini 提示 (Agent Instructions)

在開始任何任務前：
1.  讀取 `docs/DEVELOPMENT_PROGRESS.md` 確認目前進度。
2.  讀取 `files.cmake` 確認專案結構。
3.  **絕對不要重構已經 JSON 外部化的介面回到硬編碼模式。**
