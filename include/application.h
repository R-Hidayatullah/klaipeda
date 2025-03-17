#if !defined(APPLICATION_H)
#define APPLICATION_H
#include "ipf.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define DDS_MAGIC 0x20534444 // "DDS " in little-endian
#define DDS_FOURCC 0x00000004
#define DDS_DX10 0x30315844 // "DX10"

struct DDSHeader
{
    uint32_t magic;
    uint32_t size;
    uint32_t flags;
    uint32_t height;
    uint32_t width;
    uint32_t pitchOrLinearSize;
    uint32_t depth;
    uint32_t mipMapCount;
    uint32_t reserved1[11];

    struct
    {
        uint32_t size;
        uint32_t flags;
        uint32_t fourCC;
        uint32_t rgbBitCount;
        uint32_t rBitMask;
        uint32_t gBitMask;
        uint32_t bBitMask;
        uint32_t aBitMask;
    } pixelFormat;

    struct
    {
        uint32_t caps1;
        uint32_t caps2;
        uint32_t reserved[2];
    } caps;
    uint32_t reserved2;
};

struct DDSHeaderDX10
{
    uint32_t dxgiFormat;
    uint32_t resourceDimension;
    uint32_t miscFlag;
    uint32_t arraySize;
    uint32_t miscFlags2;
};

// Recursive structure for nested directories
struct DirectoryNode
{
    std::map<std::string, DirectoryNode> subdirectories;
    std::vector<int> files;
};

struct ArchiveData
{
    std::vector<uint8_t> decompressed_data;
    uint32_t temp_number;
    uint32_t selected_number;
    uint32_t last_selected_number;
    uint32_t last_selected_image;
    double extract_message_time;
    std::string last_extracted_filename;
    bool preview_tab_active;
    int width;
    int height;
    int channels;
    GLuint texture;
    unsigned char *image_data;
    DirectoryNode directory_node;
};

// Struct to hold application state
struct Application
{
    GLFWwindow *window;
    bool running;
    IPF_Root ipf_root;
    ArchiveData archive_data;
};

// Function declarations
bool initialize(Application &app);
void run(Application &app);
void cleanup(Application &app);
void render(Application &app);
void beginDockspace();
void renderMenuBar();
void render_panel(Application &app);
void flat_style();

// Initialize the application
bool initialize(Application &app)
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    // OpenGL context settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    app.window = glfwCreateWindow(1280, 720, "ImGui Docking Application", nullptr, nullptr);
    if (!app.window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(app.window);
    glfwSwapInterval(1);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    // io.IniFilename = nullptr; // 🚫 Disable .ini file

    // Setup ImGui backend
    ImGui_ImplGlfw_InitForOpenGL(app.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::StyleColorsLight();

    app.running = true;
    return true;
}

// Main loop
void run(Application &app)
{
    while (!glfwWindowShouldClose(app.window) && app.running)
    {
        glfwPollEvents();
        render(app);
        glfwSwapBuffers(app.window);
    }
}

// Cleanup resources
void cleanup(Application &app)
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(app.window);
    glfwTerminate();
}
// Rendering function
void render(Application &app)
{
    // Start new ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    beginDockspace();
    renderMenuBar(); // Add menu bar for theme selection
    render_panel(app);

    // Render ImGui
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(app.window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow *backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

// Docking space setup
void beginDockspace()
{
    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End();
}

// Function to render the menu bar with theme selection
void renderMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Themes"))
        {
            if (ImGui::MenuItem("Dark Theme"))
                ImGui::StyleColorsDark();
            if (ImGui::MenuItem("Light Theme"))
                ImGui::StyleColorsLight();
            if (ImGui::MenuItem("Classic Theme"))
                ImGui::StyleColorsClassic();
            if (ImGui::MenuItem("Flat Theme"))
                flat_style();
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void insert_into_tree(DirectoryNode &root, const std::string &path, int file_index)
{
    size_t last_slash = path.find_last_of('/');

    std::string directory = (last_slash == std::string::npos) ? "" : path.substr(0, last_slash);
    std::string filename = path.substr(last_slash + 1);

    DirectoryNode *current = &root;

    // Navigate into the correct subdirectory
    std::stringstream ss(directory);
    std::string segment;
    while (std::getline(ss, segment, '/'))
    {
        current = &current->subdirectories[segment]; // Navigate into subdirectory
    }

    // Add only the filename to the last folder, not as another subdirectory
    current->files.push_back(file_index);
}
void display_directory_tree(DirectoryNode &node, const std::string &folder_name, Application &app)
{
    if (ImGui::TreeNode(folder_name.c_str())) // Folder as TreeNode
    {
        // Show files in this folder
        for (int file_index : node.files)
        {
            const auto &entry = app.ipf_root.ipf_file_table[file_index];
            std::string filename = entry.directory_name.substr(entry.directory_name.find_last_of('/') + 1);

            bool selected = (app.archive_data.selected_number == file_index);
            if (ImGui::Selectable(filename.c_str(), selected))
            {
                app.archive_data.selected_number = file_index;
            }
        }

        // Recursively display subdirectories
        for (auto it = node.subdirectories.begin(); it != node.subdirectories.end(); ++it)
        {
            display_directory_tree(it->second, it->first, app);
        }

        ImGui::TreePop();
    }
}

void left_panel(Application &app)
{
    if (app.archive_data.temp_number != app.archive_data.selected_number)
    {
        app.archive_data.temp_number = app.archive_data.selected_number;
    }

    ImGui::Begin("Archive Browser");
    ImGui::Text("Browse archive files by category.");
    ImGui::Separator();

    // **Only populate the directory tree once**
    if (app.archive_data.directory_node.subdirectories.empty())
    {
        for (size_t i = 0; i < app.ipf_root.ipf_file_table.size(); ++i)
        {
            const auto &entry = app.ipf_root.ipf_file_table[i];
            insert_into_tree(app.archive_data.directory_node, entry.directory_name, i);
        }
    }

    // Display the directory tree
    ImVec2 child_size = ImVec2(0, 0);
    ImGui::BeginChild("IPFDataTree", child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

    for (auto it = app.archive_data.directory_node.subdirectories.begin();
         it != app.archive_data.directory_node.subdirectories.end(); ++it)
    {
        display_directory_tree(it->second, it->first, app);
    }

    ImGui::EndChild();
    ImGui::End();
}

void render_decompressed_tab(Application &app)
{
    ImGui::Text("Decompressed Data:");

    if (!app.ipf_root.ipf_file_table.empty())
    {
        ImGui::BeginChild("Decompressed Scroll", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(app.archive_data.decompressed_data.size() / 16 + 1));

        while (clipper.Step())
        {
            for (int line = clipper.DisplayStart; line < clipper.DisplayEnd; ++line)
            {
                uint64_t start_idx = static_cast<uint64_t>(line) * 16;
                std::string line_str;

                // Line number
                char line_number[16];
                snprintf(line_number, sizeof(line_number), "%08X: ", (unsigned int)start_idx);
                line_str += line_number;

                // Hexadecimal bytes
                for (uint64_t j = 0; j < 16; ++j)
                {
                    if (start_idx + j < app.archive_data.decompressed_data.size())
                    {
                        char hex_byte[4];
                        snprintf(hex_byte, sizeof(hex_byte), "%02X ", app.archive_data.decompressed_data[start_idx + j]);
                        line_str += hex_byte;
                    }
                    else
                    {
                        line_str += "   ";
                    }
                }

                // ASCII representation
                line_str += " ";
                for (uint64_t j = 0; j < 16; ++j)
                {
                    if (start_idx + j < app.archive_data.decompressed_data.size())
                    {
                        char c = app.archive_data.decompressed_data[start_idx + j];
                        line_str += (c >= 32 && c <= 126) ? c : '.';
                    }
                    else
                    {
                        line_str += " ";
                    }
                }

                ImGui::TextUnformatted(line_str.c_str());
            }
        }

        clipper.End();
        ImGui::EndChild();
    }
    else
    {
        ImGui::Text("No files loaded.");
    }
}
bool valid_png(Application &app)
{
    const std::string &filename = app.ipf_root.ipf_file_table[app.archive_data.selected_number].directory_name;

    if (filename.find("png") != std::string::npos ||
        filename.find("PNG") != std::string::npos)
    {
        return true;
    }
    return (app.archive_data.decompressed_data.size() >= 8 &&
            app.archive_data.decompressed_data[0] == 0x89 && app.archive_data.decompressed_data[1] == 0x50 &&
            app.archive_data.decompressed_data[2] == 0x4E && app.archive_data.decompressed_data[3] == 0x47 &&
            app.archive_data.decompressed_data[4] == 0x0D && app.archive_data.decompressed_data[5] == 0x0A &&
            app.archive_data.decompressed_data[6] == 0x1A && app.archive_data.decompressed_data[7] == 0x0A);
}

bool valid_jpeg(Application &app)
{
    const std::string &filename = app.ipf_root.ipf_file_table[app.archive_data.selected_number].directory_name;

    if (filename.find("jpg") != std::string::npos ||
        filename.find("jpeg") != std::string::npos ||
        filename.find("JPG") != std::string::npos ||
        filename.find("JPEG") != std::string::npos)
    {
        return true;
    }

    // Check if the file has JPEG magic bytes
    return (app.archive_data.decompressed_data.size() >= 3 &&
            app.archive_data.decompressed_data[0] == 0xFF &&
            app.archive_data.decompressed_data[1] == 0xD8 &&
            app.archive_data.decompressed_data[2] == 0xFF);
}

bool valid_webp(Application &app)
{
    return (app.archive_data.decompressed_data.size() >= 12 &&
            app.archive_data.decompressed_data[0] == 0x52 && app.archive_data.decompressed_data[1] == 0x49 &&
            app.archive_data.decompressed_data[2] == 0x46 && app.archive_data.decompressed_data[3] == 0x46); // "RIFF"
}

bool valid_dds(Application &app)
{
    const std::string &filename = app.ipf_root.ipf_file_table[app.archive_data.selected_number].directory_name;

    if (filename.find("dds") != std::string::npos ||
        filename.find("DDS") != std::string::npos)
    {
        return true;
    }
    return (app.archive_data.decompressed_data.size() >= 4 &&
            app.archive_data.decompressed_data[0] == 'D' && app.archive_data.decompressed_data[1] == 'D' &&
            app.archive_data.decompressed_data[2] == 'S' && app.archive_data.decompressed_data[3] == ' '); // "DDS "
}

bool valid_bmp(Application &app)
{
    const std::string &filename = app.ipf_root.ipf_file_table[app.archive_data.selected_number].directory_name;

    if (filename.find("bmp") != std::string::npos ||
        filename.find("BMP") != std::string::npos)
    {
        return true;
    }
    return (app.archive_data.decompressed_data.size() >= 2 &&
            app.archive_data.decompressed_data[0] == 'B' && app.archive_data.decompressed_data[1] == 'M'); // "BM"
}

bool valid_ico(Application &app)
{
    return (app.archive_data.decompressed_data.size() >= 4 &&
            app.archive_data.decompressed_data[0] == 0x00 && app.archive_data.decompressed_data[1] == 0x00 &&
            app.archive_data.decompressed_data[2] == 0x01 && app.archive_data.decompressed_data[3] == 0x00); // ICO header
}

bool valid_tga(Application &app)
{
    const std::string &filename = app.ipf_root.ipf_file_table[app.archive_data.selected_number].directory_name;

    if (filename.find("tga") != std::string::npos ||
        filename.find("TGA") != std::string::npos)
    {
        return true;
    }
    return (app.archive_data.decompressed_data.size() >= 18 &&
            (app.archive_data.decompressed_data[1] == 0 || app.archive_data.decompressed_data[1] == 1) && // Uncompressed or RLE-compressed
            (app.archive_data.decompressed_data[2] == 2 || app.archive_data.decompressed_data[2] == 10)); // Truecolor (2) or RLE Truecolor (10)
}

bool valid_gif(Application &app)
{
    return (app.archive_data.decompressed_data.size() >= 6 &&
            app.archive_data.decompressed_data[0] == 'G' && app.archive_data.decompressed_data[1] == 'I' &&
            app.archive_data.decompressed_data[2] == 'F' && app.archive_data.decompressed_data[3] == '8' &&
            (app.archive_data.decompressed_data[4] == '7' || app.archive_data.decompressed_data[4] == '9') &&
            app.archive_data.decompressed_data[5] == 'a'); // "GIF87a" or "GIF89a"
}

bool valid_tiff(Application &app)
{
    return (app.archive_data.decompressed_data.size() >= 4 &&
            ((app.archive_data.decompressed_data[0] == 'I' && app.archive_data.decompressed_data[1] == 'I' && app.archive_data.decompressed_data[2] == 0x2A && app.archive_data.decompressed_data[3] == 0x00) || // Little-endian TIFF
             (app.archive_data.decompressed_data[0] == 'M' && app.archive_data.decompressed_data[1] == 'M' && app.archive_data.decompressed_data[2] == 0x00 && app.archive_data.decompressed_data[3] == 0x2A))); // Big-endian TIFF
}

bool check_valid_image(Application &app)
{
    return valid_png(app) || valid_jpeg(app) ||
           valid_webp(app) || valid_dds(app) ||
           valid_bmp(app) || valid_ico(app) ||
           valid_tga(app) || valid_gif(app) ||
           valid_tiff(app);
}
void render_image(Application &app)
{
    // Check if the same file is already loaded
    if (app.archive_data.last_selected_image == app.archive_data.last_selected_number)
    {
        // Just display the cached image
        ImGui::Text("Image Info : %d width x %d height %d channels", app.archive_data.width, app.archive_data.height, app.archive_data.channels);
        ImGui::Image((ImTextureID)app.archive_data.texture, ImVec2(app.archive_data.width, app.archive_data.height));
        return;
    }

    if (app.archive_data.texture == 0)
    {
        glGenTextures(1, &app.archive_data.texture);
    }

    // Load new image
    app.archive_data.image_data = stbi_load_from_memory(app.archive_data.decompressed_data.data(), static_cast<int>(app.archive_data.decompressed_data.size()),
                                                        &app.archive_data.width, &app.archive_data.height,
                                                        &app.archive_data.channels, 4);
    if (!app.archive_data.image_data)
    {
        ImGui::Text("Failed to load image.");
        return;
    }

    ImGui::Text("Image Info : %d width x %d height %d channels", app.archive_data.width, app.archive_data.height, app.archive_data.channels);

    // Generate texture if not created
    glGenTextures(1, &app.archive_data.texture);
    glBindTexture(GL_TEXTURE_2D, app.archive_data.texture);

    // Upload to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app.archive_data.width, app.archive_data.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, app.archive_data.image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Free image data (no longer needed after uploading to OpenGL)
    stbi_image_free(app.archive_data.image_data);
    app.archive_data.image_data = nullptr;

    // Update last selected number to avoid reloading
    app.archive_data.last_selected_image = app.archive_data.selected_number;

    // Display image
    ImGui::Image((ImTextureID)app.archive_data.texture, ImVec2(app.archive_data.width, app.archive_data.height));
}

void render_dds_image(Application &app)
{
    if (app.archive_data.decompressed_data.size() < sizeof(DDSHeader))
    {
        ImGui::Text("Invalid DDS file.");
        return;
    }

    const DDSHeader *ddsHeader = reinterpret_cast<const DDSHeader *>(app.archive_data.decompressed_data.data());
    if (ddsHeader->magic != DDS_MAGIC)
    {
        ImGui::Text("Invalid DDS header.");
        return;
    }

    size_t offset = sizeof(DDSHeader);
    GLenum format = 0;

    // Check for DX10 header
    if (ddsHeader->pixelFormat.fourCC == DDS_DX10)
    {
        if (app.archive_data.decompressed_data.size() < offset + sizeof(DDSHeaderDX10))
        {
            ImGui::Text("Invalid DDS DX10 file.");
            return;
        }
        const DDSHeaderDX10 *dx10Header = reinterpret_cast<const DDSHeaderDX10 *>(app.archive_data.decompressed_data.data() + offset);
        offset += sizeof(DDSHeaderDX10);

        // DXGI_FORMAT mapping
        switch (dx10Header->dxgiFormat)
        {
        case 71:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break; // BC1
        case 74:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break; // BC2
        case 77:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break; // BC3
        case 80:
            format = GL_COMPRESSED_RED_RGTC1;
            break; // BC4
        case 83:
            format = GL_COMPRESSED_RG_RGTC2;
            break; // BC5
        case 95:
            format = GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB;
            break; // BC6H
        case 98:
            format = GL_COMPRESSED_RGBA_BPTC_UNORM_ARB;
            break; // BC7
        default:
            ImGui::Text("Unsupported DX10 DDS format.");
            return;
        }
    }
    else
    {
        // Legacy DDS formats
        switch (ddsHeader->pixelFormat.fourCC)
        {
        case 0x31545844:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break; // "DXT1"
        case 0x33545844:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break; // "DXT3"
        case 0x35545844:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break; // "DXT5"
        default:
            ImGui::Text("Unsupported legacy DDS format.");
            return;
        }
    }

    if (app.archive_data.texture == 0)
    {
        glGenTextures(1, &app.archive_data.texture);
    }

    glBindTexture(GL_TEXTURE_2D, app.archive_data.texture);

    // Calculate block size
    size_t blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || format == GL_COMPRESSED_RED_RGTC1) ? 8 : 16;
    size_t dataSize = app.archive_data.decompressed_data.size() - offset;

    // Use only level 0 (no mipmaps yet)
    size_t imageSize = ((ddsHeader->width + 3) / 4) * ((ddsHeader->height + 3) / 4) * blockSize;
    if (imageSize > dataSize)
    {
        ImGui::Text("DDS file size mismatch.");
        return;
    }

    glCompressedTexImage2D(GL_TEXTURE_2D, 0, format, ddsHeader->width, ddsHeader->height, 0, imageSize,
                           app.archive_data.decompressed_data.data() + offset);

    // Disable mipmapping (for debugging)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Store metadata
    app.archive_data.width = ddsHeader->width;
    app.archive_data.height = ddsHeader->height;

    ImGui::Text("DDS Image: %d x %d", app.archive_data.width, app.archive_data.height);
    ImGui::Image((ImTextureID)app.archive_data.texture, ImVec2(app.archive_data.width, app.archive_data.height));
}

void render_preview_tab(Application &app)
{
    ImGui::Text("Preview Data:");

    if (!app.ipf_root.ipf_file_table.empty())
    {

        if (check_valid_image(app))
        {
            if (valid_dds(app))
            {
                render_dds_image(app);
            }
            else
            {
                render_image(app);
            }
        }
    }
    else
    {
        ImGui::Text("No files loaded.");
    }
}

void middle_panel(Application &app)
{
    ImGui::Begin("Extracted Data");

    if (ImGui::BeginTabBar("IPF Data Tabs"))
    {
        app.archive_data.preview_tab_active = false; // Reset flag before checking active tab

        if (ImGui::BeginTabItem("Decompressed"))
        {
            render_decompressed_tab(app);
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Preview"))
        {
            if (ImGui::IsItemFocused())
            {

                if (!app.archive_data.preview_tab_active)
                {
                    app.archive_data.preview_tab_active = true;
                }
            }
            render_preview_tab(app);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void right_panel(Application &app)
{
    // File Info Panel
    ImGui::Begin("File Info");
    ImVec2 child_size = ImVec2(0, 0); // Adjust height as needed
    ImGui::BeginChild("FileInfo", child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

    ImGui::PushTextWrapPos();

    ImGui::Text("Show selected archive data info.");

    if (!app.ipf_root.ipf_file_table.empty() && app.archive_data.selected_number < app.ipf_root.ipf_file_table.size())
    {
        const auto &selected_file = app.ipf_root.ipf_file_table[app.archive_data.selected_number];

        ImGui::Text("Index: %u", app.archive_data.selected_number);
        ImGui::Text("File Name: %s", selected_file.container_name.c_str());
        ImGui::Text("Directory: %s", selected_file.directory_name.c_str());
        ImGui::Text("Compressed Size: %u bytes", selected_file.file_size_compressed);
        ImGui::Text("Uncompressed Size: %u bytes", selected_file.file_size_uncompressed);
        ImGui::Text("File Offset: 0x%X", selected_file.file_pointer);

        if (!app.archive_data.decompressed_data.empty())
        {
            ImGui::Text("Decompressed Data Size: %lu bytes", app.archive_data.decompressed_data.size());

            // Generate filename: container_name_index.bin
            std::string filename = selected_file.container_name + "_" + std::to_string(app.archive_data.selected_number) + ".bin";

            // Extract button
            if (ImGui::Button("Extract Decompressed"))
            {
                std::ofstream file(filename, std::ios::binary);
                if (file)
                {
                    file.write(reinterpret_cast<const char *>(app.archive_data.decompressed_data.data()), app.archive_data.decompressed_data.size());
                    file.close();

                    // Store filename and timestamp for 5-second display
                    app.archive_data.last_extracted_filename = filename;
                    app.archive_data.extract_message_time = ImGui::GetTime();
                }
            }

            // Show extraction message for 5 seconds
            if (!app.archive_data.last_extracted_filename.empty() && (ImGui::GetTime() - app.archive_data.extract_message_time) < 5.0)
            {
                ImGui::Text("Extracted: %s", app.archive_data.last_extracted_filename.c_str());
            }
        }
        else
        {
            ImGui::Text("Decompressed Data: Not Loaded");
        }
    }
    else
    {
        ImGui::Text("No files loaded.");
    }

    ImGui::PopTextWrapPos();
    ImGui::EndChild();
    ImGui::End();
}

// Render the UI panels
void render_panel(Application &app)
{

    left_panel(app);
    if (!app.ipf_root.ipf_file_table.empty())
    {
        if (app.archive_data.selected_number != app.archive_data.last_selected_number)
        {
            // Reset image properties
            app.archive_data.width = 0;
            app.archive_data.height = 0;
            app.archive_data.channels = 0;

            app.archive_data.decompressed_data = extract_data(app.ipf_root, app.archive_data.selected_number);
            app.archive_data.last_selected_number = app.archive_data.selected_number;
        }
    }
    middle_panel(app);

    right_panel(app);
}

void flat_style()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImGui::StyleColorsDark(&style);
    style.WindowRounding = 5.3f;
    style.FrameRounding = 2.3f;
    style.ScrollbarRounding = 0;

    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.10f, 0.85f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.90f, 0.80f, 0.80f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.65f, 0.65f, 0.45f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
}

#endif // APPLICATION_H
