uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProjection;

// "Vertex" attribute
attribute vec3 vCol;
attribute vec3 vPos;		// was: vec2 vPos
attribute vec3 vNorm;

out vec3 color;
out vec3 vertNormal;		// Also in "world" (no view or projection)
out vec3 vecWorldPosition;	// ADDED (no View or projection)


void main()
{	
	vec3 position = vPos;
	
	// Calculate the model view projection matrix here
	mat4 MVP = mProjection * mView * mModel;
	
	//Multiply the vertex position by the transformation matrix
	gl_Position = MVP * vec4(position, 1.0f);

	//Identify the vertex's position in the world (minus camera positioning)
	vecWorldPosition = vec3( mModel * vec4(position, 1.0f) ).xyz;
	
	//Inverse transpose vertex, and calculate the normal
	mat4 mWorldInTranspose = inverse(transpose(mModel));
	vertNormal = vec3( mWorldInTranspose * vec4(vNorm, 1.0f) ).xyz;		
	
    color = vCol;
}
