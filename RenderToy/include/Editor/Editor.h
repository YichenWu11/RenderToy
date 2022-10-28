#pragma once

namespace Chen::RToy::Editor {
    class Editor
    {
    public:
        static Editor& GetInstance()
        {
            static Editor instance;
            return instance;
        }

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        void Init()
        {
            
        }

    private:
        Editor() = default;
        ~Editor();
    };
}
