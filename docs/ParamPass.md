# Params Pass

* 暂时计划如果有一个类的执行需要一系列参数，就把这一系列参数打包成一个 `struct Pack`, 在 RenderToy 主类中为其调用 `FillPack()` 方法
* 公共的资源都放在一个单例的 Mngr 中，全局可访问