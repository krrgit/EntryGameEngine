#pragma once

namespace Entry
{
    enum class ShaderDataType : uint8_t
    {
        None = 0, Float,Float2,Float3,Float4,Mat3,Mat4,Int,Int2,Int3,Int4,Bool
    };

    static uint32_t ShaderDataTypeSize(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float:     return 4;
        case ShaderDataType::Float2:    return 4 * 2;
        case ShaderDataType::Float3:    return 4 * 3;
        case ShaderDataType::Float4:    return 4 * 4;
        case ShaderDataType::Mat3:      return 4 * 3 * 3;
        case ShaderDataType::Mat4:      return 4 * 4 * 4;
        case ShaderDataType::Int:       return 4;
        case ShaderDataType::Int2:      return 4 * 2;
        case ShaderDataType::Int3:      return 4 * 3;
        case ShaderDataType::Int4:      return 4 * 4;
        case ShaderDataType::Bool:      return 1;
        default:
            break;
        }

        ET_CORE_ASSERT(false, "Unknown ShaderDataType!");
        return 0;
    }

    struct BufferElement
    {
        ShaderDataType Type;
        std::string Name;
        uint32_t Size;
        uint32_t Offset; // may not be needed
        //bool Normalized; // may not be needed

        BufferElement() {}

        BufferElement(ShaderDataType type, const std::string& name)
            : Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0) //, Normalized(normalized)
        {}

        uint32_t GetComponentCount() const
        {
            switch (Type)
            {
            case ShaderDataType::Float:     return 1;
            case ShaderDataType::Float2:    return 2;
            case ShaderDataType::Float3:    return 3;
            case ShaderDataType::Float4:    return 4;
            case ShaderDataType::Mat3:      return 3 * 3;
            case ShaderDataType::Mat4:      return 4 * 4;
            case ShaderDataType::Int:       return 1;
            case ShaderDataType::Int2:      return 2;
            case ShaderDataType::Int3:      return 3;
            case ShaderDataType::Int4:      return 4;
            case ShaderDataType::Bool:      return 1;
            default:
                break;
            }

            ET_CORE_ERROR("Unknown ShaderDataType!");
            return 0;
        }
    };
    class BufferLayout {
    public:
        BufferLayout() {}
        BufferLayout(const std::initializer_list<BufferElement>& elements)
        : m_Elements(elements)
        {
           CalculateOffsetsAndStride();
           CalculatePermutation();
        }

        inline uint32_t GetStride() const { return m_Stride; }
        inline uint32_t GetAttribCount() const { return m_attribCount; }
        inline uint32_t GetPermutation() const { return m_permutation; }
        inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
    
        std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
        std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
        std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
        std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

        void DebugPrint() const {
            for (auto& element : m_Elements)
            {
                ET_CORE_INFO("Name:{0}\n Type:{1}\n Size:{2}\n Offset:{3}\n\n",
                    element.Name.data(), (unsigned int)element.Type,
                    (unsigned int)element.Size, (unsigned int)element.Offset);
            }

        }

    private:
        void CalculateOffsetsAndStride()
        {
            //ET_CORE_INFO("BufferLayout:{0} elements", m_Elements.size());
            //uint32_t offset = 0;
            m_Stride = 0;
            m_attribCount = 0;
            for (auto& element : m_Elements)
            {
                //element.Offset = offset;
                //offset += element.Size; // Causes crash??
                m_Stride += element.Size;
                m_attribCount++;
            }
        }

        void CalculatePermutation() {
            m_permutation = 0;
            if (m_attribCount == 1)
                return;
            else {
                // Calculate the value for the current iteration
                for (uint64_t i = 0; i <= m_attribCount; ++i) {
                    m_permutation |= (i << (i * 4));
                }
            }
        }
    private:
        std::vector<BufferElement> m_Elements;
        uint32_t m_Stride = 0;
        uint32_t m_attribCount = 0;
        uint64_t m_permutation = 0;
    };


    class VertexBuffer {
    public:
        virtual ~VertexBuffer() {}


        virtual void Bind() = 0;
        virtual void Unbind() const = 0;

        virtual const BufferLayout& GetLayout() const = 0;
        virtual void SetLayout(const BufferLayout layout) = 0;

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
