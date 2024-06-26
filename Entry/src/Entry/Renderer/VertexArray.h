#pragma once

#include <memory>
#include "Buffer.h"

namespace Entry
{
    class VertexArray {
    public:
        virtual ~VertexArray() {}

        virtual void Bind() = 0;
        virtual void Unbind() const = 0;

        virtual const void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
        virtual const void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

        static Ref<VertexArray> Create();
    };
}