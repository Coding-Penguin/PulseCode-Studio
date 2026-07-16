#pragma once
#include "pspch.h"
#include "uiWindow.h"

namespace PulseStudio {

	struct FileNode
	{
		std::string name;
		std::string path;
		bool isFolder = false;
		bool expanded = false;
		std::vector<FileNode> children;
	};

	class FileExplorer : public uiWindow
	{
	public:
		FileExplorer(const std::string& rootPath);
		virtual ~FileExplorer();

		virtual void OnUpdate(float deltaTime) override;
		virtual bool OnEvent(Event& event) override;
		virtual void DrawContent();

		void SetFileOpenCallback(std::function<void(const std::string& path)> callback);

	private:
		std::string m_RootPath;
		FileNode m_RootNode;
		bool m_NeedsRefresh = true;

		int m_HoveredLine = -1;
		int m_SelectedLine = -1;

		float m_LineHeight = 0.0f;

		float m_ScrollY = 0.0f;
		float m_TotalHeight = 0.0f;

		bool m_IsDraggingScrollbar = false;
		float m_DragStartY = 0.0f;
		float m_DragStartScrollY = 0.0f;

		void RefreshTree();
		void PopulateNode(FileNode& node, const std::filesystem::path& path);
		void DrawNode(const FileNode& node, int depth, float& y, float x, float width);
		void CalcTreeHeight(const FileNode& node, int depth, float& total);
		void DrawFolderIcon(float x, float y, bool expanded) const;
		void DrawFileIcon(float x, float y) const;
		int GetNodeIndexAtPosition(float mouseX, float mouseY) const;
		bool HitTestNode(const FileNode& node, int depth, float& y, float x, float width, float mouseX, float mouseY, int& hitIndex) const;

		std::function<void(const std::string&)> m_FileOpenCallback;

		struct VisibleNode
		{
			const FileNode* node;
			int depth;
			float y;
		};
		mutable std::vector<VisibleNode> m_VisibleNodes;
		void BuildVisibleList(const FileNode& node, int depth, float startY, float& yOffset) const;
	};

}
