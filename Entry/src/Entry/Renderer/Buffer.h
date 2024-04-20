#pragma once

namespace Entry
{
    class VertexBuffer {
    public:
        virtual ~VertexBuffer() {}


        virtual void Bind() = 0;
        virtual void Unbind() const = 0;

        static VertexBuffer* Create(float* vertices, uint32_t size);
    };

    class IndexBuffer {
    public:
        virtual ~IndexBuffer() {}

        virtual void Bind() = 0;
        virtual void Unbind() const = 0;

        virtual uint16_t GetCount() const = 0;
        virtual void* GetDataPointer() const = 0;

        static IndexBuffer* Create(uint16_t* indices, uint16_t count);
    };
}
