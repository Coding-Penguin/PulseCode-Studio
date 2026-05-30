#pragma once
#include <vector>
#include <string>
#include "glm/glm.hpp"

namespace PulseStudio {

	enum class HighlightColor
	{
		Default,
		Keyword,
		String,
		Comment,
		Number,
		Preprocessor,
		Macro,
		Object,
		Variable
	};

	struct HighlightSpan
	{
		int start;
		int end;
		HighlightColor color;
	};

	enum class Language
	{
		CPP,
		Python,
		Java
	};

	class Highlight
	{
	public:
		Highlight(const Language& mode = Language::CPP);
		void SetLanguage(const Language& mode);
		std::vector<HighlightSpan> HighlightLine(const std::string& line) const;

		glm::vec3 GetColorForHighlight(HighlightColor color)
		{
			switch (color)
			{
			case HighlightColor::Keyword:     return glm::vec3(0.5f, 0.8f, 1.0f);
			case HighlightColor::String:      return glm::vec3(0.8f, 0.6f, 0.2f);
			case HighlightColor::Comment:     return glm::vec3(0.3f, 0.7f, 0.3f);
			case HighlightColor::Number:      return glm::vec3(0.7f, 0.5f, 0.8f);
			case HighlightColor::Preprocessor:return glm::vec3(0.7f, 0.4f, 0.9f);
			case HighlightColor::Macro:       return glm::vec3(0.9f, 0.6f, 0.2f);
			case HighlightColor::Object:	  return glm::vec3(0.5f, 0.8f, 0.5f);
			case HighlightColor::Variable:	  return glm::vec3(0.9f, 0.9f, 1.0f);
			default:                          return glm::vec3(0.9f, 0.9f, 0.95f);
			}
		}
	private:
		std::vector<std::string> m_Keywords;
		Language m_languageMode;

		void InitCppKeywords();
		std::vector<HighlightSpan> HighlightCppLine(const std::string& line) const;
		std::vector<HighlightSpan> HighlightGenericLine(const std::string& line) const;
	};

}
