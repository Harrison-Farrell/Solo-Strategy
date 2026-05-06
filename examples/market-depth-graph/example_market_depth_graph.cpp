// -----------------------------------------------------------------------------
// Author:      Harrison Farrell
// Project:     Solo-Strategy Trading System
// Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
//
// Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
// This program is distributed WITHOUT ANY WARRANTY; without even the
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
// -----------------------------------------------------------------------------

// 3rd party libraries
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"


static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int argc, char* argv[]) {
    // Suppress unused parameter warnings
    (void)argc;
    (void)argv;

    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window =
        glfwCreateWindow(800, 500, "Market Depth Chart", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);  // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Number of price levels to simulate on each side
    const int num_levels = 50;
    double buyX[num_levels * 2];
    double buyY[num_levels * 2];
    double sellX[num_levels * 2];
    double sellY[num_levels * 2];

    double midPrice = 101.0;
    double last_update_time = 0.0;

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Update market data simulation
        double current_time = ImGui::GetTime();
        if (current_time - last_update_time > 0.05) {  // 20Hz update rate
            last_update_time = current_time;

            // Random walk for mid price
            double r = (double)rand() / RAND_MAX;
            double mid_change = (r < 0.45) ? -0.02 : ((r > 0.55) ? 0.02 : 0);
            midPrice += mid_change;
            if (midPrice < 50) midPrice = 50;
            if (midPrice > 150) midPrice = 150;

            // Generate buy side (bids) - 50 levels deep
            double current_buy_price = midPrice - 0.02; // Small spread
            double current_buy_vol = 0;

            for (int i = 0; i < num_levels; ++i) {
                // Randomly vary the volume at each price level slightly
                double vol_added = (rand() % 80) + 20;

                int idx = (num_levels - 1 - i) * 2;

                buyX[idx + 1] = current_buy_price;
                buyY[idx + 1] = current_buy_vol;

                current_buy_vol += vol_added;

                buyX[idx] = current_buy_price;
                buyY[idx] = current_buy_vol;

                current_buy_price -= 0.05;  // tick size
            }

            // Generate sell side (asks) - 50 levels deep
            double current_sell_price = midPrice + 0.02; // Small spread
            double current_sell_vol = 0;

            for (int i = 0; i < num_levels; ++i) {
                double vol_added = (rand() % 80) + 20;

                int idx = i * 2;

                sellX[idx] = current_sell_price;
                sellY[idx] = current_sell_vol;

                current_sell_vol += vol_added;

                sellX[idx + 1] = current_sell_price;
                sellY[idx + 1] = current_sell_vol;

                current_sell_price += 0.05;  // tick size
            }
        }

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("Market Depth: AAPL", nullptr,
                     ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoBringToFrontOnFocus);

        if (ImPlot::BeginPlot("##MarketDepth", ImVec2(-1, -1))) {
            ImPlot::SetupAxes("Price ($)", "Cumulative Volume",
                              ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxesLimits(midPrice - 2.5, midPrice + 2.5, 0, 3000, ImGuiCond_Always);

            // Bids
            ImPlot::PushStyleColor(ImPlotCol_Line,
                                   ImVec4(0.0f, 0.78f, 0.31f, 1.0f));
            ImPlot::PushStyleColor(ImPlotCol_Fill,
                                   ImVec4(0.0f, 0.78f, 0.31f, 0.5f));
            ImPlot::PlotShaded("Bids (Buy)", buyX, buyY, num_levels * 2);
            ImPlot::PlotLine("Bids (Buy)", buyX, buyY, num_levels * 2);
            ImPlot::PopStyleColor(2);

            // Asks
            ImPlot::PushStyleColor(ImPlotCol_Line,
                                   ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
            ImPlot::PushStyleColor(ImPlotCol_Fill,
                                   ImVec4(0.9f, 0.2f, 0.2f, 0.5f));
            ImPlot::PlotShaded("Asks (Sell)", sellX, sellY, num_levels * 2);
            ImPlot::PlotLine("Asks (Sell)", sellX, sellY, num_levels * 2);
            ImPlot::PopStyleColor(2);

            ImPlot::EndPlot();
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}