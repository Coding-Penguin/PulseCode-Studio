#pragma once
#include "pspch.h"
#include "uiWindow.h"
#include "PhotoRenderer.h"

namespace PulseCode {

	struct FileNode
	{
		std::string name;
		std::string path;
		bool isFolder = false;
		bool expanded = false;
		std::vector<FileNode> children;
	};

	enum class Filetype
	{
		CPP,
		C,
		Header,
		Python,
		Java,
		CSharp,
		Markdown,
		JSON,
		Lua,
		Unknown
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

		Filetype GetFileExtension(const std::string& path) const;
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

		const FileNode* m_HoveredNode = nullptr;

		float m_LastClickTime = 0.0f;
		const FileNode* m_LastClickedNode = nullptr;

		std::unique_ptr<PhotoRenderer> m_Folder_Close_Icon;
		std::unique_ptr<PhotoRenderer> m_Folder_Open_Icon;
		std::unique_ptr<PhotoRenderer> m_File_Icon;
		std::unique_ptr<PhotoRenderer> m_CPP_File_Icon;
		std::unique_ptr<PhotoRenderer> m_Python_File_Icon;

		void RefreshTree();
		void PopulateNode(FileNode& node, const std::filesystem::path& path);
		void DrawNode(const FileNode& node, int depth, float& y, float x, float width);
		void CalcTreeHeight(const FileNode& node, int depth, float& total);
		void DrawFolderIcon(float x, float y, bool expanded) const;
		void DrawFileIcon(float x, float y, Filetype type) const;
		int GetNodeIndexAtPosition(float mouseX, float mouseY) const;
		bool HitTestNode(const FileNode& node, int depth, float& y, float x, float width, float mouseX, float mouseY, int& hitIndex) const;
		std::string GetNodePath(const FileNode* node) const;

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
