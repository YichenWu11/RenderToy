# Objects Manager

* 每个物体都继承自最原始的基类 IObject，派生出的 Object 可以拥有不同的属性(组件), 比如 Transform, Material 等
* 每个物体有一个 InstanceID (GUID) 来做唯一标识
* 每个 Pass 含有多个物体，即就是在此 Pass 中要处理(绘制)的物体
* 每个 Object 在创建时用一个 shared_ptr 保存，每个 Pass 中存的物体用 weak_ptr 引用
* *原始的Objects暂时放在 RenderComponent 中*