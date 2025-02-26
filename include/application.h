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

struct ArchiveData
{
    std::vector<uint8_t> decompressed_data;
    uint32_t temp_number;
    uint32_t index_number;
    uint32_t selected_number;
    uint32_t last_selected_number;
    bool preview_tab_active;
    int width;
    int height;
    int channels;
    GLuint texture;
    unsigned char *image_data;
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

void left_panel(Application &app)
{
    if (app.archive_data.temp_number != app.archive_data.selected_number)
    {
        app.archive_data.temp_number = app.archive_data.selected_number;
    }

    // Archive Browser Panel
    ImGui::Begin("Archive Browser");
    ImGui::Text("Browse archive files by category.");
    ImGui::Separator();
    ImVec2 child_size = ImVec2(0, 0); // Adjust height as needed
    ImGui::BeginChild("IPFDataList", child_size, true, ImGuiWindowFlags_HorizontalScrollbar);

    if (!app.ipf_root.ipf_file_table.empty())
    {
        if (ImGui::BeginTable("file_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Directory Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Compressed Size", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Uncompressed Size", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(app.ipf_root.ipf_file_table.size());
            while (clipper.Step())
            {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                {
                    const auto &entry = app.ipf_root.ipf_file_table[i];

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    // Create a selectable row spanning all columns
                    std::string label = "##row" + std::to_string(i);
                    bool selected = (app.archive_data.selected_number == i);

                    if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        app.archive_data.selected_number = i;
                    }

                    // Populate row data
                    ImGui::SameLine();
                    ImGui::Text("%d", i);
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(entry.directory_name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%u", entry.file_size_compressed);
                    ImGui::TableNextColumn();
                    ImGui::Text("%u", entry.file_size_uncompressed);

                    // Copy to clipboard if selected and Ctrl + C is pressed
                    if (selected && ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::GetIO().KeyCtrl)
                    {
                        std::string clipboard_text = entry.directory_name;
                        ImGui::SetClipboardText(clipboard_text.c_str());
                    }
                }
            }

            ImGui::EndTable();
        }
    }
    else
    {
        ImGui::Text("No files loaded.");
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
bool valid_png(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 8 &&
            data_ptr[0] == 0x89 && data_ptr[1] == 0x50 &&
            data_ptr[2] == 0x4E && data_ptr[3] == 0x47 &&
            data_ptr[4] == 0x0D && data_ptr[5] == 0x0A &&
            data_ptr[6] == 0x1A && data_ptr[7] == 0x0A);
}

bool valid_jpeg(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 3 &&
            data_ptr[0] == 0xFF && data_ptr[1] == 0xD8 &&
            data_ptr[2] == 0xFF);
}

bool valid_webp(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 12 &&
            data_ptr[0] == 0x52 && data_ptr[1] == 0x49 &&
            data_ptr[2] == 0x46 && data_ptr[3] == 0x46); // "RIFF"
}

bool valid_dds(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 4 &&
            data_ptr[0] == 'D' && data_ptr[1] == 'D' &&
            data_ptr[2] == 'S' && data_ptr[3] == ' '); // "DDS "
}

bool valid_bmp(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 2 &&
            data_ptr[0] == 'B' && data_ptr[1] == 'M'); // "BM"
}

bool valid_ico(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 4 &&
            data_ptr[0] == 0x00 && data_ptr[1] == 0x00 &&
            data_ptr[2] == 0x01 && data_ptr[3] == 0x00); // ICO header
}

bool valid_tga(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 18 &&
            (data_ptr[1] == 0 || data_ptr[1] == 1) && // Uncompressed or RLE-compressed
            (data_ptr[2] == 2 || data_ptr[2] == 10)); // Truecolor (2) or RLE Truecolor (10)
}

bool valid_gif(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 6 &&
            data_ptr[0] == 'G' && data_ptr[1] == 'I' &&
            data_ptr[2] == 'F' && data_ptr[3] == '8' &&
            (data_ptr[4] == '7' || data_ptr[4] == '9') &&
            data_ptr[5] == 'a'); // "GIF87a" or "GIF89a"
}

bool valid_tiff(const uint8_t *data_ptr, size_t data_size)
{
    return (data_size >= 4 &&
            ((data_ptr[0] == 'I' && data_ptr[1] == 'I' && data_ptr[2] == 0x2A && data_ptr[3] == 0x00) || // Little-endian TIFF
             (data_ptr[0] == 'M' && data_ptr[1] == 'M' && data_ptr[2] == 0x00 && data_ptr[3] == 0x2A))); // Big-endian TIFF
}

bool check_valid_image(const uint8_t *data_ptr, size_t data_size)
{
    return valid_png(data_ptr, data_size) || valid_jpeg(data_ptr, data_size) ||
           valid_webp(data_ptr, data_size) || valid_dds(data_ptr, data_size) ||
           valid_bmp(data_ptr, data_size) || valid_ico(data_ptr, data_size) ||
           valid_tga(data_ptr, data_size) || valid_gif(data_ptr, data_size) ||
           valid_tiff(data_ptr, data_size);
}

void render_image(Application &app, const uint8_t *data_ptr, size_t data_size)
{

    // Load image from memory using stb_image
    app.archive_data.image_data = stbi_load_from_memory(data_ptr, static_cast<int>(data_size), &app.archive_data.width, &app.archive_data.height, &app.archive_data.channels, 4);

    if (!app.archive_data.image_data)
    {
        ImGui::Text("Failed to load image.");
        return;
    }

    // Generate OpenGL texture
    if (app.archive_data.texture == 0)
    {
        glGenTextures(1, &app.archive_data.texture);
    }
    glBindTexture(GL_TEXTURE_2D, app.archive_data.texture);

    // Upload texture to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, app.archive_data.width, app.archive_data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, app.archive_data.image_data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Display image in ImGui
    ImGui::Image((ImTextureID)app.archive_data.texture, ImVec2(app.archive_data.width, app.archive_data.height));
}

void render_preview_tab(Application &app)
{
    ImGui::Text("Preview Data:");

    if (!app.ipf_root.ipf_file_table.empty())
    {

        // Check file header for supported formats
        const uint8_t *data_ptr = reinterpret_cast<const uint8_t *>(app.archive_data.decompressed_data.data());
        size_t data_size = app.archive_data.decompressed_data.size();
        if (check_valid_image(data_ptr, data_size))
        {
            render_image(app, data_ptr, data_size);
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

// Render the UI panels
void render_panel(Application &app)
{

    left_panel(app);
    if (!app.ipf_root.ipf_file_table.empty())
    {
        if (app.archive_data.selected_number != app.archive_data.last_selected_number)
        {
            // Cleanup previous image data
            if (app.archive_data.image_data)
            {
                stbi_image_free(app.archive_data.image_data);
                app.archive_data.image_data = nullptr;
            }

            // Reset image properties
            app.archive_data.width = 0;
            app.archive_data.height = 0;
            app.archive_data.channels = 0;

            app.archive_data.decompressed_data = extract_data(app.ipf_root, app.archive_data.selected_number);
            app.archive_data.last_selected_number = app.archive_data.selected_number;
        }
    }
    middle_panel(app);

    // File Info Panel
    ImGui::Begin("File Info");
    ImGui::Text("Show selected archive data info.");
    if (!app.ipf_root.ipf_file_table.empty())
    {
    }
    else
    {
        ImGui::Text("No files loaded.");
    }
    ImGui::End();
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
