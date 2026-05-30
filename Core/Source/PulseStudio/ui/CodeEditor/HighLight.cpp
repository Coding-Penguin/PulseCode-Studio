#include "pspch.h"
#include "Highlight.h"
#include <cctype>

namespace PulseStudio {

    Highlight::Highlight(const Language& mode)
    {
        SetLanguage(mode);
    }

    void Highlight::SetLanguage(const Language& mode)
    {
        m_languageMode = mode;
        if (m_languageMode == Language::CPP)
        {
            InitCppKeywords();
        }
		// In the future, it will support more languages like Python, Java, etc.
        // For now, only C++ is implemented.
    }

    void Highlight::InitCppKeywords()
    {
        m_Keywords =
        {
            "alignas","alignof","and","and_eq","asm",
            "auto","bitand","bitor","bool","break",
            "case","catch","char","char8_t","char16_t",
            "char32_t","class","compl","concept","const",
            "consteval","constexpr","constinit",
            "const_cast","continue","co_await","co_return",
            "co_yield","decltype","default","delete",
            "do","double","dynamic_cast","else","enum",
            "explicit","export","extern","false","float",
            "for","friend","goto","if","import","inline",
            "int","long","module","mutable","namespace",
            "new","noexcept","not","not_eq","nullptr",
            "operator","or","or_eq","private","protected",
            "public","register","reinterpret_cast","requires",
            "return","short","signed","sizeof","static",
            "static_assert","static_cast","struct","switch",
            "template","this","thread_local","throw","true",
            "try","typedef","typeid","typename","union",
            "unsigned","using","virtual","void","volatile",
            "wchar_t","while","xor","xor_eq"
        };
    }

    std::vector<HighlightSpan> Highlight::HighlightLine(const std::string& line) const
    {
        if (m_languageMode == Language::CPP)
        {
            return HighlightCppLine(line);
        }
        else
        {
            return HighlightGenericLine(line);
        }
    }

    std::vector<HighlightSpan> Highlight::HighlightCppLine(const std::string& line) const
    {
        std::vector<HighlightSpan> spans;
        int len = (int)line.length();
        int i = 0;
        bool inString = false;
        bool inChar = false;
        bool inPreproc = false;
        bool inLineComment = false;

        while (i < len)
        {
            if (i == 0 && line[i] == '#')
            {
                inPreproc = true;
            }
            if (inPreproc)
            {
                int start = i;
                while (i < len && line[i] != '\n') i++;
                spans.push_back({ start, i, HighlightColor::Preprocessor });
                inPreproc = false;
                continue;
            }

            if (!inChar && !inLineComment && line[i] == '"')
            {
                int start = i;
                inString = true;
                i++;
                while (i < len && (line[i] != '"' || line[i - 1] == '\\')) i++;
                if (i < len && line[i] == '"') i++;
                spans.push_back({ start, i, HighlightColor::String });
                inString = false;
                continue;
            }

            if (!inString && !inLineComment && line[i] == '\'')
            {
                int start = i;
                inChar = true;
                i++;
                while (i < len && (line[i] != '\'' || line[i - 1] == '\\')) i++;
                if (i < len && line[i] == '\'') i++;
                spans.push_back({ start, i, HighlightColor::String });
                inChar = false;
                continue;
            }

            if (!inString && !inChar && !inLineComment && i + 1 < len && line[i] == '/' && line[i + 1] == '/')
            {
                int start = i;
                inLineComment = true;
                i += 2;
                while (i < len && line[i] != '\n') i++;
                spans.push_back({ start, i, HighlightColor::Comment });
                inLineComment = false;
                continue;
            }

            if (!inString && !inChar && !inLineComment && (std::isdigit(line[i]) || (line[i] == '.' && i + 1 < len && std::isdigit(line[i + 1]))))
            {
                int start = i;
                bool dot = false;
                while (i < len && (std::isdigit(line[i]) || line[i] == '.' || line[i] == 'e' || line[i] == 'E' ||
                    line[i] == '+' || line[i] == '-'))
                {
                    if (line[i] == '.')
                    {
                        if (dot) break;
                        dot = true;
                    }
                    i++;
                }
                spans.push_back({ start, i, HighlightColor::Number });
                continue;
            }

            if (!inString && !inChar && !inLineComment && (std::isalpha(line[i]) || line[i] == '_'))
            {
                int start = i;
                while (i < len && (std::isalnum(line[i]) || line[i] == '_')) i++;
                std::string token = line.substr(start, i - start);
                if (std::find(m_Keywords.begin(), m_Keywords.end(), token) != m_Keywords.end())
                {
                    spans.push_back({ start, i, HighlightColor::Keyword });
                }
                continue;
            }

            i++;
        }

        std::sort(spans.begin(), spans.end(),
            [](const HighlightSpan& a, const HighlightSpan& b) { return a.start < b.start; });
        std::vector<HighlightSpan> merged;
        for (auto& s : spans)
        {
            if (merged.empty() || merged.back().end < s.start)
                merged.push_back(s);
            else
                merged.back().end = std::max(merged.back().end, s.end);
        }
        return merged;
    }

    std::vector<HighlightSpan> Highlight::HighlightGenericLine(const std::string& line) const
    {
        return {};
    }

}