#include "imgui_impl_opengl3.h"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include "GL/gl.h"

#include <expected>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <print>

class Imenu {
    public:
    SDL_Window* window = nullptr;
    bool should_close = false;

    std::expected<void, std::string> init() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
            return std::unexpected(std::format("Error SDL_Init: {}", SDL_GetError()));
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        window = SDL_CreateWindow(
            "Test",
            800,
            600,
            SDL_WINDOW_OPENGL
        );
        if (!window) {
            return std::unexpected(std::format("Error SDL_CreateWindow: {}", SDL_GetError()));
        };

        context = SDL_GL_CreateContext(window);
        if (!context) {
            return std::unexpected(std::format("Error SDL_GL_CreateContext: {}", SDL_GetError()));
        };

        SDL_GL_MakeCurrent(window, context);
        SDL_GL_SetSwapInterval(1);

        auto imguiResult = init_imgui();
        if (!imguiResult) {
            return std::unexpected(imguiResult.error());
        }

        return {};
    }

    ~Imenu() {
        if (imgui_initialized) {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
        }

        if (context) SDL_GL_DestroyContext(context);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void handle_events() {
        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL3_ProcessEvent(&event);

            if (event.type == SDL_EVENT_QUIT
                    || (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
               ) {
                should_close = true;
            }
        }
    }

    void nextFrame() {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        set_ui();
        render_ui();

        SDL_GL_SwapWindow(window);
    }

    private:
    SDL_GLContext context = nullptr;
    bool imgui_initialized = false;

    void set_ui()  {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(io.DisplaySize);

        ImGui::Begin(
            "imenu",
            nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoSavedSettings
        );

        ImGui::Text("Hello test");

        if (ImGui::Button("Quit")) should_close = true;

        ImGui::End();
    }

    void render_ui() {
        ImGui::Render();

        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    std::expected<void, std::string> init_imgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        ImGui::StyleColorsDark();

        if (!ImGui_ImplSDL3_InitForOpenGL(window, context)) {
            ImGui::DestroyContext();
            return std::unexpected(std::format("ImGui_ImplSDL3_InitForOpenGL {}", SDL_GetError()));
        };
        if (!ImGui_ImplOpenGL3_Init("#version 460")) {
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            return std::unexpected(std::format("ImGui_ImplOpenGL3_Init {}", SDL_GetError()));
        };
        imgui_initialized = true;

        return {};
    }
};

int main() {
    Imenu imenu;

    auto imenuInitResult = imenu.init();
    if (!imenuInitResult) {
        std::println(stderr, "{}", imenuInitResult.error());
        return 1;
    }

    while (!imenu.should_close)  {
        imenu.handle_events();
        imenu.nextFrame();
    }

    return 0;
}
