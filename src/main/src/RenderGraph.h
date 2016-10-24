#pragma once 


namespace ag 
{
	namespace rg 
	{
		enum class NodeType {
			// Shader nodes
			VertexShader,
			FragmentShader,
			TessControlShader,
			TessEvalShader,
			GeometryShader,
			ComputeShader,
			ScreenQuadPass,
			// Misc.
			Clear,
			MSAAResolve,
			RenderTarget
		};


		enum class ResourceType {
			Texture,
			Buffer
		};

		enum class PortType {
			Texture,
			Buffer,
			VertexStream,
			FragmentStream
		};

		class RenderGraph 
		{
		public:
			// forEachSuccessor(nodeID, portID)
			// forEachPredecessor(nodeID, portID)
			// removeNode(nodeID)
			// removePort(nodeID, portID)
			// addNode() -> nodeID
			// addOutput(nodeID, type, createResource=true)
			// addInput(nodeID, type)
			// addInputOutput(nodeID, type)
			// getTexture(id)
			// getBuffer(id)
			// getResourceType(id)
			// getNodeFromResource(Resource*)
			// serialize(BinaryStream&)
			// deserialize(BinaryStream&)

			struct Edge {
				int id;
				Node* fromNode;
				int fromPortId;
				Node* toNode;
				int toPortId;
			};

		private:
			std::unordered_map<int, std::unique_ptr<Resource>> resources;
			std::unordered_map<int, std::unique_ptr<Node>> nodes;
			std::vector<Edge> edges; 
			int nextResourceID;
			int nextNodeID;
		};

		class Resource {
		public:
			int getID() const { return id_; }

			// true, false or unspecified
			optional<bool> requiresExclusiveAccess;

		protected:
			int id_;
		}

		// A global constant for a rendergraph
		enum class ConstantType 
		{
			ShaderKeyword,
			FloatVecConst,
			IntVecConst
		};

		class Constant
		{
		public:
			struct ShaderKeyword {
				std::string keyword;
			};
			struct IntVec {
				int size;
				int v[4];
			};
			struct FloatVec {
				int size;
				float v[4];
			};

			std::string name;
			variant<std::string, float, int>;
		};

		// 
		// Texture resource descriptor
		class RTexture : public Resource 
		{
	    public:
	    	optional<ImageDimensions> dimensions;
	    	optional<ImageFormat> format;
	    	optional<Constant> width;
	    	optional<Constant> height;
	    	optional<Constant> depth;
		};

		// A connection between an output port and an input port
		// The types must be compatible
		
		class Node {
		public:
			class Port 
			{
			public:
				int id;
				std::string name;
				PortType type;
				int resourceID;
			};

		protected:
			int id;
			std::vector<Resource*> resources;
			std::unordered_map<int, Port> ports;
		};

		class ComputeShaderNode : public Node
		{
		public:
		private:
		};


		{
			ComputeShaderNode n;
			rg.addInput(n, "texIn", Texture);
			rg.addOutput(n, "texOut", Texture);	// will also create a resource entry: Texture(width: unspecified, height: unspecified, etc.)
			n.shaderCode = "<GLSL source>";
		}

		// 
		// outTexture = createTexture()
		// in = addPort(in, name, texture)
		// out = addPort(out, name, outTexture)
		// 
		// 

		// class ComputeShader
		// class GenericComputeShader : public ComputeShader
		// (other specialized compute shaders)
		// 

		// class Resource
		// +type (Texture, Buffer, etc.)
		// +???
		// requiresExclusiveAccess: bool
		// owned by (xxx)

		// class Edge
		// from: (nodeID, portID)
		// to: (nodeID, portID)
		// resource: resourceID (optional)

		// class FragmentStream: public Edge
		// list of attributes (type,name)

		// class VertexStream: public Edge
		// list of attributes (type,name)

		// class Node
		// +internal resources: vector<ResourceID> (e.g. for output textures)
		// virtual void onConnect(nodeID, edgeID, portID, out nodesToUpdate)
		// => check for incompatible input
		// => apply rules (e.g. set output descriptors)
		// => update internal resources
		// => update dependencies of changed nodes
		//
		// Example of rule: 
		//     ensureSameResource(outputTexture, input)
		//     ensure(outputTexture.format, xxx, "message")
		//     ensure(outputTexture.requiresExclusiveAccess, true, "message")
		//     ensureSameFormat(output, input, "message")

		// class Texture
		// => width, height, depth: optional<int> (can be unspecified)
		// => format: optional<ImageFormat>
		// => dimensions: optional<ImageDimensions>
		// at compilation time, all fields must be specified

		//
		// In-place modifications
		// Option 1: In and out ports, must specify that it is the same resource for in and out
		// Option 2: Output texture : node parameter, no conflict checking

		//
		// RenderTarget node
		// (in) FragmentStream -> (out) Texture
		// optionally: (in) Texture
		// 
	}
}