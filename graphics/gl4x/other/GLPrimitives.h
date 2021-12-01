#ifndef	GL_PRIMITIVES_H
#define	GL_PRIMITIVES_H

#include	<oreore/common/Utility.h>

#include	"../resource/GLVertexArrayObject.h"



namespace OreOreLib
{

	static void CreateScreenSpaceQuad( GLVertexArrayObject& ssquad )
	{
		const VertexLayout_2d quadmesh[] =
		{
			{ { -1.0, -1.0 }, { 0.0, 0.0 } },
			{ { -1.0, +1.0 }, { 0.0, +1.0 } },
			{ { +1.0, +1.0 }, { +1.0, +1.0 } },
			{ { +1.0, -1.0 }, { +1.0, 0.0 } },
		};

		const uint8 quad_indices[] =
		{
			0, 1, 2, 
			3, 0, 2,
		};

		const InputElementDesc layout[] =
		{
			{ ATTRIB_LOCATION_POSITION, DATA_FORMAT::FORMAT_R32G32_FLOAT, false, sizeof(VertexLayout_2d), 0 },	// Position
			{ ATTRIB_LOCATION_TEXCOORD, DATA_FORMAT::FORMAT_R32G32_FLOAT, false, sizeof(VertexLayout_2d), 8 },	// Texcoord
		};


		ssquad.Init();
		ssquad.SetVertexAttributes( layout, ArraySize<int>(layout), PRIMITIVE_TYPE::PRIM_TRIANGLE );
		ssquad.SetVertexBuffers( quadmesh, sizeof(quadmesh), GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		ssquad.SetIndexBuffer( quad_indices, ArraySize<int>(quad_indices), DATA_FORMAT::FORMAT_R8_UINT, GL_STATIC_DRAW );
		ssquad.Update();
	}



	static void CreateOverlappedScreenSpaceQuad( GLVertexArrayObject& ssquad )
	{
		const VertexLayout_2d quadmesh[] =
		{
			{ { -1.0, -1.0 }, { 0.0, 0.0 } },
			{ { -1.0, +1.0 }, { 0.0, +1.0 } },
			{ { +1.0, +1.0 }, { +1.0, +1.0 } },
			{ { +1.0, -1.0 }, { +1.0, 0.0 } },
		};

		const uint8 quad_indices[] =
		{
			0, 1, 2,
			0, 1, 2,
			0, 1, 2,
			0, 1, 2,
		};

		const InputElementDesc layout[] =
		{
			{ ATTRIB_LOCATION_POSITION, DATA_FORMAT::FORMAT_R32G32_FLOAT, false, sizeof( VertexLayout_2d ), 0 },	// Position
			{ ATTRIB_LOCATION_TEXCOORD, DATA_FORMAT::FORMAT_R32G32_FLOAT, false, sizeof( VertexLayout_2d ), 8 },	// Texcoord
		};


		ssquad.Init();
		ssquad.SetVertexAttributes( layout, ArraySize<int>( layout ), PRIMITIVE_TYPE::PRIM_TRIANGLE );
		ssquad.SetVertexBuffers( quadmesh, sizeof( quadmesh ), GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		ssquad.SetIndexBuffer( quad_indices, ArraySize<int>( quad_indices ), DATA_FORMAT::FORMAT_R8_UINT, GL_STATIC_DRAW );
		ssquad.Update();

	}





	static void CreateUnitCube( GLVertexArrayObject& vao )
	{
		const VertexLayoutSolid cubemesh[] =
		{
			{ { -1.0f, -1.0f, -1.0f }, { -0.57735f, -0.57735f, -0.57735f }, { 0.0f, 0.0f, 0.0f } },//0: -x-y-z
			{ { -1.0f, -1.0f, +1.0f }, { -0.57735f, -0.57735f, +0.57735f }, { 0.0f, 0.0f, 1.0f } },//1: -x-y+z
			{ { -1.0f, +1.0f, -1.0f }, { -0.57735f, +0.57735f, -0.57735f }, { 0.0f, 1.0f, 0.0f } },//2: -x+y-z
			{ { -1.0f, +1.0f, +1.0f }, { -0.57735f, +0.57735f, +0.57735f }, { 0.0f, 1.0f, 1.0f } },//3: -x+y+z
			{ { +1.0f, -1.0f, -1.0f }, { +0.57735f, -0.57735f, -0.57735f }, { 1.0f, 0.0f, 0.0f } },//4: +x-y-z
			{ { +1.0f, -1.0f, +1.0f }, { +0.57735f, -0.57735f, +0.57735f }, { 1.0f, 0.0f, 1.0f } },//5: +x-y+z
			{ { +1.0f, +1.0f, -1.0f }, { +0.57735f, +0.57735f, -0.57735f }, { 1.0f, 1.0f, 0.0f } },//6: +x+y-z
			{ { +1.0f, +1.0f, +1.0f }, { +0.57735f, +0.57735f, +0.57735f }, { 1.0f, 1.0f, 1.0f } },//7: +x+y+z
		};

		const uint8 cube_indices[] =
		{
			0, 1, 2, 3, 2, 1,
			1, 5, 3, 7, 3, 5,
			5, 4, 7, 6, 7, 4,
			4, 0, 6, 2, 6, 0,
			2, 3, 6, 7, 6, 3,
			1, 0, 5, 4, 5, 0,
		};

		const InputElementDesc layout[] =
		{
			{ ATTRIB_LOCATION_POSITION, DATA_FORMAT::FORMAT_R32G32B32_FLOAT, false, sizeof( VertexLayoutSolid ), 0 },	// Position
			{ ATTRIB_LOCATION_NORMAL, DATA_FORMAT::FORMAT_R32G32B32_FLOAT, false, sizeof( VertexLayoutSolid ), 12 },	// Normal
			{ ATTRIB_LOCATION_TEXCOORD, DATA_FORMAT::FORMAT_R32G32B32_FLOAT, false, sizeof( VertexLayoutSolid ), 24 },	// Texcoord
		};


		vao.Init();
		vao.SetVertexAttributes( layout, ArraySize<int>( layout ), PRIMITIVE_TYPE::PRIM_TRIANGLE );
		vao.SetVertexBuffers( cubemesh, sizeof( cubemesh ), GL_ARRAY_BUFFER, GL_STATIC_DRAW );
		vao.SetIndexBuffer( cube_indices, ArraySize<int>( cube_indices ), DATA_FORMAT::FORMAT_R8_UINT, GL_STATIC_DRAW );
		vao.Update();

	}
	
}



#endif	// GL_PRIMITIVES_H //