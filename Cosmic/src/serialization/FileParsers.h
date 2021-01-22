#ifndef __FILEPARSERS_H__
#define __FILEPARSERS_H__

#include "src/Cosmic.h"
#include "src/math/CosmicMath.h"
#include "src/math/CosmicGeometry.h"

namespace cm
{
	class XMLAttribute
	{
	public:
		String name;
		String data;

		inline void Set(const String &name, const String &data)
		{
			this->name = name;
			this->data = data;
		}

		XMLAttribute(const String &name, const String &data) : name(name), data(data) { }
	};

	class XMLElement
	{
	public:
		String tag_name;
		String data;

	public:
		XMLElement* AddChild(const String &tag_name);
		XMLElement* AddChild(const String &tag_name, const String &data);
		XMLElement* AddChild(const XMLElement &element);
		XMLAttribute *AddAttribute(const String &name, const String &data);

		inline std::list<XMLElement> GetChildren() { return children; }
		inline std::vector<XMLAttribute> GetAttributes() { return attributes; }

	public:
		XMLElement() {}
		XMLElement(const String &tag_name) : tag_name(tag_name) { }
		XMLElement(const String &tag_name, const String &data) : tag_name(tag_name), data(data) { }
		~XMLElement() {}

	private:
		XMLElement *parent;
		std::list<XMLElement> children;
		std::vector<XMLAttribute> attributes;

		friend class XMLTree;
	};

	class XMLTree
	{
	public:
		String name;

	public:
		inline XMLElement* GetRoot() { return &root; }
		String ToString();

	public:
		XMLTree(const String &name) : name(name) { root.tag_name = "Root"; }
		XMLTree(StringStream ss);
		~XMLTree() {}

	private:
		XMLElement root;
		int32 count;

	private:
		String DoToString(const XMLElement &el);
	};

	struct VertexIndex
	{
		int32 other_index; // @NOTE: The index into the vertices array that is an exact copy of this face
		int32 position_index;
		int32 tcoord_index;
		int32 normal_index;
	};

	struct ModelResult
	{
		uint32 stride = 0;       // @NOTE: The stride of the vertices, right now it's 3f * 3f * 2f
		real32 scale = 0.0f;     // @NOTE: Scale to bake the model
		uint32 vertex_count = 0; // @NOTE: The total individual vertices
		uint32 index_count = 0;  // @NOTE: The size of indices array;
		uint32 vertex_size = 0;  // @NOTE: The size of vertices array;
		real32 *vertices;
		uint32 *indices;
		String file_name;
	};

	void FreeModel(ModelResult *model);

	enum class MeshFileType
	{
		INVALID = 0,
		OBJ,
		DAE
	};

	struct MeshMetaData//MeshEntry
	{
		MeshEntry entry;
		String name;
		String path;
		real32 base_scale;
		int32 face_count;
		int32 position_count;
		int32 normal_count;
		int32 tcoords_count;
		AABB bounding_box;
	};

	class EditableMesh
	{
	public:
		String name;

		std::vector<Vec3f> positions;
		std::vector<Vec3f> normals;
		std::vector<Vec2f> tcoords;

		int32 vertices_vertex_count = 3;
		std::vector<VertexIndex> vertices;

		// @TODO: Should probably just keep the data like this and remove the above
		MeshCollider triangles;

		ModelResult ConvertToPNTFormat();
		MeshCollider GetMeshCollider();

		bool IsValid();

		EditableMesh();
		EditableMesh(const FileResult &file_result, const MeshMetaData &data, const MeshFileType &file_type);
		~EditableMesh();

	private:
		void ParseOBJ(const FileResult &file_result, const MeshMetaData &meta_data);

	};
	enum class TextureFileFormat
	{
		INVALID,
		PNG,
		JPG,
		BMP
	};

	class EditableTexture
	{
	public:
		int32 width;
		int32 height;
		int32 channel_count;
		TextureFileFormat file_format;

		String path;
		std::vector<uint8> pixel_data;
		//std::vector<uint8> srgb_data;

	public:
		EditableTexture(const FileResult &file);
		~EditableTexture();

	};

	class Serializer
	{

	};


} // namespace cm

#endif // __FILEPARSERS_H__