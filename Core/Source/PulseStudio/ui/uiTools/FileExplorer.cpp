#include "pspch.h"
#include "FileExplorer.h"
#include "TextRenderer.h"
#include "PulseStudio/Log.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "PulseStudio/Application.h"

namespace PulseStudio {

	FileExplorer::FileExplorer(const std::string& rootPath)
		: uiWindow("File Explorer"), m_RootPath(rootPath)
	{
		m_LineHeight = TextRenderer::GetFontSize() * 1.35f;
		SetSize(0, 110, 250, 600);
		RefreshTree();
	}

	FileExplorer::~FileExplorer() {}

	void FileExplorer::SetFileOpenCallback(std::function<void(const std::string&)> callback)
	{
		m_FileOpenCallback = callback;
	}

	void FileExplorer::RefreshTree()
	{
		m_RootNode.name = std::filesystem::path(m_RootPath).filename().string();
		m_RootNode.path = m_RootPath;
		m_RootNode.isFolder = true;
		m_RootNode.expanded = true;
		m_RootNode.children.clear();
		PopulateNode(m_RootNode, m_RootPath);
		m_NeedsRefresh = false;
	}

	void FileExplorer::PopulateNode(FileNode& node, const std::filesystem::path& path)
	{
		try
		{
			for (const auto& entry : std::filesystem::directory_iterator(path))
			{
				FileNode child;
				child.name = entry.path().filename().string();
				child.path = entry.path().string();
				child.isFolder = entry.is_directory();
				child.expanded = false;
				node.children.push_back(child);
			}
			std::sort(node.children.begin(), node.children.end(),
				[](const FileNode& a, const FileNode& b)
				{
					if (a.isFolder != b.isFolder)
					{
						return a.isFolder > b.isFolder;
					}
					return a.name < b.name;
				});
		}
		catch (const std::exception& e)
		{
			PS_CORE_WARN("Failed to read directory: {}", path.string());
		}
	}

	void FileExplorer::OnUpdate(float deltaTime)
	{
		if (!IsVisible()) return;
		DrawContent();
	}

	bool FileExplorer::OnEvent(Event& event)
	{
		if (!IsVisible()) return false;

		if (uiWindow::OnEvent(event)) return true;

		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& e)
			{
			float contentH = GetHeight() - 30;
			float maxScroll = std::max(0.0f, m_TotalHeight - contentH);
			if (maxScroll <= 0) return false;

			m_ScrollY -= e.GetYOffset() * 20.0f;
			if (m_ScrollY < 0) m_ScrollY = 0;
			if (m_ScrollY > maxScroll) m_ScrollY = maxScroll;
			return true;
			});

		dispatcher.Dispatch<MouseButtonPressedEvent>([this](MouseButtonPressedEvent& e)
			{
			float mx = e.GetMouseX(), my = e.GetMouseY();
			float contentX = GetX();
			float contentY = GetY() + 30;
			float contentW = GetWidth();
			float contentH = GetHeight() - 30;

			float scrollbarX = contentX + contentW - 8;
			if (mx >= scrollbarX) return false;

			m_VisibleNodes.clear();
			float yOffset = 0.0f;
			BuildVisibleList(m_RootNode, 0, contentY, yOffset);

			for (const auto& vn : m_VisibleNodes)
			{
				if (my >= vn.y && my <= vn.y + m_LineHeight)
				{
					const FileNode* node = vn.node;
					if (node->isFolder)
					{
						std::function<bool(FileNode&)> findAndToggle = [&](FileNode& n) -> bool
							{
							if (&n == node)
							{
								n.expanded = !n.expanded;
								if (n.expanded && n.children.empty())
								{
									PopulateNode(n, n.path);
								}
								return true;
							}
							for (auto& child : n.children)
							{
								if (findAndToggle(child)) return true;
							}
							return false;
							};
						findAndToggle(m_RootNode);
						return true;
					}
					else
					{
						if (m_FileOpenCallback)
						{
							m_FileOpenCallback(node->path);
						}
						return true;
					}
				}
			}
			return false;
			});

		dispatcher.Dispatch<MouseMovedEvent>([this](MouseMovedEvent& e)
			{
				if (m_IsDraggingScrollbar)
				{
					float contentY = GetY() + 30;
					float contentH = GetHeight() - 30;
					float maxScroll = std::max(0.0f, m_TotalHeight - contentH);
					float deltaY = e.GetY() - m_DragStartY;
					float ratio = deltaY / contentH;
					m_ScrollY = m_DragStartScrollY + ratio * maxScroll;
					if (m_ScrollY < 0) m_ScrollY = 0;
					if (m_ScrollY > maxScroll) m_ScrollY = maxScroll;
					return true;
				}
			return false;
			});

		dispatcher.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& e)
			{
			if (e.GetMouseButton() == GLFW_MOUSE_BUTTON_LEFT)
			{
				m_IsDraggingScrollbar = false;
			}
			return false;
			});

		return false;
	}

	void FileExplorer::DrawContent()
	{
		uiWindow::DrawContent();

		float contentX = GetX() + 10;
		float contentY = GetY() + 40;
		float contentW = GetWidth() - 10;
		float contentH = GetHeight() - 40;

		Application& app = Application::Get();
		int winHeight = app.GetWindow().GetHeight();
		glEnable(GL_SCISSOR_TEST);
		glScissor((int)contentX, winHeight - (int)(contentY + contentH), (int)contentW, (int)contentH);

		m_TotalHeight = 0.0f;
		CalcTreeHeight(m_RootNode, 0, m_TotalHeight);

		float maxScroll = std::max(0.0f, m_TotalHeight - contentH);
		if (m_ScrollY > maxScroll) m_ScrollY = maxScroll;
		if (m_ScrollY < 0) m_ScrollY = 0;

		float y = contentY - m_ScrollY;
		DrawNode(m_RootNode, 0, y, contentX, contentW);

		if (m_TotalHeight > contentH)
		{
			float thumbH = contentH * (contentH / m_TotalHeight);
			float thumbY = contentY + (m_ScrollY / (m_TotalHeight - contentH)) * (contentH - thumbH);
			glColor4f(0.5f, 0.5f, 0.5f, 0.8f);
			glBegin(GL_QUADS);
			glVertex2f(contentX + contentW - 8, thumbY);
			glVertex2f(contentX + contentW - 2, thumbY);
			glVertex2f(contentX + contentW - 2, thumbY + thumbH);
			glVertex2f(contentX + contentW - 8, thumbY + thumbH);
			glEnd();
		}

		glDisable(GL_SCISSOR_TEST);
	}

	void FileExplorer::CalcTreeHeight(const FileNode& node, int depth, float& total)
	{
		total += m_LineHeight;
		if (node.isFolder && node.expanded)
		{
			for (const auto& child : node.children)
			{
				CalcTreeHeight(child, depth + 1, total);
			}
		}
	}

	void FileExplorer::DrawNode(const FileNode& node, int depth, float& y, float x, float width)
	{
		float indent = depth * 16.0f;
		float iconSize = 16.0f;
		float textX = x + indent + iconSize + 4;

		if (y + m_LineHeight < GetY() + 30 || y > GetY() + GetHeight())
		{
			y += m_LineHeight;
			if (node.isFolder && node.expanded)
			{
				for (const auto& child : node.children)
				{
					DrawNode(child, depth + 1, y, x, width);
				}
			}
			return;
		}

		float iconX = x + indent;
		float iconY = y + (m_LineHeight - iconSize) / 2;
		if (node.isFolder)
		{
			DrawFolderIcon(iconX, iconY, node.expanded);
		}
		else
		{
			DrawFileIcon(iconX, iconY);
		}

		if (TextRenderer::Get().IsInitialized())
		{
			TextRenderer::Get().DrawText(node.name, textX, y + 2, 0.9f, 0.9f, 0.9f, 1.0f);
		}
		else
		{
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glBegin(GL_QUADS);
			glVertex2f(textX, y);
			glVertex2f(textX + 50, y);
			glVertex2f(textX + 50, y + m_LineHeight);
			glVertex2f(textX, y + m_LineHeight);
			glEnd();
		}

		y += m_LineHeight;

		if (node.isFolder && node.expanded)
		{
			for (const auto& child : node.children)
			{
				DrawNode(child, depth + 1, y, x, width);
			}
		}
	}

	void FileExplorer::DrawFolderIcon(float x, float y, bool expanded) const
	{
		glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
		if (expanded)
		{
			glBegin(GL_TRIANGLES);
			glVertex2f(x, y);
			glVertex2f(x + 12, y);
			glVertex2f(x + 6, y + 10);
			glEnd();
		}
		else
		{
			glBegin(GL_TRIANGLES);
			glVertex2f(x, y);
			glVertex2f(x, y + 10);
			glVertex2f(x + 10, y + 5);
			glEnd();
		}
	}

	void FileExplorer::DrawFileIcon(float x, float y) const
	{
		glColor4f(0.6f, 0.6f, 0.6f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(x, y);
		glVertex2f(x + 10, y);
		glVertex2f(x + 10, y + 10);
		glVertex2f(x, y + 10);
		glEnd();
	}

	void FileExplorer::BuildVisibleList(const FileNode& node, int depth, float startY, float& yOffset) const
	{
		float screenY = startY + yOffset - m_ScrollY;
		m_VisibleNodes.push_back({ &node, depth, screenY });
		yOffset += m_LineHeight;
		if (node.isFolder && node.expanded)
		{
			for (const auto& child : node.children)
			{
				BuildVisibleList(child, depth + 1, startY, yOffset);
			}
		}
	}

}
