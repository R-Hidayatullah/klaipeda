#if !defined(APPLICATION_H)
#define APPLICATION_H
#include "ipf.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Struct to hold application state
struct Application
{
    GLFWwindow *window;
    bool running;
    IPF_Root ipf_root;
};

// Function declarations
bool initialize(Application &app);
void run(Application &app);
void cleanup(Application &app);
void render(Application &app);
void beginDockspace();
void renderMenuBar();
void renderPanels(IPF_Root &ipf_root);

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
    io.IniFilename = nullptr; // 🚫 Disable .ini file

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
    renderPanels(app.ipf_root);

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

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

// Render the UI panels
void renderPanels(IPF_Root &ipf_root)
{
    // Archive Browser Panel
    ImGui::Begin("Archive Browser");
    ImGui::Text("Browse archive files by category.");
    ImGui::Separator();

    if (!ipf_root.ipf_file_table.empty())
    {
        if (ImGui::BeginTable("file_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
        {
            ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 50.0f);
            ImGui::TableSetupColumn("Directory Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Compressed Size", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Uncompressed Size", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(ipf_root.ipf_file_table.size());
            while (clipper.Step())
            {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
                {
                    const auto &entry = ipf_root.ipf_file_table[i];
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%d", i);
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(entry.directory_name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%u", entry.file_size_compressed);
                    ImGui::TableNextColumn();
                    ImGui::Text("%u", entry.file_size_uncompressed);
                }
            }

            ImGui::EndTable();
        }
    }
    else
    {
        ImGui::Text("No files loaded.");
    }

    ImGui::End();

    // File Viewer Panel
    ImGui::Begin("File Viewer");
    ImGui::Text("View files in binary, image, or 3D format.");
    ImGui::End();

    // File Info Panel
    ImGui::Begin("File Info");
    ImGui::Text("Show selected archive data info.");
    ImGui::End();
}

#endif // APPLICATION_H
