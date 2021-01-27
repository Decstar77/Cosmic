#include "FileParsers.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/std_image.h"

namespace cm
{
	XMLElement* XMLElement::AddChild(const String &tag_name)
	{
		children.emplace_back(tag_name);

		XMLElement *el = &children.back();
		el->parent = this;

		return el;
	}

	XMLElement* XMLElement::AddChild(const String &tag_name, const String &data)
	{
		children.emplace_back(tag_name, data);

		XMLElement *el = &children.back();
		el->parent = this;

		return el;
	}

	cm::XMLElement* XMLElement::AddChild(const XMLElement &element)
	{
		children.push_back(element);

		XMLElement *el = &children.back();
		el->parent = this;

		return el;
	}

	XMLAttribute* XMLElement::AddAttribute(const String &name, const String &data)
	{
		attributes.emplace_back(name, data);

		XMLAttribute *att = &attributes.back();

		return att;
	}

	String XMLTree::DoToString(const XMLElement &el)
	{
		StringStream ss;

		if (el.data.length() > 0)
		{
			ss << "<" << el.tag_name;
			for (const XMLAttribute &att : el.attributes)
			{
				ss << " " << att.name << "='" << att.data << "'";
			}
			ss << ">";
			ss << el.data;
		}
		else
		{
			ss << "<" << el.tag_name;

			for (const XMLAttribute &att : el.attributes)
			{
				ss << " " << att.name << "=\"" << att.data << "\"";
			}

			ss << ">" << std::endl;
		}

		for (const XMLElement &child : el.children)
		{
			ss << DoToString(child);
		}

		ss << "</" << el.tag_name << ">" << std::endl;

		return ss.str();
	}

	String XMLTree::ToString()
	{
		StringStream ss;
		ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;

		ss << "<" << root.tag_name << ">" << std::endl;

		for (const XMLElement &el : root.children)
		{
			ss << DoToString(el);
		}

		ss << "</" << root.tag_name << ">" << std::endl;

		return ss.str();
	}

	XMLTree::XMLTree(StringStream ss)
	{
		ss.clear();
		ss.seekg(0);

		String header;
		std::getline(ss, header);

		String line;
		std::stack<XMLElement> elements;
		while (std::getline(ss, line))
		{
			//line.erase(std::remove_if(line.begin(), line.end(), std::isspace), line.end());
			bool opening = false;
			bool closing = false;
			String opening_tag;
			String closing_tag;
			XMLElement element;

			int32 opening_index = static_cast<int32>(line.find_first_of('<'));
			if (opening_index != std::string::npos)
			{
				int32 closing_index = static_cast<int32>(line.find_first_of('>'));

				opening_tag = line.substr(opening_index + 1, closing_index - opening_index - 1);
				if (opening_tag.at(0) != '/')
				{
					opening = true;
				}
			}

			opening_index = static_cast<int32>(line.find_first_of('/'));
			if (opening_index != std::string::npos)
			{
				int32 closing_index = static_cast<int32>(line.find_last_of('>'));

				closing_tag = line.substr(opening_index + 1, closing_index - opening_index - 1);
				closing = true;
			}

			std::vector<String> attribs;
			for (int32 i = 0; i < opening_tag.length(); i++)
			{
				char c = opening_tag.at(i);
				if (c == ' ')
				{
					String attrib;

					i++;
					bool first_tag = false;
					for (;; i++)
					{
						c = opening_tag.at(i);
						if (c == '\"')
						{
							if (first_tag)
							{
								break;
							}
							else
							{
								first_tag = true;
								continue;
							}
						}
						else
						{
							attrib.push_back(c);
						}
					}
					attribs.push_back(attrib);
				}
				else
				{
					element.tag_name.push_back(c);
				}
			}

			for (String &attrib : attribs)
			{
				int32 eq = static_cast<int32>(attrib.find_first_of('='));
				element.AddAttribute(attrib.substr(0, eq), attrib.substr(eq + 1));
			}

			if (opening && !closing)
			{
				elements.push(element);
			}
			else if (!opening && closing)
			{
				if (elements.size() == 1)
				{
					this->root = elements.top();
					elements.pop();
				}
				else
				{
					XMLElement el = elements.top();
					elements.pop();
					elements.top().AddChild(el);
				}
			}
			else if (opening && closing)
			{
				int32 start = static_cast<int32>(line.find_first_of('>'));
				int32 end = static_cast<int32>(line.find_last_of('<'));

				element.data = line.substr(start + 1, end - start - 1);

				elements.top().AddChild(element);
			}
			else if (!opening && !closing)
			{

			}
		}
	}

	/*
	EditableMesh OBJParseModel(FileResult file_result, const String &file_name, const real32 &scale)
	{
		StringStream file;
		file << (const char *)file_result.data.data();


		int32 vertex_count = 0;
		int32 tcoords_count = 0;
		int32 normal_count = 0;
		int32 faces_count = 0;
		int32 face_vertex_count = 3;


		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream in(line);
			std::string type;
			in >> type;
			if (type == "v")
			{
				vertex_count++;
			}
			else if (type == "vt")
			{
				tcoords_count++;
			}
			else if (type == "vn")
			{
				normal_count++;
			}
			else if (type == "f")
			{
				faces_count += face_vertex_count;
			}
		}

		file.clear();
		file.seekg(0);

		int32 vertex_next = 0;
		Vec3f *vertices = (Vec3f *)malloc(vertex_count * sizeof(Vec3f));
		int32 tcoords_next = 0;
		Vec2f *tcoords = (Vec2f *)malloc(tcoords_count * sizeof(Vec2f));
		int32 normals_next = 0;
		Vec3f *normals = (Vec3f *)malloc(normal_count * sizeof(Vec3f));
		int32 face_next = 0;
		VertexIndex *faces = (VertexIndex *)malloc(faces_count * sizeof(VertexIndex));


		while (std::getline(file, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "v")
			{
				in >> vertices[vertex_next].x >> vertices[vertex_next].y >> vertices[vertex_next].z;
				vertex_next++;
			}
			else if (type == "vt")
			{
				in >> tcoords[tcoords_next].x >> tcoords[tcoords_next].y;
				tcoords_next++;
			}
			else if (type == "vn")
			{
				in >> normals[normals_next].x >> normals[normals_next].y >> normals[normals_next].z;
				normals_next++;
			}
			else if (type == "f")
			{
				char delim;
				int32 repeat = face_vertex_count; // @NOTE: Assumes triangulated !!
				while (repeat != 0)
				{
					repeat--;
					in >> faces[face_next].position_index >> delim >>
						faces[face_next].tcoord_index >> delim >>
						faces[face_next].normal_index;
					faces[face_next].other_index = -1;
					// @NOTE: OBJ files start at 1 or remove it
					faces[face_next].position_index -= 1;
					faces[face_next].normal_index -= 1;
					faces[face_next].tcoord_index -= 1;

					face_next++;
				}
			}
		}

		// @NOTE: Baked Scaling
		Mat3f scaling_matrix = ScaleCardinal(Mat3f(), Vec3f(scale));

		for (uint32 i = 0; i < (uint32)vertex_count; i++)
		{
			vertices[i] = vertices[i] * scaling_matrix;
		}

		EditableMesh mesh;

		String name = file_name;
		int32 name_pos = (int32)file_name.find_last_of('/');

		if (name_pos != std::string::npos)
		{
			name = file_name.substr(name_pos + 1);
			int32 ext_pos = (int32)name.find_first_of(".");
			if (ext_pos != std::string::npos)
			{
				name = name.substr(0, ext_pos);
			}
		}

		mesh.name = name;
		mesh.scale = scale;

		mesh.positions_count = vertex_count;
		mesh.positions = vertices;

		mesh.tcoords_count = tcoords_count;
		mesh.tcoords = tcoords;

		mesh.normal_count = normal_count;
		mesh.normals = normals;

		mesh.vertices_count = faces_count;
		mesh.vertices_vertex_count = face_vertex_count;
		mesh.vertices = faces;

		return mesh;
	}
	*/

	ModelResult EditableMesh::ConvertToPNTFormat()
	{
		// @NOTE: Convert into readable GPU format
		int32 vertices_count = (int32)vertices.size();

		std::vector<uint32> index_data = std::vector<uint32>(vertices_count);
		for (uint32 i = 0; i < (uint32)vertices_count; i++)
		{
			VertexIndex *current_face = &vertices.at(i);

			if (current_face->other_index == -1)
			{
				index_data[i] = i;
				continue;
			}

			for (uint32 j = i + 1; j < (uint32)vertices_count; j++)
			{
				VertexIndex *other_face = &vertices[j];

				if (current_face->position_index == other_face->position_index &&
					current_face->normal_index == other_face->normal_index &&
					current_face->tcoord_index == other_face->tcoord_index)
				{
					index_data[j] = i;
					other_face->other_index = i;
				}
			}
		}

		std::vector<real32> vertex_data = std::vector<real32>(vertices_count * 3 * 3 * 2);
		for (uint32 i = 0, vertex_data_index = 0; i < (uint32)vertices_count; i++)
		{
			const VertexIndex &face = vertices[i];

			if (face.other_index == -1)
			{
				vertex_data[vertex_data_index++] = positions[face.position_index].x;
				vertex_data[vertex_data_index++] = positions[face.position_index].y;
				vertex_data[vertex_data_index++] = positions[face.position_index].z;

				vertex_data[vertex_data_index++] = normals[face.normal_index].x;
				vertex_data[vertex_data_index++] = normals[face.normal_index].y;
				vertex_data[vertex_data_index++] = normals[face.normal_index].z;

				vertex_data[vertex_data_index++] = tcoords[face.tcoord_index].x;
				vertex_data[vertex_data_index++] = tcoords[face.tcoord_index].y;
			}
		}

		ModelResult model = {};

		// model.file_name = file_name;
		// model.scale = scale;
		model.vertex_count = (uint32)positions.size();
		model.index_count = vertices_count;
		model.vertex_size = vertices_count * 3 * 3 * 2;
		model.vertices = vertex_data;
		model.indices = index_data;
		model.stride = sizeof(real32) * 3 + sizeof(real32) * 3 + sizeof(real32) * 2;

		return model;
	}

	MeshCollider EditableMesh::GetMeshCollider()
	{
		int32 vertices_count = (int32)vertices.size();

		if (vertices_count != 0)
		{
			if (triangles.size() == 0)
			{
				triangles.resize(vertices_count / 3);

				for (uint32 i = 0, tri_index = 0; i < (uint32)vertices_count; i += 3, tri_index++)
				{
					VertexIndex *vertex1 = &vertices[i];
					VertexIndex *vertex2 = &vertices[i + 1];
					VertexIndex *vertex3 = &vertices[i + 2];

					// @NOTE: Are we a duplicate of someone else ? If so find him with other_index
					if (vertex1->other_index != -1)
					{
						vertex1 = &vertices[vertex1->other_index];
					}

					if (vertex2->other_index != -1)
					{
						vertex2 = &vertices[vertex2->other_index];
					}

					if (vertex2->other_index != -1)
					{
						vertex2 = &vertices[vertex2->other_index];
					}

					Triangle tri;
					tri.v0 = positions[vertex1->position_index];
					tri.v1 = positions[vertex2->position_index];
					tri.v2 = positions[vertex3->position_index];

					triangles.at(tri_index) = tri;
				}
			}
		}

		return triangles;
	}

	bool EditableMesh::IsValid()
	{
		return static_cast<bool>(vertices.size());
	}

	EditableMesh::EditableMesh()
	{

	}

	EditableMesh::EditableMesh(const FileResult &file_result, const MeshMetaData &meta_data, const MeshFileType &file_type)
	{
		switch (file_type)
		{
		case MeshFileType::OBJ: ParseOBJ(file_result, meta_data); break;
		case MeshFileType::DAE:
			break;
		default:
			ASSERT(0, "Mesh file type not supported");
		}
	}


	EditableMesh::~EditableMesh()
	{

	}

	void EditableMesh::ParseOBJ(const FileResult &file_result, const MeshMetaData &meta_data)
	{
		StringStream file;
		file << (const char *)file_result.data.data();

		int32 vertex_next = 0;
		int32 tcoords_next = 0;
		int32 normals_next = 0;
		int32 face_next = 0;

		int32 vertex_count = 0;
		int32 tcoords_count = 0;
		int32 normal_count = 0;
		int32 faces_count = 0;
		int32 face_vertex_count = 3;

		std::string line;
		while (std::getline(file, line))
		{
			std::istringstream in(line);
			std::string type;
			in >> type;
			if (type == "v")
			{
				vertex_count++;
			}
			else if (type == "vt")
			{
				tcoords_count++;
			}
			else if (type == "vn")
			{
				normal_count++;
			}
			else if (type == "f")
			{
				faces_count += face_vertex_count;
			}
		}

		file.clear();
		file.seekg(0);

		positions.resize(vertex_count);
		normals.resize(normal_count);
		tcoords.resize(tcoords_count);
		vertices.resize(faces_count);

		while (std::getline(file, line))
		{
			std::istringstream in(line);

			std::string type;
			in >> type;

			if (type == "v")
			{
				in >> positions[vertex_next].x >> positions[vertex_next].y >> positions[vertex_next].z;
				vertex_next++;
			}
			else if (type == "vt")
			{
				in >> tcoords[tcoords_next].x >> tcoords[tcoords_next].y;
				tcoords_next++;
			}
			else if (type == "vn")
			{
				in >> normals[normals_next].x >> normals[normals_next].y >> normals[normals_next].z;
				normals_next++;
			}
			else if (type == "f")
			{
				char delim;
				int32 repeat = face_vertex_count; // @NOTE: Assumes triangulated !!
				while (repeat != 0)
				{
					repeat--;
					in >> vertices[face_next].position_index >> delim >>
						vertices[face_next].tcoord_index >> delim >>
						vertices[face_next].normal_index;
					vertices[face_next].other_index = -1;
					// @NOTE: OBJ files start at 1 or remove it
					vertices[face_next].position_index -= 1;
					vertices[face_next].normal_index -= 1;
					vertices[face_next].tcoord_index -= 1;

					face_next++;
				}
			}
		}

		// @NOTE: Baked Scaling
		Mat3f scaling_matrix = ScaleCardinal(Mat3f(), Vec3f(meta_data.base_scale));

		for (uint32 i = 0; i < (uint32)positions.size(); i++)
		{
			positions[i] = positions[i] * scaling_matrix;
		}

		String name = file_result.path;
		int32 name_pos = (int32)name.find_last_of('/');

		if (name_pos != std::string::npos)
		{
			name = name.substr(name_pos + 1);
			int32 ext_pos = (int32)name.find_first_of(".");
			if (ext_pos != std::string::npos)
			{
				name = name.substr(0, ext_pos);
			}
		}

		this->name = name;
	}

	EditableTexture::EditableTexture(const FileResult &file)
	{
		uint8 *data = stbi_load_from_memory((stbi_uc*)file.data.data(), (int32)file.data.size(), &width, &height, &channel_count, 4);
		ASSERT(data, "Cannot find image: " + file.path);

		channel_count = 4;

		uint32 size = static_cast<uint32>(width * height * channel_count);
		this->pixel_data.assign(data, data + size);
	}

	EditableTexture::~EditableTexture()
	{

	}
} // namespace cm