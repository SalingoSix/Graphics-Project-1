// Fragment shader
#version 420

in vec3 color;
in vec3 vertNormal;
in vec3 vecWorldPosition;		// ADDED 

//uniform vec4 diffuseColour;		// New variable

// The values our OBJECT material
uniform vec4 materialDiffuse;	
uniform vec4 materialAmbient;   		// 0.2
uniform float ambientToDiffuseRatio; 	// Maybe	// 0.2 or 0.3
uniform vec4 materialSpecular;  // rgb = colour of HIGHLIGHT only
								// w = shininess of the 
uniform vec3 eyePosition;	// Camera position


/*****************************************************/
struct sLightDesc {
	vec4 position;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;		// Colour (xyz), intensity (w)
	vec4 attenuation;	// x = constant, y = linear, z = quadratic
	vec4 direction;
	vec4 typeParams;	// x = type, y = distance cut-off
	                    // z angle1, w = angle2
};

const int NUMBEROFLIGHTS = 10;
uniform sLightDesc myLight[NUMBEROFLIGHTS];

// This function has now been moved to the bottom of the file...
// Calculate the contribution of a light at a vertex
vec3 calcLightColour( in vec3 vecNormal, 
                      in vec3 vecWorldPosition, 
                      in int lightID );
/*****************************************************/



void main()
{	
	//gl_FragColor = (diffuseColour * 0.000001f);	
	//gl_FragColor += vec4(vertNormal,1.0f);  
	//
	// Calculate colour contribution for only light #0
//	gl_FragColor.rgb = calcLightColour( vertNormal, vecWorldPosition, 0 );

	// Set to black...
	gl_FragColor.rgb = vec3(0.0f, 0.0f, 0.0f);
	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
	{
		gl_FragColor.rgb += calcLightColour( vertNormal, vecWorldPosition, index );
	}
	
	// 
	gl_FragColor.a = 1.0f;	// set 4th value to 1 if unsure
	
	// Screen is so dim...
	gl_FragColor *= 1.5f;	// 150% brighter
	
//	gl_FragColor += vec4( 1.0f, 0.0f, 0.0f, 1.0f );
}

// Inspired by Mike Bailey's Graphics Shader book
// (when you see "half-vector", that's eye space)
// Technically, a "per-pixel" or a form of Phong shading
// Calcualte the contribution of a light at a vertex
vec3 calcLightColour( in vec3 vecNormal, 
                      in vec3 vecWorldPosition, 
                      in int lightID )
{
	vec3 colour = vec3( 0.0f, 0.0f, 0.0f );
	
	vec3 outDiffuse = vec3( 0.0f, 0.0f, 0.0f );
	vec3 outSpecular = vec3( 0.0f, 0.0f, 0.0f );
	

	// Distance between light and vertex (in world)
	vec3 vecToLight = myLight[lightID].position.xyz - vecWorldPosition;
	// How long is this vector?
	float lightDistance = length(vecToLight);
	// Short circuit distance
	if ( myLight[lightID].typeParams.y <  lightDistance )
	{
		return colour;
	}
	
	// The "normalized" vector to the light (don't care about distance, just direction)
	vec3 lightVector = normalize( vecToLight );
	
	// The normalized version of the vertex normal
	vecNormal = normalize(vecNormal);
	
	// How much diffuse light is being "reflected" off the surface 
	float diffFactor = max(0.0f, dot( lightVector, vecNormal ));
	
	outDiffuse.rgb = myLight[lightID].diffuse.rgb 		// Light contribution
	                 * materialDiffuse.rgb				// Material contribution
					 * diffFactor;						// Factor based on direction

// Simple linear attenuation
//	float attenuation = 1.0f / lightDistance; 
	//
	// From the "Shader" book article
	// 1.0 dividedBy ( constant 
	//                + linear * distance 
	//                + quadratic * distance * distance)
	// Our light description is: 
	// 	vec4 attenuation;	// x = constant, 
	//                         y = linear, 
	//                         z = quadratic
	float attenuation = 1.0f / 
	   ( myLight[lightID].attenuation.x // 0  
	   + myLight[lightID].attenuation.y * lightDistance			// Linear
	   + myLight[lightID].attenuation.z * lightDistance * lightDistance );	// Quad
	outDiffuse.rgb *= attenuation;

	colour = outDiffuse;
	
	return colour;
}// vec3 calcLightColour(...) 