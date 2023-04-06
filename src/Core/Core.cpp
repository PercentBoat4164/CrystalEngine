#include "Core.hpp"

IE::Core::Logger IE::Core::Core::m_logger{ILLUMINATION_ENGINE_CORE_LOGGER_NAME};
std::mutex       IE::Core::Core::m_enginesMutex{};
std::unordered_map<std::string, std::shared_ptr<IE::Core::Engine>> IE::Core::Core::m_engines{};
std::mutex                                                         IE::Core::Core::m_windowsMutex{};
std::unordered_map<GLFWwindow *, IE::Core::Window>                 IE::Core::Core::m_windows{};
IE::Core::Threading::ThreadPool *const IE::Core::Core::m_threadPool{new Threading::ThreadPool{}};
IE::Core::FileSystem *const            IE::Core::Core::m_filesystem{new FileSystem{}};

IE::Core::Core &IE::Core::Core::getInst(const std::filesystem::path &t_path) {
    static IE::Core::Core inst{t_path};
    return inst;
}

IE::Core::Logger *IE::Core::Core::getLogger() {
    return &m_logger;
}

IE::Core::FileSystem *IE::Core::Core::getFileSystem() {
    return m_filesystem;
}

IE::Core::Threading::ThreadPool *IE::Core::Core::getThreadPool() {
    return m_threadPool;
}

IE::Core::Window *IE::Core::Core::getWindow(GLFWwindow *t_window) {
    std::lock_guard<std::mutex> lock(m_windowsMutex);
    auto                        window = m_windows.find(t_window);
    if (window != m_windows.end()) return &window->second;
    else
        m_logger.log(
          "Window '" + std::to_string((uint64_t) t_window) + "' does not exist!",
          IE::Core::Logger::ILLUMINATION_ENGINE_LOG_LEVEL_ERROR
        );
    return nullptr;
}
