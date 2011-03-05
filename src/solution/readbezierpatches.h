#ifndef READBEZIERPATCHES_H
#define READBEZIERPATCHES_H

/*******************************************************************\
*                                                                   *
*                 R e a d B e z i e r P a t c h e s                 *
*                                                                   *
\*******************************************************************/

#include <fstream>
#include <vector>

#include <cstdio>
#include <cstring>

namespace graphics {

    int ReadBezierPatches(const char* filename,
		     std::vector<graphics::MyMathTypes::bezier_patch>& BezierPatches)
    {
	//std::cout << "-->ReadBezierPatches(...)" << std::endl;

	const int NVERTEX        = 0;
	const int READ_VERTICES  = 1;
	const int PATCHNAME      = 2;
	const int SEARCH_PATCHES = 3;
	const int READ_PATCHES   = 4;

	char ch;
	const int MAX_BUFFER = 256;
	char  InputBuffer[MAX_BUFFER];
	
	int    NumberOfVertices;
	int    VertexNumber;
	double x;
	double y;
	double z;

	char   PatchName[MAX_BUFFER];
	int    PatchNumber;
	int    index_11, index_12, index_13, index_14;
	int    index_21, index_22, index_23, index_24;
	int    index_31, index_32, index_33, index_34;
	int    index_41, index_42, index_43, index_44;

	// The vertex list
	std::vector<MyMathTypes::vector3_type> Vertices;


	std::ifstream data_file(filename);
	if (!data_file) {
	    std::cerr << "Cannot open data file: " << filename << std::endl << std::flush;
	    return -1;
	}
    
	int state = NVERTEX;
	while (data_file.get(InputBuffer, MAX_BUFFER, '\n')) {
	    if (data_file.get(ch) && ch != '\n') {
		data_file.close();
		std::cerr << "Eof on data file: " << filename << std::endl << std::flush;
		return -1;
	    }

	    // Now one line of data is in InputBuffer
	    // Use sscanf to extract the numeric values

	    // Build a data structure which represents the different
	    // vertices and patches
	    //
	    // There are several possibilities for the input lines:
	    //
	    // 1: a comment line, i.e. InputBuffer[0] == '#'
	    // 2: a 'number of vertices line', i.e. just one number
	    //    sscanf(InputBuffer, "%d", &NumberOfVertices)
	    // 3: a 'vertex line', i.e. 4 numbers
	    //    sscanf(InputBuffer, "%d%lf%lf%lf", &VertexNumber, &x, &y, &z)
	    // 4: a 'patch line', i.e. 17 numbers
	    //    sscanf(InputBuffer,
	    //           "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
	    //           &PatchNumber,
	    //           &index_11, &index_12, &index_13, &index_14,
	    //           &index_21, &index_22, &index_23, &index_24,
	    //           &index_31, &index_32, &index_33, &index_34,
	    //           &index_41, &index_42, &index_43, &index_44)

	    switch (state) {
	    case NVERTEX:
		if (InputBuffer[0] != '#') {
		    if (sscanf(InputBuffer, "%d", &NumberOfVertices) != 1) {
			std::cerr << "number of vertices not found in data file: "
				  << filename << std::endl << std::flush;
			return -1;
		    }

		    //std::cout << "Number of Vertices = " << NumberOfVertices
		    //	      << std::endl << std::endl;

		    state = READ_VERTICES;
		}
		break;
	    case READ_VERTICES:
		if (InputBuffer[0] != '#') {
		    if (sscanf(InputBuffer, "%d%lf%lf%lf",
			       &VertexNumber, &x, &y, &z) != 4) {
			std::cerr << "vertex not found in data file: " 
				  << filename << std::endl << std::flush;
			return -1;
		    }
		    else {
			// insert the vertex in a data structure
		    
			MyMathTypes::vector3_type Vertex(x, y, z);
			Vertices.push_back(Vertex);

#if 0
			std::cout << "Vertex " << std::setw(3) << VertexNumber << ": ";

			std::cout.precision(4);
			std::cout << "(x, y, z) = (" 
				  << std::setw(6) << x << ", " 
				  << std::setw(6) << y << ", " 
				  << std::setw(6) << z 
				  << ')' << std::endl;
#endif

			if (VertexNumber == NumberOfVertices) {
			    //std::cout << "Number of Vertices in list: " << Vertices.size() << std::endl;
			    state = PATCHNAME;
			}

		    }
		}
		break;
	    case PATCHNAME:
		if (InputBuffer[0] == '#') {
		    if (strlen(InputBuffer) > 2) {
			// read the name of the patch
		    
			if (sscanf(InputBuffer, "# %s", PatchName) != 1) {
			    std::cerr << "patch name not found" << std::endl << std::flush;
			    return -1;
			}
			//std::cout << "patch name: " << PatchName << std::endl << std::flush;
			state = SEARCH_PATCHES;
		    }
		}
		break;
	    case SEARCH_PATCHES:
	    case READ_PATCHES:
		if (InputBuffer[0] == '#') {
		    if (state == READ_PATCHES) {
			state = PATCHNAME;
		    }
		}
		else {
		    state = READ_PATCHES;
		    if (sscanf(InputBuffer,
			       "%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
			       &PatchNumber,
			       &index_11, &index_12, &index_13, &index_14,
			       &index_21, &index_22, &index_23, &index_24,
			       &index_31, &index_32, &index_33, &index_34,
			       &index_41, &index_42, &index_43, &index_44) != 17) {
			std::cerr << "No patch found in data file: " << filename 
				  << std::endl << std::flush;
			return -1;
		    }
		    else {
			// insert patch in a data structure
		    
			graphics::MyMathTypes::bezier_patch BPatch;

			// First row
			BPatch[1][1] = Vertices[index_11-1];
			BPatch[1][2] = Vertices[index_12-1];
			BPatch[1][3] = Vertices[index_13-1];
			BPatch[1][4] = Vertices[index_14-1];

			// Second row
			BPatch[2][1] = Vertices[index_21-1];
			BPatch[2][2] = Vertices[index_22-1];
			BPatch[2][3] = Vertices[index_23-1];
			BPatch[2][4] = Vertices[index_24-1];

			// Third row
			BPatch[3][1] = Vertices[index_31-1];
			BPatch[3][2] = Vertices[index_32-1];
			BPatch[3][3] = Vertices[index_33-1];
			BPatch[3][4] = Vertices[index_34-1];

			// Forth row
			BPatch[4][1] = Vertices[index_41-1];
			BPatch[4][2] = Vertices[index_42-1];
			BPatch[4][3] = Vertices[index_43-1];
			BPatch[4][4] = Vertices[index_44-1];

			BezierPatches.push_back(BPatch);

#if 0
			std::cout << "patch number: " 
				  << std::setw(2) << PatchNumber << std::endl << std::flush;

			std::cout << "patch indices: " << std::endl << '\t'
				  << std::setw(6) << index_11 << ", " 
				  << std::setw(6) << index_12 << ", " 
				  << std::setw(6) << index_13 << ", " 
				  << std::setw(6) << index_14 << std::endl;
		    
			std::cout << '\t'
				  << std::setw(6) << index_21 << ", " 
				  << std::setw(6) << index_22 << ", "
				  << std::setw(6) << index_23 << ", " 
				  << std::setw(6) << index_24 << std::endl;
		    
			std::cout << '\t'
				  << std::setw(6) << index_31 << ", " 
				  << std::setw(6) << index_32 << ", "
				  << std::setw(6) << index_33 << ", " 
				  << std::setw(6) << index_34 << std::endl;
		    
			std::cout << '\t'
				  << std::setw(6) << index_41 << ", " 
				  << std::setw(6) << index_42 << ", "
				  << std::setw(6) << index_43 << ", " 
				  << std::setw(6) << index_44 << std::endl;
#endif
		    }
		}
		break;
	    }
	}
	if (!data_file.eof()) {
	    std::cerr << "Something strange happend on data file: " << filename << std::endl;
	    return -1;
	}
	data_file.close();
    
#if 0
	std::cout << "The vertices in the list of Vertices:" << std::endl;
	std::cout << "=====================================" << std::endl;
	for (int i = 0; i < Vertices.size(); ++i) {
	    std::cout << "Vertex[" << std::setw(3) << i+1 << "]: [" << std::setw(6) << Vertices[i] << "]"
		      << std::endl;
	}
    
	std::cout << "The Bezier Patches read:" << std::endl;
	std::cout << "========================" << std::endl;
	for (int i = 0; i < BezierPatches.size(); ++i) {
	    std::cout << "Bezier Patch[" << std::setw(2) << i+1 << "]: [" << BezierPatches[i] << "]"
		      << std::endl << std::endl;
	}
#endif
	//std::cout << "<--ReadBezierPatches(...)" << std::endl;

	return 0;
    }

}

#endif
