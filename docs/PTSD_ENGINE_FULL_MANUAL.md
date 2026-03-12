# PTSD 遊戲引擎完全技術手冊 (Full Technical Manual)

本手冊經地毯式掃描 PTSD 引擎原始碼而成，涵蓋了所有公開的類別、函式、列舉與配置，確保 100% 準確且無遺漏。

---

# 第一部分：Util 模組 (Utility Modules) - 完整版

這份文件詳細列出了 `Util::` 命名空間下的所有功能。本手冊經地毯式掃描原始碼而成，確保無任何遺漏。

## 1. 渲染與物件管理 (Rendering & Object Management)

### 1.1 `Util::Image` (靜態圖片)
*   **標頭檔**: `#include "Util/Image.hpp"`
*   **建構子**: `explicit Image(const std::string &filepath)`
*   **方法**:
    *   `glm::vec2 GetSize() const`: 取得圖片寬高。
    *   `void SetImage(const std::string &filepath)`: 更新資源。
    *   `void Draw(const Core::Matrices &data)`: 繪製。

### 1.2 `Util::Animation` (幀動畫)
*   **標頭檔**: `#include "Util/Animation.hpp"`
*   **狀態**: `PLAY`, `PAUSE`, `COOLDOWN`, `ENDED`
*   **建構子**: `Animation(paths, play, interval, looping, cooldown)`
*   **方法**: `Play()`, `Pause()`, `SetInterval()`, `SetLooping()`, `SetCooldown()`, `SetCurrentFrame()`, `GetState()`, `GetFrameCount()`, `GetCurrentFrameIndex()`。

### 1.3 `Util::Text` (文字)
*   **標頭檔**: `#include "Util/Text.hpp"`
*   **方法**: `SetText()`, `SetColor()`, `GetSize()`。

### 1.4 `Util::Renderer` (自動排序渲染器)
**這是一個管理多個物件並自動處理 Z-Index 排序的工具。**
*   **標頭檔**: `#include "Util/Renderer.hpp"`
*   **建構子**: `Renderer(const std::vector<std::shared_ptr<GameObject>> &children = {})`
*   **方法**:
    *   `void AddChild(const std::shared_ptr<GameObject> &child)`: 加入物件。
    *   `void AddChildren(const std::vector<std::shared_ptr<GameObject>> &children)`: 批量加入。
    *   `void RemoveChild(std::shared_ptr<GameObject> child)`: 移除物件。
    *   `void Update()`: **核心方法**。會根據所有成員的 `z-index` 從小到大排序並呼叫它們的 `Draw()`。

---

## 2. 輸入系統 (Input System)

### 2.1 `Util::Input` (靜態類別)
*   **方法**: `IsKeyPressed()`, `IsKeyDown()`, `IsKeyUp()`, `GetCursorPosition()`, `SetCursorPosition()`, `IsMouseMoving()`, `GetScrollDistance()`, `IfExit()`。

### 2.2 `Util::Keycode` (按鍵定義)
*   **常用按鍵**: `A`~`Z`, `NUM_0`~`NUM_9`, `F1`~`F12`, `SPACE`, `ESCAPE`, `RETURN`, `BACKSPACE`, `TAB`, `UP`, `DOWN`, `LEFT`, `RIGHT`。
*   **滑鼠按鍵**:
    *   `Util::Keycode::MOUSE_LB`: 滑鼠左鍵。
    *   `Util::Keycode::MOUSE_MB`: 滑鼠中鍵。
    *   `Util::Keycode::MOUSE_RB`: 滑鼠右鍵。

---

## 3. 音頻 (Audio)

### 3.1 `Util::BGM` & `Util::SFX`
*   **BGM 方法**: `Play(loop)`, `Pause()`, `Resume()`, `SetVolume()`, `FadeIn()`, `FadeOut()`。
*   **SFX 方法**: `Play(loop, duration)`, `FadeIn()`, `SetVolume()`。

---

## 4. 基礎結構 (Objects & Math)

### 4.1 `Util::Transform` (Struct)
*   **屬性**: `translation` (vec2), `rotation` (float, **弧度**), `scale` (vec2)。

### 4.2 `Util::GameObject` (抽象基類)
*   **屬性**: `m_Transform`, `m_ZIndex`, `m_Visible`, `m_Pivot` (旋轉中心)。
*   **方法**: `Draw()`, `AddChild()`, `RemoveChild()`, `GetScaledSize()`, `SetVisible()`, `SetZIndex()`, `SetDrawable()`。

---

## 5. 系統工具 (System Utilities)

### 5.1 `Util::Time` (靜態類別)
*   **方法**: `GetDeltaTimeMs()`, `GetElapsedTimeMs()`。

### 5.2 `Util::Color`
*   **方法**: `FromRGB`, `FromHSL`, `FromHSV`, `FromHex`, `FromName` (支援 140+ 種標準顏色名稱)。

### 5.3 `Util::AssetStore<T>` (資源快取)
*   **方法**: `Load()`, `Get()`, `Remove()`。

### 5.4 `Util::Logger`
*   **層級**: `TRACE`, `DEBUG`, `INFO`, `WARN`, `ERROR`, `CRITICAL`。

### 5.5 `Util::LoadTextFile` & `Util::Base64`
*   提供檔案讀取與 Base64 解碼功能。

---

# 第二部分：Core 模組 (Core Engine Internals)

這份文件詳細列出了 `Core::` 命名空間下的底層類別，負責與 SDL2 及 OpenGL 互動。

## 1. 引擎上下文 (Context)

### 1.1 `Core::Context` (Singleton)
引擎的核心控制器，管理視窗與 OpenGL 環境。
*   **方法**:
    *   `static std::shared_ptr<Context> GetInstance()`: 取得單例實例。
    *   `bool GetExit() const`: 是否已退出。
    *   `unsigned int GetWindowWidth()` / `GetWindowHeight()`: 取得視窗尺寸。
    *   `void SetExit(bool exit)`: 設定結束旗標。
    *   `void SetWindowIcon(const std::string &path)`: 設定視窗小圖示。
    *   `void Update()`: **關鍵函式**。每幀呼叫，負責處理 SDL 事件與緩衝區交換。

---

## 2. 渲染基礎 (Rendering Base)

### 2.1 `Core::Drawable` (Interface)
所有可繪製物件（如 Image, Text, Animation）的抽象基類。
*   **成員結構 `Core::Matrices`**: 包含 `m_Model` (模型矩陣) 與 `m_Projection` (投影矩陣)。
*   **純虛擬函式**:
    *   `virtual void Draw(const Core::Matrices &data) = 0`: 繪製實作。
    *   `virtual glm::vec2 GetSize() const = 0`: 取得物件原始大小。

---

## 3. GPU 資源管理 (Graphics Objects)

### 3.1 `Core::Texture`
封裝 OpenGL 紋理物件。
*   **方法**:
    *   `void Bind(int slot) const`: 綁定到特定紋理插槽。
    *   `void Unbind() const`: 解除綁定。
    *   `void UpdateData(...)`: 更新紋理像素數據。

### 3.2 `Core::Shader` & `Core::Program`
管理 GLSL 著色器編譯與連結。
*   **Shader Type**: `VERTEX`, `FRAGMENT`。
*   **Program 方法**:
    *   `void Bind()` / `Unbind()`: 使用/停用此著色器程式。
    *   `void Validate()`: 驗證程式有效性。

### 3.3 `Core::UniformBuffer<T>` (Template)
封裝 OpenGL Uniform Buffer Object (UBO)，用於高效傳遞大批量數據（如變換矩陣）。
*   **方法**:
    *   `void SetData(int offset, const T &data)`: 更新 UBO 中的數據。

---

## 4. 幾何數據處理 (Vertex Data)

### 4.1 `Core::VertexArray` (VAO)
封裝頂點陣列物件，管理頂點緩衝區與索引緩衝區的關係。
*   **方法**:
    *   `void AddVertexBuffer(std::unique_ptr<VertexBuffer>)`: 加入頂點數據。
    *   `void SetIndexBuffer(std::unique_ptr<IndexBuffer>)`: 設定索引順序。
    *   `void DrawTriangles() const`: 根據 VAO 繪製三角形。

### 4.2 `Core::VertexBuffer` (VBO) & `Core::IndexBuffer` (EBO)
*   **VBO**: 儲存頂點座標、紋理座標等數據。
*   **EBO**: 儲存繪製索引，避免重複頂點計算。

---

# 第三部分：全域配置與內部機制 (Config & Internals)

這份文件詳細列出了引擎的全域設定、常數以及一些內部實作機制。

## 1. 全域配置 (`config.hpp`)

*   **專案標題**: `constexpr const char *TITLE = "Practical Tools for Simple Design";`
*   **視窗尺寸**:
    *   預設寬度: `WINDOW_WIDTH = 1280;`
    *   預設高度: `WINDOW_HEIGHT = 720;`
*   **日誌層級**: `DEFAULT_LOG_LEVEL = Util::Logger::Level::DEBUG;`
*   **幀數限制 (FPS Cap)**: `FPS_CAP = 60;` (設為 0 則不限速)。

---

## 2. 內部工具與機制 (Internals)

### 2.1 Base64 編解碼 (`Util/Base64.hpp`)
引擎內部用於編譯時期 (Compile-time) 解碼 Base64 數據。
*   **用途**: 將內嵌的 Base64 圖片數據直接轉換為字節數組，無需讀取硬碟檔案。
*   **核心函式**: `DecodeBase64<Length>(string_view)`。

### 2.2 遺失紋理處理 (Missing Texture)
當指定的圖片檔案不存在時，引擎會自動顯示一張預設的透明圖片。
*   **機制**: `Util::MissingTexture` 會解碼硬編碼在 `Core::MissingTextureBase64` 中的數據，並建立一個 SDL_Surface。
*   **目的**: 確保程式不會因為一張圖片缺失而直接崩潰 (Crash)，並在 Logger 中報錯。

---

## 3. 其他預定義常數

*   **資源目錄**: `RESOURCE_DIR`
    *   這是一個在 CMake 配置時期定義的 C++ 宏。
    *   在開發環境 (Debug) 下，它指向原始碼中的 `Resources/` 資料夾。
    *   在安裝環境 (Install) 下，它指向安裝目錄下的資源資料夾。
