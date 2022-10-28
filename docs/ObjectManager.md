# Objects Manager

* 每个物体都继承自最原始的基类 IObject，派生出的 Object 可以拥有不同的属性(IProperty), 比如 Transform, Material 等
* 每个物体有一个 InstanceID (GUID) 来做唯一标识
* 每个 Pass 含有多个物体，即就是在此 Pass 中要处理(绘制)的物体
* 每个 Object 在创建时用一个 shared_ptr 保存，每个 Pass 中存物体的原生指针，只负责用
* 每个 Impl in IProperty 都是对齐(如果需要)的，可以直接申请给帧资源
* *<font color=lightyellow>所有的 Object 统一用一个全局的 ObjectMngr 来管理</font>*

---

## Design

IObject 中存有一个 name2IProperty 的 map，而每个 IProperty 都有一个内置的 `struct Impl`, 其中包含了该属性特有的一些变量，比如 transform 属性会有其的 translate, shear, scale 等变换矩阵，material 属性会有 albedo, shiness 等属性。每个 IProerty 通过返回 std::any 来在类成员上实现多态, 如下:
```cpp
    struct Impl
    {
        int id{1};
    };

    Impl impl;

    std::any GetImpl() override { return impl; };
```

在 IObject 中有成员
```cpp
    std::unordered_map<std::string, IProperty*> mProperties;
```

有方法如下
```cpp
        void AddProperty(std::string name, IProperty* property)
        {
            if (mProperties.find(name) != mProperties.end()) return;
                mProperties[name] = property;
        }

        void DelProperty(std::string name)
        {
            if (mProperties.find(name) != mProperties.end()) mProperties.erase(mProperties.find(name));
        }

        template<typename T>
        typename T::Impl GetPropertyImpl(std::string name)
        {
            assert(mProperties.find(name) != mProperties.end());
            auto impl = mProperties.find(name)->second->GetImpl();
            return std::any_cast<typename T::Impl>(impl);
        }
```
