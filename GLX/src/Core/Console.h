#include <algorithm>
#include <cstring>
#include <imgui.h>

class Console
{
public:
    static ImGuiTextBuffer Buf;
    static ImGuiTextFilter Filter;
    static ImVector<int> LineOffsets; // Index to lines offset
    static bool ScrollToBottom;

    Console()
    {
        Clear();
    }

    void Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    static void AddLog(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        size_t old_size = Buf.size();
        Buf.appendfv(fmt, args);
        va_end(args);

        for (size_t new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
        if (LineOffsets.empty() || Buf[LineOffsets.back() - 1] != '\n')
            LineOffsets.push_back(Buf.size());
        ScrollToBottom = true;
    }

    void Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open)) {
            ImGui::End();
            return;
        }

        if (ImGui::Button("Clear"))
            Clear();
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (copy)
            ImGui::LogToClipboard();

        if (Filter.IsActive()) {
            const char* buf_begin = Buf.begin();
            const char* line = buf_begin;
            for (int line_no = 0; line != NULL; line_no++) {
                const char* line_end = (line_no < LineOffsets.Size) ? buf_begin + LineOffsets[line_no] : NULL;
                if (Filter.PassFilter(line, line_end)) {
                    ImGui::TextUnformatted(line, line_end);
                }
                line = line_end && line_end[1] ? line_end + 1 : NULL;
            }
        }
        else {
            ImGui::TextUnformatted(Buf.begin());
        }

        if (ScrollToBottom)
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::EndChild();
        ImGui::End();
    }
};
