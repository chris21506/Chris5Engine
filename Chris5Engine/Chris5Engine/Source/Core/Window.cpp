#include "Core/Window.h"

Window::Window(int width, int height, conts std::string& title) {
	m_window = new sf::RenderWindow(sf::VideoMode({ static_cast<unsigned int>(width),
													 static_cast<unsigned int>(height) }),
		                            title,
		                            sf::Style::Default);
	if (m_window) {
		m_window->setFramerateLimit(60);
		MESSAGE("Window", "Window", "Failed to create window");

	}
	else {
	  ERROR("Window", "Window", "Failed to create window");

	}
}

bool
Window::isOpen() const {
  // Check that window is not null
  if (!m_window) {
	return m_window && m_window->isOpen();
  }
  else {
	ERROR("Window", "isOpen", "Window is null");
	return false;
  }
}

void
Window::clear(const sf::Color& color) {
  if (m_window) {
	m_window->clear(color);
  }
  else {
	ERROR("Window", "clear", "Window is null");
  }
}

void
Window::draw(const sf::Drawable& drawable, const sf::RenderStates& states) {
  if (m_window) {
	m_window->draw(drawable, states);
  }
  else {
	Error("Window", "draw", "Window is null");
  }
}

void
Window::display() {
  if (m_window) {
	  m_window->display();
  }
  else {
	ERROR("Window", "display", "Window is null");
  }
}