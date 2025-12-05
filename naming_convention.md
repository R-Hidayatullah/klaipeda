
---

# ✅ **Recommended Naming Convention (C++14, Cross-Platform, OpenGL)**

---

## **1. File Names (snake_case)**

Use **lowercase snake_case** for all file names.

### ✔ Examples

```
renderer.cpp
renderer.hpp
opengl_context.cpp
opengl_context.hpp
vertex_buffer.cpp
texture_loader.cpp
math_vec3.hpp
```

### Why?

* Works on Linux, Windows, macOS (case-sensitive FS safe)
* Consistent with CMake and most C++ engine codebases

---

## **2. Class Names (PascalCase)**

Each class name should be **PascalCase**, descriptive, and unique.

### ✔ Examples

```cpp
class Renderer;
class OpenGLContext;
class VertexBuffer;
class ShaderProgram;
class Texture2D;
class Camera;
```

---

## **3. Structs (PascalCase or PascalCase + Data Suffix)**

For simple data containers:

### ✔ Examples

```cpp
struct Vec3;
struct Mat4;
struct MeshData;
struct TextureDesc;
```

---

## **4. Functions / Methods (camelCase)**

### ✔ Examples

```cpp
void init();
void renderFrame();
bool loadShader(const std::string& path);
GLuint createTexture2D(int width, int height);
```

---

## **5. Member Variables**

Two valid options.
I recommend the **m_ prefix** (common in game engines).

### Option A: `m_` + camelCase

```cpp
class Texture2D {
private:
    GLuint m_id;
    int m_width;
    int m_height;
};
```

### Option B: trailing underscore `_`

(Used in Google C++ and LLVM)

```cpp
GLuint id_;
int width_;
```

**Pick one and stay consistent.**

---

## **6. Local Variables (camelCase)**

### ✔ Examples

```cpp
int vertexCount;
float deltaTime;
GLuint vao;
std::string filePath;
```

---

## **7. Constants (ALL_CAPS with underscores)**

### ✔ Examples

```cpp
const float PI = 3.14159265f;
const int MAX_LIGHTS = 16;
```

---

## **8. Enums (PascalCase) + Enum Values (PascalCase)**

### ✔ Examples

```cpp
enum class TextureFormat {
    Rgba8,
    Rgb8,
    Depth24Stencil8
};

enum class LogLevel {
    Info,
    Warning,
    Error
};
```

---

## **9. Namespaces (lowercase)**

Avoid uppercase for namespaces.

### ✔ Examples

```cpp
namespace engine {
namespace gfx {
namespace math {
```

---

## **10. Macros (ALL_CAPS) — use rarely**

### ✔ Examples

```cpp
#define ASSERT(x)
#define ENABLE_DEBUG_LOG
```

Avoid macros except for platform detection or compile flags.

---

# 🎯 **OpenGL Resource Naming (Good Practice)**

For OpenGL objects, use short prefixes:

| Type             | Prefix   |
| ---------------- | -------- |
| VAO              | `vao`    |
| VBO              | `vbo`    |
| Texture          | `tex`    |
| Shader           | `shader` |
| Program          | `prog`   |
| Framebuffer      | `fbo`    |
| Uniform Location | `loc`    |

### ✔ Example

```cpp
GLuint vaoModel;
GLuint texDiffuse;
GLuint progLighting;
```

---

# 🎯 **Folder Structure Naming**

Use snake_case:

```
src/
    core/
    gfx/
    math/
    utils/
include/
assets/
shaders/
external/
```

---

# 🎯 **CMake Naming**

Use lowercase snake_case:

```
add_library(engine_core ...)
add_library(engine_gfx ...)
target_link_libraries(engine_gfx PUBLIC engine_core)
```

---

# ⭐ Final Summary (Short Version)

| What?       | Style                         |
| ----------- | ----------------------------- |
| Files       | `snake_case`                  |
| Classes     | `PascalCase`                  |
| Structs     | `PascalCase`                  |
| Methods     | `camelCase`                   |
| Local vars  | `camelCase`                   |
| Member vars | `m_camelCase` OR `camelCase_` |
| Constants   | `ALL_CAPS`                    |
| Enums       | `PascalCase`                  |
| Enum values | `PascalCase`                  |
| Namespaces  | lowercase                     |
| Macros      | `ALL_CAPS`                    |

---

Here are the **best, cleanest, most common function naming conventions** for a modern C++14, cross-platform, OpenGL-based project.

---

# ✅ **Recommended: camelCase for functions**

This is the most widely used style in game engines (bgfx, Godot C++ modules, Urho3D, OGRE, etc.)

### ✔ Examples

```cpp
void init();
void shutdown();
void renderFrame();
bool loadTexture(const std::string& path);
GLuint createShader(const char* src, GLenum type);
void setPosition(float x, float y, float z);
```

### **Why it's best for engines**

* Consistent with member variables in camelCase
* Easy to distinguish from ClassNames (PascalCase)
* Short and readable
* Works well with OpenGL function names (glCreateShader, glBindBuffer)

---

# 🟦 Alternative 1: PascalCase (not recommended for C++)

Used in C# and Unreal Engine, but uncommon in C++ engines.

### Examples

```cpp
void Init();
void LoadShader();
void RenderFrame();
```

### Why not ideal?

* Looks too similar to class names
* Not idiomatic C++

---

# 🟧 Alternative 2: snake_case (used in Linux kernel, C libs)

Some C++ game engines use it (raylib, some physics libs).

### Examples

```cpp
void init();
void load_texture();
void render_frame();
```

### Why not ideal?

* C-like rather than C++-like
* Doesn’t match OpenGL naming style

---

# ⭐ Best Practice (Game Engine Style)

### **Member functions: camelCase**

```cpp
class Renderer {
public:
    void init();
    void renderFrame();
    void loadMesh(const std::string& path);
    void setViewport(int width, int height);
};
```

### **Private helper functions: camelCase but with leading _ optional**

```cpp
private:
    void setupGLState();
    void compileShaders();
    void uploadVertexData();
```

Avoid double underscores (`__foo`) — reserved by C++.

---

# ⭐️ Consistency Rules

### 1. **Function names = verbs**

Good:

```cpp
loadMesh()
createTexture()
getPosition()
setVolume()
updateCamera()
```

Bad (noun-like, unclear):

```cpp
mesh()
texture()
camera()
```

---

### 2. **Accessors follow get/set**

```cpp
int getWidth() const;
void setWidth(int width);

const Vec3& getPosition() const;
void setPosition(const Vec3& pos);
```

---

### 3. **Boolean functions start with "is" / "has" / “can”**

```cpp
bool isVisible() const;
bool hasAlpha() const;
bool canRenderShadow() const;
```

---

### 4. **Factory functions begin with create**

```cpp
GLuint createShader(GLenum type);
GLuint createProgram();
Texture2D* createTexture(const TextureDesc& desc);
```

---

### 5. **OpenGL wrapper functions mirror GL naming**

```cpp
void bind();
void unbind();
void uploadData();
void compile();
void link();
```

---

# 🔥 Example (Full Naming Style in Action)

```cpp
class Texture2D {
public:
    bool loadFromFile(const std::string& path);
    void bind(uint32_t slot = 0) const;
    void unbind() const;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    bool loadImageData(const std::string& path);
    void uploadToGPU();

    GLuint m_id = 0;
    int m_width = 0;
    int m_height = 0;
};
```

This is clean, modern, and matches C++14 engine style.

---
