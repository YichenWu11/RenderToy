# Material Manager

目前的方案是，有两个 `struct`:
```cpp
    struct BasicMaterial
    {
        std::string Name;             // Unique material name for lookup.
        int MatIndex = -1;          // Index into constant buffer corresponding to this material.
        int DiffuseSrvHeapIndex = -1; // Index into SRV heap for diffuse texture.
        int NormalSrvHeapIndex = -1;  // Index into SRV heap for normal texture.
        DirectX::XMFLOAT4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };
        float Roughness = 0.25f;
        DirectX::XMFLOAT4X4 MatTransform{Math::MathHelper::Identity4x4()};
    };

    // structed buffer content (will be passed to shader)
    struct BasicMaterialData
    {
        DirectX::XMFLOAT4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };
        DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };
        float Roughness = 0.5f;
        DirectX::XMFLOAT4X4 MatTransform{Math::MathHelper::Identity4x4()};
        UINT DiffuseMapIndex = 0;
        UINT NormalMapIndex = 0;
        /* data for alignment */
        UINT MaterialPad1;
        UINT MaterialPad2;
    };
```
* BasicMaterial 是 MaterialMngr中实际存储的类型, BasicMaterialData 是填充 material 结构化缓冲区的类型。为所有的 material 创建一个大的结构化缓冲区，然后根据所有 BasicMaterial 来填充对应的 BasicMaterialData, 然后填充结构化缓冲区.
* BasicMaterial 中的 `MatIndex` 即就是该 material 在整个结构化缓冲区中的索引

物体的 material 属性内的 impl 如下:
```cpp
    struct Impl
    {
        BasicMaterial* material;  // 内含 matIndex
    };
```
* 每个物体通过自己的 matIndex 在 hlsl 中取到自己的 material 数据.
