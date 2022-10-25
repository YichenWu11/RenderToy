# Params Pass

* 每个 IComponent 有一个 `ComPack`, 其中包含该 IComponent 中所含的所有 IPass 的 `PassPack` 所需的成员
* 每个 IPass 有一个 `PassPack`, 其中包含了执行其更新所需的所有成员
  * > IPass 分为 RenderPass 和 LogicalPass
  * > 每个 IProperty 有一个其对应的 LogicalPass 用来更新所有物体的 IProperty (如果需要更新的话)
* 公共的资源都放在一个单例的 Mngr 中，全局可访问

## ***Pack传递流程***

RenderToy 有 `FillLogicalPack()` 和 `FillRenderPack()`, 通过这两个方法来填充 IComponent 的 `ComPack`
<br/>
在 IComponent 中有 `FillPassPack()` ,在这个方法中手动填充所有 IPass 的 `PassPack()`
